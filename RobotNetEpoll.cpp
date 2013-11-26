#include "RobotNetEpoll.h"

#include <unistd.h>
#include <errno.h>

/*
 *	Epoll高并发服务器机制
 *	Power by zhenwenRen
 *	date:2013-01-22 --- 2013-01-23
 *	Email: admin@unix8.net
 * */
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
RobotNetEpoll::RobotNetEpoll(void) : listenFd(-1), socketNum(0)
{
	//构造函数，初始化监听套接字和管理的套接字数目
	events = NULL;
	epfd = -1;
}

RobotNetEpoll::~RobotNetEpoll(void) 
{
  Close();
}

//添加一个套接字到容器，不包含监听套接字
void RobotNetEpoll::addSocketToList(int sock) 
{
	socketList.push_back(sock);
	socketNumInc();
}

//Epoll描述符生成
int RobotNetEpoll::openEpoll(int Max_Events)  
{
	maxevents = Max_Events;
  struct rlimit rt;
  rt.rlim_max = rt.rlim_cur = maxevents;
  errno = 0;
  if (setrlimit(RLIMIT_NOFILE, &rt) == -1) 
	{
		 PRINT_ERR("setrlimit 出错:  %d %s\n", errno, strerror(errno));
		 return -1;
  }
	//为类的成员分配空间
  events = new struct epoll_event[maxevents];
	//创建Epoll描述符epfd
  if ((epfd = epoll_create(maxevents)) < 0)
  {
		delete [] events;
		epfd = -1;
    return -1;
  }
 return 0;
}
int RobotNetEpoll::Close()
{
	// 关闭监听套接字
	if(listenFd != -1) 
		close(listenFd); 			
	for(list<int>::iterator iter = socketList.begin(); iter != socketList.end(); ++iter)
	{
		//关闭套接字
		close(*iter);
	}
	//清除
	socketList.clear(/*socketVector.begin(), socketVector.end()*/);  
	//关闭Epoll描述符
	if(epfd != -1)
		close(epfd);
	//释放资源
	if(events != NULL) 
		delete [] events;				
	return 0;
}
//注册描述符事件
int RobotNetEpoll::registerHandler(int fd)
{
	struct epoll_event ev;
	ev.data.fd = fd;
	//设置要处理的事件类型
	ev.events = EPOLLRDHUP | EPOLLIN | EPOLLET /*| EPOLLERR | EPOLLHUP*/;
	//注册epoll事件
	int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
	if(ret == -1)
	{
		PRINT_ERR("epoll_ctl:  %d %s\n", errno, strerror(errno));
		//一次注册失败，还在考虑是否关闭所有描述符
		//	Close();												
    return -1;
  }
  return 0;
}
int RobotNetEpoll::registerHandler(int fd,int fd_mask)
{
	struct epoll_event ev;
  ev.data.fd = fd;
	//设置要处理的事件类型
	switch(fd_mask)
 	{
 	case ACCEPT_MASK:
				 listenFd = fd;
				 ev.events = /*EPOLLRDHUP|*/EPOLLIN|EPOLLET/*|EPOLLERR|EPOLLHUP*/;
				 break;
 	case READ_MASK:
				 ev.events = EPOLLRDHUP|EPOLLIN|EPOLLET/*|EPOLLERR|EPOLLHUP*/;
				 break;
 	case WRITE_MASK:
				 ev.events = EPOLLOUT|EPOLLET/*|EPOLLERR|EPOLLHUP*/;
				 break;
 	default:
				 return -1;
	 }
	//注册epoll事件
	int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
	if(ret == -1)
	{
		PRINT_ERR("epoll_ctl:  %d %s\n",errno,strerror(errno));
		return -1;
	}
	return 0;
}
int RobotNetEpoll::removeHandler(int fd,int fd_mask)
{
	struct epoll_event ev;
	ev.data.fd = fd;
	//设置要处理的事件类型
	ev.events = EPOLLRDHUP|EPOLLIN|EPOLLOUT|EPOLLET|EPOLLERR|EPOLLHUP;
	//取消epoll事件
	int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
	if(ret == -1)
	{
		PRINT_ERR("EPOLL_CTL_DEL:  %d %s\n", errno, strerror(errno));
		return -1;
	}
	return 0;
}
int RobotNetEpoll::removeHandler(int fd)
{
  struct epoll_event ev;
  ev.data.fd = fd;
  //设置要处理的事件类型
  ev.events = EPOLLRDHUP|EPOLLIN|EPOLLOUT|EPOLLET|EPOLLERR|EPOLLHUP;
  //注册epoll事件
  int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
  if(ret == -1)
	{
 		return -1;
  }
  return 0;
}
int RobotNetEpoll::updateHandler(int fd,int fd_mask)
{
	removeHandler(fd);
	return	registerHandler(fd, fd_mask);
}
#ifdef ROBOT_NAMESPACE
}
#endif
