// noncopyable.h
// Created by Lixin on 2020.03.05

#ifndef MAIN_BASE_NONCOPYABLE_H
#define MAIN_BASE_NONCOPYABLE_H

namespace main
{

class copyable
{
protected:
	copyable() = default;
	~copyable() = default;
};

} // namespace main

#endif // MAIN_BASE_COPYABLE_H
