#include "RobotLightTask.h"

#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif

RobotLightTask::RobotLightTask(unsigned char no, unsigned char action, unsigned char tPriority): 
RobotTask(tPriority, "light ctl")
,lightNo(no)
,lightAction(action)
,robotPacket(NULL  //robotPacket Construct.....
,10
,RobotPacket::Start
,RobotPacket::End
,RobotPacket::ControlLightCommand)//robotPacket Construct  end.....
{
}


RobotLightTask::RobotLightTask(unsigned char no, unsigned char action, std::string taskName, unsigned char tPriority): 
RobotTask(tPriority, taskName)
,lightNo(no)
,lightAction(action)
,robotPacket(NULL  //robotPacket Construct.....
,10
,RobotPacket::Start
,RobotPacket::End
,RobotPacket::ControlLightCommand)//robotPacket Construct  end.....
{
}


RobotLightTask::RobotLightTask(const RobotLightTask &t) : 
RobotTask(t)
,lightNo(t.lightNo)
,lightAction(t.lightAction)
{
}

RobotLightTask& RobotLightTask::operator=(const RobotLightTask& t)
{
	RobotTask::operator=(t);
	lightNo = t.lightNo;
	lightAction = t.lightAction;
	return *this;
}

#ifdef ROBOT_NAMESPACE
}
#endif
