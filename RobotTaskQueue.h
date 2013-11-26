#pragma once
#ifndef _TASKQUEUE_
#define _TASKQUEUE_
/*
 *date: 2012/11/7
 *author:Renzhenwen
 *任务队列
 */

//
#include "RobotTask.h"
//#include "robot_shared_ptr.h"
#include "RobotDeBug.h"
#include <memory>
#include <queue>
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
class RobotTaskQueue
{
public:
	RobotTaskQueue(void) {}
	~RobotTaskQueue(void) {}
	inline void pushTask(RobotTask *t);
	inline void runTask(void);
	template<class T> inline T getFirstTask(void);
	inline void clearTask(void);
	inline void runTopTask(void);
	inline int taskNum(void) const; 
	inline bool empty(void) const;
protected:
	//任务优先队列
	priority_queue<shared_ptr<RobotTask> > _taskQueue;
};

/*inline函数放在一个文件里*/
inline void RobotTaskQueue::pushTask(RobotTask *t)
{
	shared_ptr<RobotTask> p(t);     //RuntimeError? tmp 
	_taskQueue.push(p);
}
inline void RobotTaskQueue::runTask(void)
{
	while(!_taskQueue.empty())
	{
		(_taskQueue.top())->doTask();
		_taskQueue.pop();
	}

}
inline void RobotTaskQueue::runTopTask(void)
{
	(_taskQueue.top())->doTask();
	_taskQueue.pop();
	clearTask();
	/*
	 * 运行优先级最高的任务，其他任务则丢弃
	 *
	 * */
}
template<class T> inline T RobotTaskQueue::getFirstTask(void)
{
	T t;
	t = _taskQueue.top();
	_taskQueue.pop();
	return t;
}
inline void RobotTaskQueue::clearTask(void)
{
	while(!_taskQueue.empty())
	{
		_taskQueue.pop();
	}

}
inline int RobotTaskQueue::taskNum(void) const
{
	return _taskQueue.size();
}
inline bool RobotTaskQueue::empty() const
{
	return _taskQueue.empty();
}
#ifdef ROBOT_NAMESPACE
}
#endif
#endif
