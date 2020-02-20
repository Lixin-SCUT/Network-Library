// Buffer.cc
// Created by Lixin on 2020.02.20

#include "main/net/Buffer.h"

#include "main/net/SocketsOps.h"

#include <errno.h> // for errno
#include <sys/uio.h> // for readv and iovec

using namespace main;
using namespace main::net;

const char Buffer::kCRLF = "\r\n";

const size_t Buffer::kCheapPrepend;
const size_t Buffer::kInitialSize;

ssize_t Buffer::readFd(int fd, int* savedErrno)
{
	//saved an ioctl()/FIONREAD call to tell how much to read
	char extrabuf[65536];
	struct iovec vec[2];
	const size_t writable = writableBytes();
	vec[0].iov_base = begin() + writerIndex_;
	vec[0].iov_len = writable;
	vec[1].iov_base = extrabuf;
	vec[1].iov_len = sizeof(extrabuf);
	// when there is enough space in buffer,do not use extrabuf
	// when extrabuf is used, we read 128k-1 bytes at most
	const int iovcnt = (writable < sizeof(extrabuf)? 2 : 1);
	const ssize_t sockets::readv(fd,vec,iovcnt);
	if(n < 0)
	{
		*savedErrno = errno;
	}
	else if(implicit_cast<size_t>(n) <= writable)
	{
		writerIndex_ += n;
	}
	else
	{
		writerIndex_ = buffer_.size();
		append(extrabuf,n - writable);
	}
	
	return n;
}
