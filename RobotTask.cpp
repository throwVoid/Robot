/*
 *date: 2012/11/6
 *author:Renzhenwen
 *任务类
 */
#include "RobotTask.h"


#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif


unsigned char RobotTask::commandsCount = 0;

RobotTask::RobotTask(unsigned char tPriority) : 
priority(tPriority)
,commandNo(++commandsCount)
,taskName("no task name")
{
}


RobotTask::RobotTask(unsigned char tPriority, std::string tTaskName) : 
priority(tPriority)
,commandNo(++commandsCount)
,taskName(tTaskName)
{
}



RobotTask::~RobotTask(void) throw()
{
}



RobotTask::RobotTask(const RobotTask& t)
{
	priority = t.priority;
	commandNo = ++commandsCount;
}



RobotTask& RobotTask::operator=(const RobotTask& t)
{
	priority = t.priority;
	return(*this);
}



std::string RobotTask::getTaskName(void) const
{
	return taskName;
}


unsigned char RobotTask::getCommandNo(void) const
{
	return commandNo;
}



unsigned char RobotTask::getPriority(void) const
{
	return priority;
}

void RobotTask::doTask(void)	
{
	return;
}

unsigned char* RobotTask::getTaskBuf()
{
	return INVALID_BUF_PTR;
}


int RobotTask::getTaskBufLen()
{
	return INVALID_LEN;
}

#ifdef ROBOT_NAMESPACE
}
#endif
