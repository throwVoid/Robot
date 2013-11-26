#pragma once
#ifndef ROBOT_MAIN_CONDITION_H_
#define ROBOT_MAIN_CONDITION_H_
/*
 * Powered by RenZhenwen
 * date:2012-12-07
 *
 * */

#include "RobotMutex.h"
#include <pthread.h>
//#include<map>
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
class RobotCondition
{
public:
	//条件量和锁的状态
  typedef enum {    				
    STATUS_FAILED=1, 				
    STATUS_FAILED_DESTROY, 
    STATUS_FAILED_INIT, 
    STATUS_WAIT_TIMEDOUT,
    STATUS_WAIT_INTR, 
    STATUS_MUTEX_FAILED_INIT,
    STATUS_MUTEX_FAILED 
  }Condi;//错误标识
  //typedef std::map<int, std::string> StrMap;
  typedef pthread_cond_t CondType;
  RobotCondition(void);
  ~RobotCondition(void);
  //pthread_cond_signal的包装
  int signal(void);   
  //pthread_cond_broadcast的包装	
  int broadcast(void);	
  //pthread_cond_wait的包装
  int wait(void);				
  //以下3成员函数为锁的包装
  int lock(void);				
  int unlock(void);			
  int timedWait(unsigned int msecs);
  //const char *getError(int messageNumber) const;
  //这个锁用来保护条件
  RobotMutex myMutex;  
//public:
  //static StrMap myStrMap;
private:
	//记录初始化状态
  bool myFailedInit;	
  CondType myCond;
};
#ifdef ROBOT_NAMESPACE
}
#endif
#endif//ROBOT_MAIN_CONDITION_H_
