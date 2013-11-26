#include <errno.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <assert.h>
#include <stdio.h>
#include "RobotCondition.h"
/*
 * Powered by RenZhenwen
 * date:2012-12-07
 *
 * */
//map<int,string>  RobotCondition::myStrMap;
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
RobotCondition::RobotCondition(void) : myMutex(), myFailedInit(false) , myCond()
{
	pthread_condattr_t attr;
	pthread_condattr_init(&attr);
	if (pthread_cond_init(&myCond, &attr) != 0)
	{
		myCond = PTHREAD_COND_INITIALIZER;
		myFailedInit = true;
	}
	pthread_condattr_destroy(&attr);
	/*几种错误返回原因
  myStrMap[STATUS_FAILED]="未知错误";
	myStrMap[STATUS_FAILED_DESTROY]="锁被占有，不能析构";
	myStrMap[STATUS_FAILED_INIT] ="条件量初始化失败";
	myStrMap[STATUS_MUTEX_FAILED_INIT]="锁初始化失败";
	myStrMap[STATUS_MUTEX_FAILED]="锁的其他失败原因";*/
}

RobotCondition::~RobotCondition(void)
{ 
	if(!myFailedInit)
		pthread_cond_destroy(&myCond);
}

int RobotCondition::signal(void)
{
	if (myFailedInit)
		return(STATUS_FAILED_INIT);
	if (pthread_cond_signal(&myCond) != 0)
		return(STATUS_FAILED);
	return(0);
}

int RobotCondition::broadcast(void)
{
	if (myFailedInit)
		return(STATUS_FAILED_INIT);
	if (pthread_cond_broadcast(&myCond) != 0)
		return(STATUS_FAILED);
	return(0);
}
int RobotCondition::lock(void)
{
	int ret;
	ret = myMutex.lock();
	if (ret != 0)
	{
		if (ret == RobotMutex::STATUS_FAILED_INIT)
			return(STATUS_MUTEX_FAILED_INIT);
		else
			return(STATUS_MUTEX_FAILED);
	}
	return 0;
}
int RobotCondition::unlock(void)
{
	int ret = myMutex.unlock();
	if (ret != 0)
	{
		if (ret == RobotMutex::STATUS_FAILED_INIT)
		  return(STATUS_MUTEX_FAILED_INIT);
		else
		  return(STATUS_MUTEX_FAILED);
	}
	return 0;
}
int RobotCondition::wait(void)
{
	int ret;
	if (myFailedInit)
	{
		return(STATUS_FAILED_INIT);
	}
	myMutex.lock();//2013-06-01
	ret = pthread_cond_wait(&myCond, &myMutex.getMutex());
	if (ret != 0)
	{
		if (ret == EINTR)
			return(STATUS_WAIT_INTR);
		else
		{
			return(STATUS_FAILED);
		}
	}
	myMutex.unlock();
	return(0);
}

int RobotCondition::timedWait(unsigned int msecs)
{
#ifndef WIN32
	int ret;
	struct timespec spec;
	struct timeval  tp;

	if (myFailedInit)
		return(STATUS_FAILED_INIT);
	myMutex.lock();//2013-06-01
	gettimeofday(&tp, NULL);//获取时间
	spec.tv_sec = tp.tv_sec;
	spec.tv_nsec = tp.tv_usec * 1000;
	spec.tv_sec += (long int)rint(((float)msecs)/1000.0);
	spec.tv_nsec += (long int)( ( msecs % 1000 ) * 1000000); //1秒=1000毫秒=1000000纳秒
	ret=pthread_cond_timedwait(&myCond, &myMutex.getMutex(), &spec);
  
	if (ret != 0)
	{
		if (ret == EINTR)
			return(STATUS_WAIT_INTR);
		else if (ret == ETIMEDOUT)
			return(STATUS_WAIT_TIMEDOUT);
		else
			return(STATUS_FAILED);
	}
	myMutex.unlock();
#endif
	return(0);
}
#ifdef ROBOT_NAMESPACE
}
#endif
