//
// Created by 黎鑫 on 2020/4/23.
//

#include "HttpSession.h"
#include "../log/Logging.h"

#include <iostream>
#include <functional>
#include <memory>

using std::stringstream;
using std::to_string;

HttpSession::HttpSession()
    : prase_result_(false),
      keepalive_(true)
{}

HttpSession::~HttpSession() {}

bool HttpSession::PraseHttpRequest(string& msg, HttpRequestContext& http_request_context)
{
    string crlf("\r\n");
    string crlfcrlf("\r\n\r\n");
    int prev = 0;
    int next = 0;
    int pos_colon;
    string key;
    string value;

    prase_result_ = false;

    if((next = msg.find(crlf)) != string::npos )
    {
        string first_line(msg.substr(prev, next - prev));
        prev = next;
        stringstream sstream(first_line);
        sstream >> http_request_context.method;
        sstream >> http_request_context.url;
        sstream >> http_request_context.version;
    }
    else
    {
        LOG << "ERROR in PraseHttpRequest() : http_request_line isn't complete!";
        msg.clear();
        return prase_result_;
    }

    int pos_crlfcrlf = 0;
    if((pos_crlfcrlf = msg.find(crlfcrlf)) != string::npos)
    {
        while(prev != pos_crlfcrlf)
        {
            next = msg.find(crlf, prev + 2);
            pos_colon = msg.find(":", prev + 2);
            key = msg.substr((prev + 2, pos_colon - prev - 2));
            value = msg.substr((pos_colon + 2, next - pos_colon - 2));
            prev = next;
            http_request_context.header.insert(make_pair(key, value));
        }
    }
    else
    {
        LOG << "ERROR in PraseHttpRequest() : http_request_header isn't complete!";
        msg.clear();
        return prase_result_;
    }

    http_request_context.body = msg.substr(pos_crlfcrlf + 4);
    prase_result_ = true;
    msg.clear();
    return prase_result_;
}

void HttpSession::HttpProcess(const HttpRequestContext &http_request_context, string &response_context)
{
    string response_body;
    string error_msg;
    string path;
    string query_string;

    if(http_request_context.method == "GET")
    { ; }
    else if(http_request_context.method == "POST")
    { ; }
    else
    {
        error_msg = "Method Not Implemented";
        HttpError(501, error_msg, http_request_context, response_context);
        return;
    }

    int pos = http_request_context.url.find("?");
    if(pos != string::npos)
    {
        path = http_request_context.url.substr(0, pos);
        query_string = http_request_context.url.substr(pos + 1);
    }
    else
    {
        path = http_request_context.url;
    }

    map<string, string>::const_iterator iter = http_request_context.header.find("Connection");
    if(iter != http_request_context.header.end())
    {
        keepalive_ = (iter->second == "Keep-Alive");
    }
    else
    {
        if(http_request_context.version == "HTTP/1.1")
        {
            keepalive_ = true;
        }
        else
        {
            keepalive_ = false;
        }
    }

    if(path == "/")
    {
        path = "/default.html";
    }
    else if(path == "/hello")
    {
        string filetype("text/html");
        response_body = "Hallo World!";
        response_context += http_request_context.version + " 200 OK\r\n";
        response_context += "Server: Lixin's HttpServer supported by NetworkLibrary/0.1\r\n";
        response_context += "Content-Type: " + filetype + "; charset=utf-8\r\n";
        if(iter != http_request_context.header.end())
        {
            response_context += "Connection: " + iter->second + "\r\n";
        }
        response_context += "Content-Length: " + to_string(response_context.size()) + "\r\n";
        response_context += "\r\n";
        response_context += response_body;
        return;
    }
    else
    {
        ;
    }

    path.insert(0, ".");
    FILE* fp = nullptr;
    if((fp = fopen(path.c_str(), "rb")) == nullptr)
    {
        HttpError(404, "Not Found", http_request_context, response_context);
        return;
    }
    else
    {
        char buffer[4096];
        memset(buffer, 0 , sizeof(buffer));
        while(fread(buffer, sizeof(buffer), 1, fp) == 1)
        {
            response_body.append(buffer);
            memset(buffer, 0, sizeof(buffer));
        }
        if(feof(fp))
        {
            response_body.append(buffer);
        }
        else
        {
            LOG << "ERROR in HttpProcess() : error occurs in fread.";
        }
        fclose(fp);
    }

    string filetype = "text/html";
    response_context += http_request_context.version + " 200 OK\r\n";
    response_context += "Server: Lixin's HttpServer supported by NetworkLibrary/0.1\r\n";
    response_context += "Content-Type: " + filetype + "; charset=utf-8\r\n";
    if(iter != http_request_context.header.end())
    {
        response_context += "Connection: " + iter->second + "\r\n";
    }
    response_context += "Content-Length: " + to_string(response_context.size()) + "\r\n";
    response_context += "\r\n";
    response_context += response_body;
}

void HttpSession::HttpError(const int error,
                            const string short_msg,
                            const HttpRequestContext &http_request_context,
                            string &response_context)
{
    string response_body;
    response_body += "<html><title>出错了</title>";
    response_body += "<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"></head>";
    response_body += "<style>body{background-color:#f;font-size:14px;}h1{font-size:60px;color:#eeetext-align:center;padding-top:30px;font-weight:normal;}</style>";
    response_body += "<body bgcolor=\"ffffff\"><h1>";
    response_body += std::to_string(error) + " " + short_msg;
    response_body += "</h1><hr><em> Lixin's HttpServer supported by NetworkLibrary</em>\n</body></html>";

    string http_version;
    if(http_request_context.version.empty())
    {
        http_version = "HTTP/1.1";
    }
    else
    {
        http_version = http_request_context.version;
    }

    response_context += http_version + " " + to_string(error) + " " + short_msg + "\r\n";
    response_context += "Server: Lixin's HttpServer supported by NetworkLibrary/0.1\r\n";
    response_context += "Content-Type: text/html\r\n";
    response_context += "Connection: Keep-Alive\r\n";
    response_context += "Content-Length: " + std::to_string(response_body.size()) + "\r\n";
    response_context += "\r\n";
    response_context += response_body;
}