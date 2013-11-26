#pragma once
#ifndef RobotThread_H
#define RobotThread_H


#include <map>
#include <pthread.h>
#include "RobotMutex.h"
#include "RobotFunctor.h"
/*
 * power by RenZhenwen
 * date:2012-12-07
 * */
class RobotThread
{
public:
	typedef pthread_t ThreadType;
	typedef std::map<ThreadType, RobotThread*> MapType;
	typedef enum {
		STATUS_FAILED=1,
		STATUS_NORESOURCE,
		STATUS_NO_SUCH_THREAD,
		STATUS_INVALID,
		STATUS_JOIN_SELF,
		STATUS_ALREADY_DETATCHED
	} Status;

	RobotThread(void);
	//用已经存在的线程ID来初始化对象
	RobotThread(ThreadType thread, bool joinable);		
	//传递线程处理函数。joinable为设置是否线程分离
	RobotThread(RobotFunctor *func, bool joinable = true);

	//virtual ~RobotThread(void);
	//暂时没打算让这个类被继承
	~RobotThread(void);


	static void initThreads(void);
	//返回自己的Thread对象
	static RobotThread* self(void);
	//停止所有线程
	static void stopAll(void);		
	//取消所有线程
	static void cancelAll(void);
	//等待所有线程	
	static void joinAll(void);  
	static void yieldProcessor(void);
	//创建和启动线程
	int create(RobotFunctor *func, bool joinable = true, bool lowerPriority = true);
	//停止线程
	void stopRunning(void) { myRunning = false; }
	//等待线程
	int join(void **ret = NULL);  
	//分离线程
	int detach(void);		
	//取消线程
	void cancel(void);		
	//返回线程运行状态
	bool getRunning(void) const { return(myRunning); }
	//锁住后安全获取线程运行状态
	bool getRunningWithLock(void) { bool running; lock(); running = myRunning; unlock(); return running; }
	//获取线程join状态
	bool getJoinable(void) const { return(myJoinable); }  
	//获取线程ID指针，ID不能直接复制
	const ThreadType* getThread(void) const {return(&myThread);}
	//返回线程函数********重要，多态实现
	RobotFunctor * getFunc(void) const { return(myFunc); }  
	//设置线程运行状态
	void setRunning(bool running) { myRunning = running; }
	//锁住线程运行实例
	int lock(void) { return(myMutex.lock()); }
	int tryLock(void) { return(myMutex.tryLock()); }
	int unlock(void) { return(myMutex.unlock()); }
	//返回线程名字
	const char *getThreadName(void) { return myName.c_str(); }
	//设置线程名字
	void setThreadName(const char *name) { myName = name; }
protected:
	static RobotMutex ourThreadsMutex;			//用于访问所有线程map时候的锁，不需要条件变量同步
	static MapType ourThreads;				//线程map，存放所有线程
	int doJoin(void **ret=NULL);					//等待线程，回收资源
	std::string myName;							//线程名字
	RobotMutex myMutex;								//线程锁
 
	bool myRunning;									//线程状态，运行or停止
	bool myJoinable;									//join状态 detach还是joinable
	RobotFunctor *myFunc;								//线程函数封装		
	ThreadType myThread;								//线程ID
};


#endif
