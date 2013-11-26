#pragma once
#ifndef ROBOT_MAIN_TASK_H_
#define ROBOT_MAIN_TASK_H_

#include "RobotPacket.h"
#include "IRobotTask.h"


#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif



class RobotTask : public IRobotTask
{
public:
	//任务优先级
	typedef enum TaskLevel 
	{
		High = 0x01, 
		Middle = 0x02, 
		Common = 0x04, 
		Lower = 0x08
	}TaskLevel;

	#define INVALID_LEN -1
	#define INVALID_BUF_PTR NULL

	RobotTask(const RobotTask& t);
	RobotTask& operator=(const RobotTask& t);
	RobotTask(unsigned char);
	RobotTask(unsigned char, std::string);

	unsigned char getCommandNo(void) const;
	unsigned char getPriority(void) const;

	virtual ~RobotTask(void) throw();								
	virtual std::string getTaskName(void) const;
	virtual void doTask(void);										
	virtual unsigned char* getTaskBuf();
	virtual int getTaskBufLen();
protected:
	//任务优先级											
	unsigned char priority;	
	//统计发送控制包的个数
	unsigned char commandNo;
	std::string taskName;
private:
	static unsigned char commandsCount;
};


#ifdef ROBOT_NAMESPACE
}
#endif
#endif//ROBOT_MAIN_TASK_H_
