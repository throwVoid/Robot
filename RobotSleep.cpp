#include "RobotSleep.h"
#include <unistd.h>
/*
 * Power by Renzhenwen
 * Email: admin@unix8.net
 * */
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
void RobotSleep::RobotSleepSec(int sec)
{
	struct timeval delayTime;
	delayTime.tv_sec = sec;
	delayTime.tv_usec = 0;
	select(1,NULL, NULL, NULL, &delayTime);
}
void RobotSleep::RobotSleepMsec(int msec)
{
	struct timeval delayTime;
	delayTime.tv_sec = 0;
	delayTime.tv_usec = msec * 1000;
	select(1,NULL, NULL, NULL, &delayTime);
}
void RobotSleep::RobotSleepUsec(int usec)
{
	struct timeval delayTime;
	delayTime.tv_sec = 0;
	delayTime.tv_usec = usec;
	select(1,NULL, NULL, NULL, &delayTime);
}
#ifdef ROBOT_NAMESPACE
}
#endif
