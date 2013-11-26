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
//返回准备好的事件个数
int RobotNetEpoll::handleEvents() 
{
#if 0
  int nfds = epoll_wait(epfd, events, DEFUALT_EVENTS, -1);
  if(nfds < 0)
  	return -1;
  else if(nfds == 0) 
		return 0;
	else
		return nfds;
#else
  return  epoll_wait(epfd, events, DEFUALT_EVENTS, -1);
#endif
}
/*把第I个事件数据存放到buf和len这两个值-结果参数中*/
RobotNetEpoll::EpollDataStatus RobotNetEpoll::DispactchEvent(int i, unsigned char* buf, int *len, int& fd)
{
	//处理网络挂端
	if(unlikely(events[i].events & EPOLLRDHUP))
	{
		*len = 0;
		fd = events[i].data.fd;
		EeventHandleClose(fd);
		return EpollSocketClosed; 
	}
	else if(likely(events[i].events & EPOLLIN))
	{
		if(unlikely(events[i].data.fd == listenFd))
		{
			struct sockaddr_in clientaddr;
			socklen_t clilen;
			bzero(&clientaddr, sizeof(clientaddr));
			clilen = sizeof(clientaddr);
			errno = 0;
			//accept客户端
			int connfd = accept(listenFd, (sockaddr *)&clientaddr, &clilen);
			if(connfd < 0)
			{
				PRINT_ERR("accept fail:%s", strerror(errno));
				//accept出错，返回EpollError
				return EpollError;
			}
			//将fd作为值-结果返回
			fd = connfd;
			//char str[16] = {0};
			//根据IP地址长度设置返回长度
			*len =16; 
			//以字符串返回的IP地址，增加结尾符
			buf[16] = '\0';
			inet_ntop(AF_INET, &(clientaddr.sin_addr),(char*)buf/*str*/, 16);
			if((epollStatus = EeventHandleAccept(connfd,NULL/*(char*)buf*//*str*/, ntohs(clientaddr.sin_port))) == EpollError)
			{
				EeventHandleClose(connfd);
			}
			return epollStatus;
		/*返回两种情况：EpollAccept, EpollError*/
		}
		else
		{
			fd = events[i].data.fd;
			if((epollStatus =(EeventHandleIn(events[i].data.fd, buf, len))) == EpollError )
			{
				EeventHandleClose(events[i].data.fd);
			}
			return epollStatus;
		}
	}
	else if(unlikely(events[i].events & EPOLLOUT))
	{
		fd = events[i].data.fd;
		if((epollStatus = (EeventHandleOut(events[i].data.fd, buf, len))) == EpollError)
		{
			EeventHandleClose(events[i].data.fd);
		}
		return epollStatus;
	}
	else if(unlikely(events[i].events & EPOLLHUP || events[i].events & EPOLLERR))
	{
		fd = events[i].data.fd;
		EeventHandleError(events[i].data.fd);
		return EpollError;
	}
	return EpollError;
}
RobotNetEpoll::EpollDataStatus RobotNetEpoll::EeventHandleAccept(int fd,char *ip,int port)
{
	//设置成非阻塞
	fcntl (fd, F_SETFL, O_NONBLOCK);
	//已经连接的套接字则等待消息的到来
	if(registerHandler(fd, READ_MASK) != 0) 
		return EpollError;
	else
	{
		//将连接的客户端套接字加入到容器中
		addSocketToList(fd);
		//表示accept返回
  	return EpollAccept;  	
	}
}
RobotNetEpoll::EpollDataStatus RobotNetEpoll::EeventHandleIn(int fd, unsigned char* buf, int *len)
{
	int n =	read(fd, buf, *len);
	*len = n;
	if(n == 0)
	{
		EeventHandleClose(fd);
		//意味中该套接字已经关闭
		return EpollSocketClosed; 
	}
	else if(n < 0)
	{
		return EpollError;
	}
	//这个状态返回表明还需进一步判断
	return EpollSuccessWithData;
}
RobotNetEpoll::EpollDataStatus RobotNetEpoll::EeventHandleOut(int fd, const unsigned  char *buf, int* len)
{
	if(write(fd, buf, *len) < 0)
		return EpollError;
	else
  	return EpollSuccessNoData;  //写入成功返回的是空
}

RobotNetEpoll::EpollDataStatus RobotNetEpoll::EeventHandleClose(int fd)
{
	//从Epoll中去除该连接
	removeHandler(fd);
  close(fd);
	socketNumDec();						//减少数量
	socketList.remove(fd);		//从链表中删除
  return EpollSuccessNoData;
}
RobotNetEpoll::EpollDataStatus RobotNetEpoll::EeventHandleError(int fd)
{
  return EeventHandleClose(fd);
}
#ifdef ROBOT_NAMESPACE
}
#endif
