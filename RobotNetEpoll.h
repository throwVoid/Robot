#pragma once
#ifndef ROBOT_MAIN_NETEPOLL_H_
#define ROBOT_MAIN_NETEPOLL_H_

/*
 *  Epoll高并发服务器机制
 *  Power by zhenwenRen
 *  date:2013-01-22 --- 2013-01-23
 *  Email: admin@unix8.net
 * */

#include <sys/epoll.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <list>
#include <map>
#include <string>
#include "RobotDeBug.h"
#include "RobotSocket.h"


#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif

//#define DEFUALT_EVENTS 1024
#define DEFUALT_EVENTS 30 

class RobotNetEpoll 
{
public:
	#define likely(x) __builtin_expect(!!(x),1)
	#define unlikely(x) __builtin_expect(!!(x),0)
 	enum
	{
		ACCEPT_MASK = 1,											//新连接
		READ_MASK = 2,												//可读
		WRITE_MASK = 3,												//可写
		CLR_MASK = 4													//清除
	};

	//用于事件发生后返回状态
	typedef enum EpollDataStatus 
	{
		EpollAccept = 0x01,//有新的tcp连接建立
		EpollSocketClosed = 0x02,//新的tcp连接断开
		EpollSuccessWithData = 0x04,//收到数据
		EpollSuccessNoData = 0x08,//作为返回状态后继续判断
		EpollError = 0x10
	}EpollDataStatus;

	//构造函数
	RobotNetEpoll(void);
	//析构函数
  ~RobotNetEpoll(void);
	//创建Epoll
  int openEpoll(int Max_Events = DEFUALT_EVENTS);	
	//注册
  int registerHandler(int fd);
	//根据mask注册
  int registerHandler(int fd, int fd_mask);
	//移除
  int removeHandler(int fd, int fd_mask);			
  int removeHandler(int fd);
	//更新
  int updateHandler(int fd, int fd_mask);			

	/*以下两个函数目的：将每个事件的处理权交给其他模块*/
	//返回当前可用的事件数目
	int handleEvents();     
	//提取数据，交给其他模块处理
	EpollDataStatus DispactchEvent(int i, unsigned char* buf, int *len, int&); 

	/*以下两个函数目的：将每个事件的处理权由类的内部处理*/
	//返回当前可用的事件数目
	int handleEventsAll(int*);     
	//epoll模块内部处理事件
	int DispactchEventsAll(int nfds); 



private:
	/*以下5个函数目的：将每个事件的处理权交给其他模块*/
	//处理服务器ACCEPT事件
  EpollDataStatus EeventHandleAccept(int fd, char *ip, int port);    
	//处理可读事件
  EpollDataStatus EeventHandleIn(int fd, unsigned char* buf, int* len);					
	//处理输出事件
  EpollDataStatus EeventHandleOut(int fd, const unsigned char* buf, int* len);				
	//处理关闭事件
  EpollDataStatus EeventHandleClose(int fd);			
	//处理出错事件
  EpollDataStatus EeventHandleError(int fd);			

	/*以下5个函数目的：将每个事件的处理权由类的内部处理*/
	//处理服务器ACCEPT事件
  int EeventHandleAcceptAll(int fd, char *ip, int port);    
	//处理可读事件
  int EeventHandleInAll(int fd);				
	//处理输出事件	
  int EeventHandleOutAll(int fd);				
	//处理关闭事件
  int EeventHandleCloseAll(int fd);			
	//处理出错事件
  int EeventHandleErrorAll(int fd);			

protected:
	//关闭EPOLL和释放资源
	int Close();										
	//增加一个描述符到容器
	void addSocketToList(int fd);  
	//增加套接字计数
	int socketNumInc() { socketNum ++; return socketNum;} 
	//减少套接字计数
	int socketNumDec() { socketNum --; return socketNum;} 
private:
	//返回的Epoll状态
	EpollDataStatus epollStatus;
	int epfd;
	int listenFd;
	struct epoll_event *events;
	int maxevents;
	int socketNum;
	list<int> socketList;
	//map<int,string> socketMap;
};
#ifdef ROBOT_NAMESPACE
}
#endif
#endif//ROBOT_MAIN_NETEPOLL_H_
