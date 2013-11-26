#include "RobotWheelTask.h"


#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif


//不带描述的构造函数
RobotWheelTask::RobotWheelTask(short l, 
short r,
#ifdef ROBOT_RUNTIME
unsigned char t, 
#endif
unsigned char tPriority) : 
RobotTask(tPriority)
,leftWheelSpeed(l)
,rightWheelSpeed(r)
#ifdef ROBOT_RUNTIME
,runTime(t) 
#endif
,robotPacket(NULL, 25, RobotPacket::Start, RobotPacket::End, RobotPacket::ControlWheelCommand) 
{ 
}


//带描述的构造函数
RobotWheelTask::RobotWheelTask(short l, 
short r,
#ifdef ROBOT_RUNTIME
unsigned char t, 
#endif
std::string tTaskName,
unsigned char tPriority) : 
RobotTask(tPriority, tTaskName)
,leftWheelSpeed(l)
,rightWheelSpeed(r)
#ifdef ROBOT_RUNTIME
,runTime(t) 
#endif
,robotPacket(NULL, 25, RobotPacket::Start, RobotPacket::End, RobotPacket::ControlWheelCommand) 
{ 
}

//拷贝构造函数
RobotWheelTask::RobotWheelTask(const RobotWheelTask &t) : RobotTask(t)
,leftWheelSpeed(t.leftWheelSpeed) 
,rightWheelSpeed(t.rightWheelSpeed)
#ifdef ROBOT_RUNTIME
,runTime(t.runTime)
#endif
{
}

//=操作符
RobotWheelTask& RobotWheelTask::operator=(const RobotWheelTask& t)
{
		RobotTask::operator=(t);
		leftWheelSpeed = t.leftWheelSpeed;
		rightWheelSpeed = t.rightWheelSpeed;
		#ifdef ROBOT_RUNTIME
		runTime = t.runTime;
		#endif
		return *this;
}

RobotWheelTask::~RobotWheelTask(void) throw()
{
}

std::string RobotWheelTask::getTaskName(void) const
{
	return "轮子控制任务"+RobotTask::getTaskName();
}

unsigned char RobotWheelTask::getPriority(void) const
{
	return RobotTask::getPriority();
}

#ifdef ROBOT_NAMESPACE
}
#endif
