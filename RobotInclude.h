#pragma once
#ifndef ROBOT_MAIN_MYGLOBE_H_
#define ROBOT_MAIN_MYGLOBE_H_
#include "Robot.h"
#include "RobotPathPlan.h"
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif




#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
						  void operator=(const TypeName&)



extern void signalFunc(int signo);


#ifdef ROBOT_NAMESPACE
}
#endif

#endif
