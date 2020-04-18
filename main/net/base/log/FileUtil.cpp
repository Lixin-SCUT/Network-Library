//
// Created by 黎鑫 on 2020/4/16.
//


#include <cstdlib>
#include "FileUtil.h"

AppendFile::AppendFile(const char* filename)
    : fp_(fopen(filename, "ae"))
{
    setbuf(fp_, buffer_);
}

AppendFile::~AppendFile()
{
    if(0 != fclose(fp_))
    {
        perror("ERROR : fclose() for fp_");
        exit(-1);
    }
}

void AppendFile::Append(const char *logline, size_t len)
{
    size_t n = fwrite(logline, 1, len, fp_);
    size_t remain = len - n;
    size_t n_temp = 0;
    while(remain > 0)
    {
        n_temp = fwrite(logline + n, 1, remain, fp_);
        if(0 == n_temp)
        {
            perror("ERROR : Append() for fp_");
            exit(-1);
        }
        n += n_temp;
        remain -= n_temp;
    }
}

void AppendFile::Flush()
{
    fflush(fp_);
}






