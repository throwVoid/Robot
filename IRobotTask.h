#pragma once
#ifndef ROBOT_MAIN_ITASK_H_
#define ROBOT_MAIN_ITASK_H_

#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif

//任务接口虚基类
class IRobotTask
{
public:

	virtual ~IRobotTask(void){}

	virtual std::string getTaskName(void) const = 0;

	virtual void doTask(void) = 0;

	virtual unsigned char* getTaskBuf() = 0;
	
	virtual int getTaskBufLen() = 0;
};

#ifdef ROBOT_NAMESPACE
}
#endif
#endif

