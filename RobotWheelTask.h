#pragma once
#ifndef ROBOT_MAIN_WHEELTASK_H_
#define ROBOT_MAIN_WHEELTASK_H_

#include "RobotPacket.h"
#include "RobotTask.h"
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif


//轮子控制任务
class RobotWheelTask : public RobotTask  
{
protected:
	//左轮速度
	short leftWheelSpeed;
	//右轮速度
	short rightWheelSpeed;
#ifdef ROBOT_RUNTIME
	//运行时间
	unsigned char runTime;
#endif
	//发送至机器人的数据帧
	RobotPacket robotPacket;
public:

	RobotWheelTask(short l = 0, short r = 0,
#ifdef ROBOT_RUNTIME
	unsigned char t = 0,
#endif
	unsigned char tPriority = RobotTask::Common);



	RobotWheelTask(short l, short r,
#ifdef ROBOT_RUNTIME
	unsigned char t,
#endif
 	std::string, unsigned char tPriority = RobotTask::Common);

	~RobotWheelTask(void) throw();

	RobotWheelTask(const RobotWheelTask &t);

	RobotWheelTask& operator=(const RobotWheelTask& t);

	std::string getTaskName(void) const;

	unsigned char getPriority(void) const;
	



	//设置左轮速度，右轮速度，运行时间
	void setWheel(short l,short r
#ifdef ROBOT_RUNTIME
	, unsigned char t
#endif
	)
	{
		leftWheelSpeed = l;
		rightWheelSpeed = r;
#ifdef ROBOT_RUNTIME
		runTime = t;
#endif
	}  




	//执行任务函数
	void doTask()
	{
		robotPacket.empty();
		robotPacket.byte2ToBuf(leftWheelSpeed);
		robotPacket.byte2ToBuf(rightWheelSpeed);
#ifdef ROBOT_RUNTIME
		robotPacket.byteToBuf(runTime);
#endif

#ifdef PACKET_HAVE_ID
		robotPacket.setId(commandNo);
		(++commandNo) %= 128;
#endif
		robotPacket.finalizePacket(); 
		//robotPacket.printHex();
	}



	unsigned char* getTaskBuf()
	{
		return robotPacket.getBuf(); 
	}




	int getTaskBufLen()
	{
		return robotPacket.getLength();
	}
	void printPacket()
	{
		robotPacket.printHex();
	}
};
#ifdef ROBOT_NAMESPACE
}
#endif

#endif
