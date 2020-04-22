//
// Created by 黎鑫 on 2020/4/21.
//

#include "TcpConnection.h"
#include "log/Logging.h"
#include <unistd.h>

using std::bind;
using std::this_thread::get_id;


const int kBufSize = 4096;

int sendn(int fd, std::string& buffer_out);
int recvn(int fd, std::string& buffer_in);

TcpConnection::TcpConnection(
        EventLoop *eventLoop,
        int fd,
        const struct sockaddr_in &clientaddr)
                : fd_(fd),
                  loop_(eventLoop),
                  client_addr_(clientaddr),
                  channel_ptr_(new Channel()),
                  halfclose_(false),
                  disconnected_(false),
                  // asyncprocessing_(false),
                  buffer_in_(),
                  buffer_out_()
{
    channel_ptr_->SetFd(fd_);
    channel_ptr_->SetEvent(EPOLLIN | EPOLLET);
    channel_ptr_->SetReadCallBack(bind(&TcpConnection::HandleRead, this));
    channel_ptr_->SetWritableCallBack(bind(&TcpConnection::HandleWrite, this));
    channel_ptr_->SetErrorCallBack(bind(&TcpConnection::HandleError, this));
    channel_ptr_->SetClosedCallBack(bind(&TcpConnection::HandleClose, this));
}

TcpConnection::~TcpConnection()
{
    loop_->RemovePoller(channel_ptr_.get());
    close(fd_);
}

void TcpConnection::AddChannelToLoop()
{
    loop_->QueueInLoop(bind(&EventLoop::AddToPoller,loop_, channel_ptr_.get()));
}

void TcpConnection::Send(const string &str)
{
    buffer_out_ += str;
    if(loop_->GetTid() == get_id())
    {
        SendInLoop();
    }
    else
    {
        // asyncprocessing_ = false;
        loop_->QueueInLoop(bind(&TcpConnection::SendInLoop, shared_from_this()));
    }
}

void TcpConnection::SendInLoop()
{
    if(disconnected_)
    {
        LOG << "WARN : connection has been closed ! ";
        return;
    }
    int n = sendn(fd_, buffer_out_);
    if(n > 0)
    {
        uint32_t events = channel_ptr_->GetEvents();
        if(buffer_out_.size() > 0)
        {
            channel_ptr_->SetEvent(events | EPOLLOUT);
            loop_->UpdatePoller(channel_ptr_.get());
        }
        else
        {
            channel_ptr_->SetEvent(events & (~EPOLLOUT));
            sendcomplete_callback_(shared_from_this());
            if(halfclose_)
            {
                HandleClose();
            }
        }
    }
    else if(n < 0)
    {
        HandleError();
    }
    else
    {
        HandleClose();
    }
}
void::TcpConnection::ShutDown()
{
    if(loop_->GetTid() == get_id())
    {
        ShutDownInLoop();
    }
    else
    {

        loop_->QueueInLoop(bind(&TcpConnection::ShutDownInLoop, shared_from_this()));
    }
}

void TcpConnection::ShutDownInLoop()
{
    if(disconnected_) { return; }
    LOG << "INFO : connection is shutdowning .";
    close_callback_(shared_from_this());
    loop_->QueueInLoop(bind(&TcpConnection::connectioncleanup_,shared_from_this()));
    disconnected_ = true;
}



void TcpConnection::HandleRead()
{
    int n = recvn(fd_, buffer_in_);

    if(n > 0)
    {
        message_callback_(shared_from_this(), buffer_in_);
    }
    else if(n == 0)
    {
        HandleClose();
    }else
    {
        HandleError();
    }
}

void TcpConnection::HandleWrite()
{
    int n = sendn(fd_, buffer_out_);
    if(n > 0)
    {
        uint32_t events = channel_ptr_->GetEvents();
        if(buffer_out_.size() > 0)
        {
            channel_ptr_->SetEvent(events | EPOLLOUT);
            loop_->UpdatePoller(channel_ptr_.get());
        }
        else
        {
            channel_ptr_->SetEvent(events & (~EPOLLOUT));
            sendcomplete_callback_(shared_from_this());
            if(halfclose_)
            {
                HandleClose();
            }
        }
    }
    else if(n < 0)
    {
        HandleError();
    }
    else
    {
        HandleClose();
    }
}

void TcpConnection::HandleError()
{
    if(disconnected_) { return; }
    error_callback_(shared_from_this());
    loop_->QueueInLoop(bind(&TcpConnection::connectioncleanup_, shared_from_this()));
    disconnected_ = true;
}

void TcpConnection::HandleClose()
{
    if(disconnected_) { return; }

    //  if(buffer_out_.size() > 0 || buffer_in_.size() > 0 || asyncprocessing_ )
    if(buffer_out_.size() > 0 || buffer_in_.size() > 0 )
    {
        halfclose_ = true;
        if(buffer_in_.size() > 0)
        {
            message_callback_(shared_from_this(), buffer_in_);
        }
    }
    else
    {
        loop_->QueueInLoop(bind(&TcpConnection::connectioncleanup_,shared_from_this()));
        close_callback_(shared_from_this());
        disconnected_ = true;
    }
}

int sendn(int fd, std::string& buffer_out)
{
    int n = 0;
    int sum = 0;
    int length = buffer_out.size();

    while(true)
    {
        n = write(fd, buffer_out.c_str(), buffer_out.size());


        if(n > 0)
        {
            sum += n;
            buffer_out.erase(0, n);
            if (sum == length)
            {
                return length;
            }

        }
        else if(n < 0)
        {
                if(errno == EAGAIN)
                {
                    LOG << "WARN : EAGAIN occurs in sendn().";
                    return sum;
                }
                else if(errno == EINTR)
                {
                    LOG << "WARN : EINTR occurs in sendn().";
                    continue;
                }
                else if(errno == EPIPE)
                {
                    LOG << "ERROR : EPIPE occurs in sendn().";
                    return -1;
                }
                else
                {
                    LOG << "ERROR : unknown error occurs in sendn().";
                    return -1;
                }
        }
        else
        {
            return 0;
        }
    }
}

int recvn(int fd, std::string& buffer_in)
{
    int n = 0;
    int sum = 0;
    char buffer[kBufSize];

    while(true)
    {
        n = read(fd, &buffer, kBufSize);
        if(n > 0)
        {
            sum += n;
            buffer_in.append(buffer, n);
            if(n < kBufSize)
            {
                return sum;
            }
            else
            {
                continue;
            }
        }
        else if(n < 0)
        {
            if(errno == EAGAIN)
            {
                LOG << "WARN : EAGAIN occurs in recvn().";
                return sum;
            }
            else if(errno == EINTR)
            {
                LOG << "WARN : EINTR occurs in recvn().";
                continue;
            }
            else
            {
                LOG << "WARN : unknown error occurs in recvn().";
                return -1;
            }
        }
        else if(n == 0)
        {
            LOG << "INFO : Client shutdown the connection.";
            return 0;
        }
    }
}