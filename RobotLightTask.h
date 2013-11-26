#pragma once
#ifndef ROBOT_MAIN_LIGHTTASK_H_
#define ROBOT_MAIN_LIGHTTASK_H_

#include "RobotTask.h"

#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif



class RobotLightTask : public RobotTask  
{
private:
	//灯编号
	unsigned char lightNo;
	//灯动作
	unsigned char lightAction;
	RobotPacket robotPacket;
public:
	RobotLightTask(unsigned char no = 0, unsigned char action = 0, unsigned char tPriority = RobotTask::Common);
	RobotLightTask(unsigned char no, unsigned char action, std::string, unsigned char tPriority = RobotTask::Common);
	RobotLightTask(const RobotLightTask &t);
	RobotLightTask& operator=(const RobotLightTask& t);

	void setLight(unsigned char no, unsigned char action)
	{
		lightNo = no;
		lightAction = action;
	}  


	void doTask()
	{
		robotPacket.empty();
		robotPacket.uByteToBuf(lightNo);
		robotPacket.uByteToBuf(lightAction);
		robotPacket.finalizePacket(); 
	}


	unsigned char* getTaskBuf()
	{
		return robotPacket.getBuf(); 
	}


	int getTaskBufLen()
	{
		return robotPacket.getLength();
	}


	std::string getTaskName() const
	{
		return RobotTask::getTaskName();
	}
};

#ifdef ROBOT_NAMESPACE
}
#endif

#endif
