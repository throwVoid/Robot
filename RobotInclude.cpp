#include "RobotInclude.h"
#include "RobotDeBug.h"
#include <stdlib.h>
#include <unistd.h>
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
void signalFunc(int signo) 
{
	PRINT("\n机器人正在退出");
	PRINT("...");
	sleep(1);
	PRINT("机器人已经退出");
	exit(0);
}
#ifdef ROBOT_NAMESPACE
}
#endif
