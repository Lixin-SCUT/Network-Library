//
// Created by 黎鑫 on 2020/4/23.
//

#ifndef MYPROJECT_HTTPSESSION_H
#define MYPROJECT_HTTPSESSION_H

#include <string>
#include <sstream>
#include <map>

using std::string;
using std::map;

typedef struct _HttpRequestContext
{
    string method;
    string url;
    string version;
    map<string, string> header;
    string body;
} HttpRequestContext;

typedef struct _HttpResponseContext
{
    string version;
    string status_code;
    string status_message;
    map<string, string> header;
    string body;
} HttpResponseContext;


class HttpSession
{
public:
    HttpSession();
    ~HttpSession();

    bool PraseHttpRequest(string& str, HttpRequestContext& http_request_context);

    void HttpProcess(const HttpRequestContext& http_request_context, string& response_context);

    void HttpError(const int error, const string short_msg, const HttpRequestContext& http_request_context, string& response_context);

    bool KeepAlive()
    {   return keepalive_; }
private:
    HttpRequestContext http_request_context_;
    bool prase_result_;
    bool keepalive_;

    //  string response_context_;
    //  string response_body_;
    //  string error_msg_;
    //  string path_;
    //  string query_string_;
    //  string body_buff_;

};
#endif //MYPROJECT_HTTPSESSION_H
