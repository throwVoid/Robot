#pragma once
#ifndef ROBOT_MAIN_MUTEX_H_
#define ROBOT_MAIN_MUTEX_H_
/*
 * Power by RenZhenwen
 * date:2012-12-07
 * Email: admin@unix8.net
 * */

#include <pthread.h>
//#include <string>
//#include <map>
#include "RobotDeBug.h"

#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
class RobotMutex
{
public:
	typedef pthread_mutex_t MutexType;
	typedef enum 
	{
		STATUS_FAILED_INIT=1,		//初始化失败
		STATUS_FAILED, 					//不明失败
		STATUS_ALREADY_LOCKED 	//已经上锁
	} Status;
//	typedef std::map<int, std::string> StrMap;
	RobotMutex(void);
	~RobotMutex(void);
	int lock(void);						//Linux下mutex的操作包装函数
	int tryLock(void);
	int unlock(void);
//	const char * getError(int messageNumber) const;
	MutexType& getMutex(void) {return(myMutex);}
protected:
	bool myFailedInit;
	MutexType myMutex;
//	StrMap myStrMap;
};
#ifdef ROBOT_NAMESPACE
}
#endif
#endif//ROBOT_MAIN_MUTEX_H_
