//
// Created by 黎鑫 on 2020/4/16.
//

#ifndef MYPROJECT_FILEUTIL_H
#define MYPROJECT_FILEUTIL_H

#define BUFSIZE 64 * 1024

#include <cstdio>
#include "../base/noncopyable.h"

class AppendFile : noncopyable
{
public:
    explicit AppendFile(const char* filename);
    ~AppendFile();

    void Append(const char* logline, size_t len);
    void Flush();

private:
    FILE* fp_;
    char buffer_[BUFSIZE]; // TODO: change to pointer
};

#endif //MYPROJECT_FILEUTIL_H
