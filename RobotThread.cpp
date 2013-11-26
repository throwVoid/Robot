#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include "RobotThread.h"
/*
 * power by RenZhenwen
 * date:2012-12-07
 * */

RobotMutex RobotThread::ourThreadsMutex;  //静态变量来管理所有线程
RobotThread::MapType RobotThread::ourThreads;


void RobotThread::stopAll(void)  //停止所有线程，静态
{
  MapType::iterator iter;
  ourThreadsMutex.lock();
  for (iter=ourThreads.begin(); iter != ourThreads.end(); ++iter)
    (*iter).second->stopRunning();
  ourThreadsMutex.unlock();
}

void RobotThread::joinAll(void)//等待所有线程，静态
{
  MapType::iterator iter;
  RobotThread *thread;
  thread=self();
  ourThreadsMutex.lock();
  for (iter=ourThreads.begin(); iter != ourThreads.end(); ++iter)
  {
    if ((*iter).second->getJoinable() && thread && (thread != (*iter).second))
    {
      (*iter).second->doJoin();
    }
  }
  ourThreads.clear();					//完成后清除map
  //ourThreads.insert(MapType::value_type(thread->myThread, thread));
  ourThreadsMutex.unlock();
}

RobotThread::RobotThread(void) :
						myRunning(false),myJoinable(false), myFunc(0),myThread() //传递是否阻塞所有信号
{
	//需要手动调用create
}

RobotThread::RobotThread(ThreadType thread, bool joinable) :
												myRunning(false), myJoinable(joinable),myFunc(0),myThread(thread) //传递线程ID，join，阻塞标识
{
	//需要手动调用create
}

RobotThread::RobotThread(RobotFunctor *func, bool joinable) :
												myRunning(false),myJoinable(false),myFunc(func),myThread()//传递函数封装，join，阻塞标识
{
  create(func, joinable);
}

RobotThread::~RobotThread(void)
{
	PRINT("机器人线程类对象析构");
}

int RobotThread::join(void **iret)
{
  int ret;
  ret=doJoin(iret);
  if (ret)
    return(ret);
  ourThreadsMutex.lock();
  ourThreads.erase(myThread);
  ourThreadsMutex.unlock();
  return(0);
}



static void * run(void *arg)//极其重要的函数，线程创建需要调用的函数，这个函数实际上调用的是Functor封装对象里的函数，目的就是去static
{
  RobotThread *t=(RobotThread*)arg;
  void *ret=NULL;
  if (dynamic_cast<ArRetFunctor<void*>*>(t->getFunc()))
    ret=((ArRetFunctor<void*>*)t->getFunc())->invokeR();
  else
    t->getFunc()->invoke();
  return(ret);
}



void RobotThread::initThreads(void)//静态函数，由主进程调用，初始化线程map
{
  RobotThread *main;
  ThreadType pt;

  pt=pthread_self();

  ourThreadsMutex.lock();
  if (ourThreads.size())
  {
    ourThreadsMutex.unlock();
    return;
  }
  main=new RobotThread;
  main->myJoinable=true;
  main->myRunning=true;
  main->myThread=pt;
  ourThreads.insert(MapType::value_type(pt, main));
  ourThreadsMutex.unlock();
}

RobotThread * RobotThread::self(void)			//返回当前线程的封装
{
  ThreadType pt;
  MapType::iterator iter;

  ourThreadsMutex.lock();
  pt=pthread_self();
  iter=ourThreads.find(pt);
  ourThreadsMutex.unlock();

  if (iter != ourThreads.end())
    return((*iter).second);
  else
    return(NULL);
}

void RobotThread::cancelAll(void)
{
  MapType::iterator iter;

  ourThreadsMutex.lock();
  for (iter=ourThreads.begin(); iter != ourThreads.end(); ++iter)
  {
    pthread_cancel((*iter).first);
    (*iter).second->stopRunning();
  }
  ourThreads.clear();
  ourThreadsMutex.unlock();
}

int RobotThread::create(RobotFunctor *func, bool joinable, bool lowerPriority)
{
  int ret;
  pthread_attr_t attr;

  pthread_attr_init(&attr);
  if (joinable)
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  else
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  myJoinable=joinable;
  myFunc=func;
  myRunning=true;
  if ((ret=pthread_create(&myThread, &attr, &run, this)) != 0)
  {
    pthread_attr_destroy(&attr);
    if (ret == EAGAIN)
    {
      return(STATUS_NORESOURCE);
    }
    else
    {
      return(STATUS_FAILED);
    }
  }
  else
  {
    ourThreadsMutex.lock();
    ourThreads.insert(MapType::value_type(myThread, this));
    ourThreadsMutex.unlock();
    pthread_attr_destroy(&attr);
    return(0);
  }
}

int RobotThread::doJoin(void **iret)
{
  int ret;
  if ((ret=pthread_join(myThread, iret)) != 0)
  {
    if (ret == ESRCH)
    {
      return(STATUS_NO_SUCH_THREAD);
    }
    else if (ret == EINVAL)
    {
      return(STATUS_INVALID);
    }
    else if (ret == EDEADLK)
    {
      return(STATUS_JOIN_SELF);
    }
  }
  return(0);
}

int RobotThread::detach(void)
{
  int ret;
  if ((ret=pthread_detach(myThread)) != 0)
  {
    if (ret == ESRCH)
    {
      return(STATUS_NO_SUCH_THREAD);
    }
    else if (ret == EINVAL)
    {
      return(STATUS_ALREADY_DETATCHED);
    }
  }
  myJoinable=false;
  return(0);
}

void RobotThread::cancel(void)
{
  ourThreadsMutex.lock();
  ourThreads.erase(myThread);
  ourThreadsMutex.unlock();
  pthread_cancel(myThread);
}

void RobotThread::yieldProcessor(void)
{
  sched_yield();//函数可以使另一个级别等于或高于当前线程的线程先运行
}
