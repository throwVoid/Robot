#pragma once
#ifndef SLEEP
#define SLEEP
/*
 * Power by Renzhenwen
 * Email: admin@unix8.net
 *
 * */
#include <unistd.h>
#include <stdlib.h>
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
class RobotSleep
{
public:
	static void RobotSleepSec(int sec);			//秒级休眠
	static void RobotSleepMsec(int msec);		//毫秒级休眠
	static	void RobotSleepUsec(int usec);  //微秒级休眠
};
#ifdef ROBOT_NAMESPACE
}
#endif
#endif
