#include "RobotNetEpoll.h"

/*
 *  Epoll高并发服务器机制
 *  Power by zhenwenRen
 *  date:2013-01-22 --- 2013-01-23
 *  Email: admin@unix8.net
 * */
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
int RobotNetEpoll::handleEventsAll(int* timeout)
{
  int ew_timeout = 5000;
  if(timeout != NULL) 
		ew_timeout = *timeout;
  int nfds = epoll_wait(epfd, events, DEFUALT_EVENTS, ew_timeout);
  if(nfds < 0)
  	return -1;
  if(nfds == 0) 
		return 0;
  DispactchEventsAll(nfds);
  return nfds;
}
int RobotNetEpoll::DispactchEventsAll(int nfds)
{
	for(int i = 0; i < nfds; ++i)
	{
		if(events[i].data.fd == listenFd)
		{
			struct sockaddr_in clientaddr;
			socklen_t clilen;
			bzero(&clientaddr, sizeof(clientaddr));
			clilen = sizeof(clientaddr);
			errno = 0;
			int connfd = accept(listenFd, (sockaddr *)&clientaddr, &clilen);
			if(connfd < 0)
			{
				PRINT_ERR("accept fail:%s", strerror(errno));
				continue;
			}
			char str[16] = {0};
			inet_ntop(AF_INET, &(clientaddr.sin_addr), str, 16);
			if(EeventHandleAcceptAll(connfd, str, ntohs(clientaddr.sin_port)) < 0)
			{
				EeventHandleCloseAll(connfd);
			}
		}
		else if(events[i].events & EPOLLIN)
		{
			if (events[i].data.fd < 0)
				continue;
			if(EeventHandleInAll(events[i].data.fd) < 0)
			{
				EeventHandleCloseAll(events[i].data.fd);
			}
		}
		else if(events[i].events & EPOLLOUT)
		{
			if(EeventHandleOutAll(events[i].data.fd) < 0)
			{
				EeventHandleCloseAll(events[i].data.fd);
			}
		}
		else if(events[i].events & EPOLLHUP || events[i].events & EPOLLERR)
		{
			EeventHandleErrorAll(events[i].data.fd);
		}
	}
return 0;
}
int RobotNetEpoll::EeventHandleAcceptAll(int fd,char *ip,int port)
{
  return 0;
}
int RobotNetEpoll::EeventHandleInAll(int fd)
{
  return 0;
}
int RobotNetEpoll::EeventHandleOutAll(int fd)
{
  return 0;
}
int RobotNetEpoll::EeventHandleCloseAll(int fd)
{
  close(fd);
  return 0;
}
int RobotNetEpoll::EeventHandleErrorAll(int fd)
{
  return EeventHandleClose(fd);
}
#ifdef ROBOT_NAMESPACE
}
#endif
