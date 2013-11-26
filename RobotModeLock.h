#pragma once
#ifndef _ROBOTMODE_
#define _ROBOTMODE_
#include "RobotCondition.h"
/*
 *Powered by Renzhenwen
 *Email:admin@unix8.net
 * */
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
class RobotModeLock
{
public:
	typedef enum RobotRunMode
	{
		MODE_JOYSTICK = 0x00,
		MODE_WANDER = 0x01,
		MODE_TEADM = 0x02
	}RobotRunMode;
	RobotModeLock(void);
	~RobotModeLock(void);
	//获取当前模式
	RobotRunMode	getMode(void) const {return mode;}
	//设置当前模式
	void setMode(RobotRunMode flag) {mode = flag;}
	//上锁
	int lock(void) {return cond.lock();}
	//解锁
	int unlock(void) {return cond.unlock();}
	//信号通知
	int lockSignal(void) {return cond.signal();}
	//等待
	int lockWait(void) {return cond.wait();}
	//计时等待
	int lockWaitTime(int msec) {return cond.timedWait(msec);}   //防止错过信号，导致智能模式一直被阻塞
private:
	//为了使用wait来阻塞并同步
	RobotCondition cond;	
	//0 为手柄模式， 1为智能模式
	RobotRunMode mode;			
};
#ifdef ROBOT_NAMESPACE
}
#endif
#endif
