// noncopyable.h
// Created by Lixin on 2020.03.05

#ifndef MAIN_BASE_NONCOPYABLE_H
#define MAIN_BASE_NONCOPYABLE_H

namespace main
{

class noncopyable
{
public:
	noncopyable(const noncopyable&) = delete;
	noncopyable& operator=(const noncopybale&) = delete;

protected:
	noncopyable() = default;
	~noncopyable() = default;
};

} // namespace main

#endif // MAIN_BASE_COPYABLE_H
