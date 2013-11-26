#include "RobotSocket.h"
/*
 * power by RenZhenwen
 * date: start 2012-12-25 圣诞节
 * date: finish 2012-12-29
 * email: admin@unix8.net 
 * */
/*
 * 这个函数是针对TCP的，设置套接口的SO_LINGER选项
 *
 * */
bool RobotSocket::setLinger(int time)
{
	struct linger lin;
	/*
	struct linger
	{
	int l_onoff;
	int l_linger;
	}
	*/
	if (time)
	{
		lin.l_onoff=1;
		lin.l_linger=time; //如果在套接口缓冲区还有数据，那么进程就休眠直到所有数据发完并且ACK已经收到。
	}
	else
	{
		lin.l_onoff=0;
		lin.l_linger=time;
	}

	if (setsockopt(myFD, SOL_SOCKET, SO_LINGER, &lin, sizeof(lin)) != 0)
	{
		myErrorStr="设置套接字选项LINGER失败";
		perror(myErrorStr.c_str());
		return(false);
	}
	else
		return(true);
}

/*
 * 禁止UDP的广播
 * */
bool RobotSocket::setBroadcast()
{
	if (setsockopt(myFD, SOL_SOCKET, SO_BROADCAST, NULL, 0) != 0)
	{
		myErrorStr = "设置广播套接字选项失败";
		perror(myErrorStr.c_str());
		return(false);
	}
	else
		return(true);
}
/*
 * 允许地址重用
 * */
bool RobotSocket::setReuseAddress()
{
	int opt=1;

	if (setsockopt(myFD, SOL_SOCKET, SO_REUSEADDR,
		 (char*)&opt, sizeof(opt)) != 0)
	{
		myErrorStr="地址重用选项设置失败";
		perror(myErrorStr.c_str());
		return(false);
	}
	else
		return(true);
}

/*
 *  设置套接口的非阻塞
 * */
bool RobotSocket::setNonBlock()
{
	if (fcntl(myFD, F_SETFL, O_NONBLOCK) != 0)
	{
		myErrorStr="设置非阻塞失败";
		perror(myErrorStr.c_str());
		return(false);
	}
	else
	{
		myNonBlocking = true;
		return(true);
	}
}

/*
 * 如果这是一个TCP连接，并且设置TCP_nodelay选项，则禁止使用nagle算法
 * */
bool RobotSocket::setNoDelay(bool flag)
{
	if(myType != TCP) return false;
	int f = flag?1:0;
	int r = setsockopt(myFD, IPPROTO_TCP, TCP_NODELAY, (char*)&f, sizeof(f));
	return (r != -1);
}

