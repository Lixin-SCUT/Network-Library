//
// Created by 黎鑫 on 2020/4/16.
//

#ifndef MYPROJECT_NONCOPYABLE_H
#define MYPROJECT_NONCOPYABLE_H

class noncopyable
{
public:

    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;

protected:
    noncopyable() = default;
    ~noncopyable() = default;
};

#endif //MYPROJECT_NONCOPYABLE_H
