#pragma once
#ifndef ROBOT_MAIN_TASKPRIORITYQUEUE_H_
#define ROBOT_MAIN_TASKPRIORITYQUEUE_H_
/*
 *date: 2012/11/7
 *author:RenZhenwen
 *共享队列，实现成单例模式。
 * */

#include <stdlib.h>
#include <semaphore.h>

#include "RobotMutex.h"
#include "RobotTaskQueue.h"
#include "RobotPacket.h"
#include "RobotCondition.h"
//#include "robot_shared_ptr.h"
#include <memory>
#include <queue>

#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif

class RobotTaskPriorityQueue
{
public:
	//共享区的消息机制和保护机制对象
	RobotCondition cond;			
	//任务优先队列
	RobotTaskQueue taskPriorityQueue;
	//单例设计模式
	static RobotTaskPriorityQueue* getInstance(void)  
	{	
		if(_taskQueueAddr == NULL)
		{
				_taskQueueAddr = new RobotTaskPriorityQueue() ;
		}
		return _taskQueueAddr;
	}
	~RobotTaskPriorityQueue(void)
	{
		if(_taskQueueAddr != NULL)
		{
			delete _taskQueueAddr;
			_taskQueueAddr = NULL;
		}
	}
private:
	//避免类的外部构造该类的对象
	RobotTaskPriorityQueue(void):taskPriorityQueue() {}			
	//唯一的RobotTaskPriorityQueue地址
	static RobotTaskPriorityQueue *_taskQueueAddr;
};


class RobotSerialDataQueue
{
public:
	//用来保护串口数据的同步量
	RobotCondition cond;
	//串口数据缓冲区，智能指针
	queue<shared_ptr<RobotPacket>,deque<shared_ptr<RobotPacket> > > serialQueue;
	static RobotSerialDataQueue *getInstance(void)
	{	
		if(_serialDataQueueAddr == NULL)
		{	
				_serialDataQueueAddr = new RobotSerialDataQueue();
				_serialDataQueueAddr = NULL;
		}
		return _serialDataQueueAddr;	
	}
	~RobotSerialDataQueue(void)
	{
		if(_serialDataQueueAddr != NULL)
		{
			delete _serialDataQueueAddr;
			_serialDataQueueAddr = NULL;
		}
	}
private:
	RobotSerialDataQueue(void) {}
	static RobotSerialDataQueue* _serialDataQueueAddr;
};
#if 0
struct semLight
{
	sem_t	put, get;
};
class RobotSerialDataArray
{
private:
	RobotSerialDataArray(void) {}
	static RobotSerialDataArray *_robotSerialDataArray;
	static const int serialDataLen = 35;
public:
	static RobotMutex serialDataMutex;
	//数据存储区
	static unsigned char* serialDataArray;
	//定义信号灯结构体
	static struct semLight shared;	
	//初始化信号灯
	static void initSem(void)
	{
			if(-1 == sem_init(&shared.put, 0, 1))
			{
				if(-1 == sem_init(&shared.get, 0, 0))
				{
				}
			}
	}
	//获取存储区地址
	static unsigned char* getArray(void)
	{
		return serialDataArray;
	}
	//获取锁
	static RobotMutex& getMutex(void)
	{
		return serialDataMutex;
	}
	//获取该类的单例对象
	static RobotSerialDataArray* getInstance(void)
	{
		if(NULL == _robotSerialDataArray)
		{
				//在对象构造时，分配数据内存区
				serialDataArray = new unsigned char[serialDataLen];
				_robotSerialDataArray = new RobotSerialDataArray();
		}
		return _robotSerialDataArray;
	}
	
	~RobotSerialDataArray(void)
	{
		if(NULL != _robotSerialDataArray)
		{
			delete _robotSerialDataArray;
			_robotSerialDataArray = NULL;
			delete [] serialDataArray;
			serialDataArray= NULL;
		}
	}
};
#endif

class RobotNetDataQueue
{
public:
	RobotCondition cond;
	//网络数据缓冲区
	queue<shared_ptr<RobotPacket> > netDataQueue;
	static RobotNetDataQueue* getInstance(void)
	{	
		if(_netDataQueueAddr == NULL)
		{
				_netDataQueueAddr = new RobotNetDataQueue();	
		}
		return _netDataQueueAddr;
	}
	~RobotNetDataQueue(void)	
	{
		if(_netDataQueueAddr != NULL)
		{
			delete _netDataQueueAddr;
			_netDataQueueAddr = NULL;
		}
	}
private:
	RobotNetDataQueue() : cond() {}
	static RobotNetDataQueue* _netDataQueueAddr;
};
#ifdef ROBOT_NAMESPACE
}
#endif

#endif//ROBOT_MAIN_TASKPRIORITYQUEUE_H_
