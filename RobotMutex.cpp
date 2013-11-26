#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include "RobotMutex.h"
/*
 * Power by RenZhenwen
 * date:2012-12-07
 * Email: admin@unix8.net
 * */


#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif

RobotMutex::RobotMutex(void) : myFailedInit(false), myMutex()
{
	if (pthread_mutex_init(&myMutex, 0) < 0)
	{
		myMutex = PTHREAD_MUTEX_INITIALIZER;
		myFailedInit = true;
	}
	else
		unlock();
//	myStrMap[STATUS_FAILED_INIT]="初始化失败";
//	myStrMap[STATUS_FAILED]="未知错误";
//	myStrMap[STATUS_ALREADY_LOCKED]="锁已经被其他线程锁住";
}


RobotMutex::~RobotMutex(void)
{
	if (!myFailedInit && (pthread_mutex_destroy(&myMutex) < 0))
	{	
		if (errno == EBUSY)
		{
   	}
		else
		{
		}
	}      
}


int RobotMutex::lock(void) 
{
	if (myFailedInit)
	{
		return(STATUS_FAILED_INIT);
	}
	if (pthread_mutex_lock(&myMutex) < 0)
	{
		if (errno == EDEADLK)
		{
			return(STATUS_ALREADY_LOCKED);
		}
		else
		{
			return(STATUS_FAILED);
		}
	}
	return(0);
}

int RobotMutex::tryLock(void) 
{
	if (myFailedInit)
	{
		return(STATUS_FAILED_INIT);
	}

	if (pthread_mutex_trylock(&myMutex) < 0)
	{
		if (errno == EBUSY)
		{
		  return(STATUS_ALREADY_LOCKED);
		}
		else
		{
		  return(STATUS_FAILED);
		}
	}
	return(0);
}

int RobotMutex::unlock(void) 
{
	if (myFailedInit)
	{
		return(STATUS_FAILED_INIT);
	}

	if (pthread_mutex_unlock(&myMutex) < 0)
	{
		if (errno == EPERM)
		{
			return(STATUS_ALREADY_LOCKED);
		}
		else
		{
			return(STATUS_FAILED);
		}
	}

	return(0);
}

//const char *RobotMutex::getError(int messageNumber) const
//{
//	StrMap::const_iterator it;
//	if ((it = myStrMap.find(messageNumber)) != myStrMap.end())
//		return (*it).second.c_str();
//	else
//		return NULL;
//}
//
//
//


#ifdef ROBOT_NAMESPACE
}
#endif

