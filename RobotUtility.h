#pragma once
#ifndef ROBOT_MAIN_UTILITY_H_
#define  ROBOT_MAIN_UTILITY_H_

#include "robot-config.h"
#include <string>
#ifdef LINUX
#include <cxxabi.h>
#include <execinfo.h>
#include <dlfcn.h>
#endif
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif

class RobotUtility
{
public:
	static const std::string Stack();
	static std::string  l2string(long l);
	static std::string  ToString(double d);
	static std::string int642string(int64_t l);
	static std::string int642string(uint64_t l);

};


#ifdef ROBOT_NAMESPACE
}
#endif
#endif

