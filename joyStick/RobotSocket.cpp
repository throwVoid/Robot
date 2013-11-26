#include "RobotSocket.h"

/*
 * power by RenZhenwen
 * date: start 2012-12-25 圣诞节
 * date: finish 2012-12-29
 * email: admin@unix8.net 
 * */
bool RobotSocket::ourInitialized = true;
bool RobotSocket::init()
{
	return(true);
}
void RobotSocket::shutdown()
{
}


/* ----global----
 * mySin
 * 针对客户端：存放服务器的套接字结构
 * 针对服务器：为监听套接字，存储服务器的套接字结构；为连接套接字，则存储客户端套接字结构信息
 * */






void RobotSocket::internalInit(void)
{
	myCloseFunctor = NULL;
	sprintf(myIPString, "none");
	resetTracking();
}
/*
 *@ 为进行套接字和相关控制的构造函数
 * */
RobotSocket::RobotSocket() : 
  myType(Unknown),   	//套接字类型
  myError(NoErr),	//错误原因
  myErrorStr(),		//错误描述
  myDoClose(true),	
  myFD(-1),		//套接字
  myNonBlocking(false), //不阻塞
  mySin()		//套接字结构
{
	internalInit();
}




/*
 * 指定了myDoClose则需要调用全局close来关闭连接套接字
 * */
bool RobotSocket::close()
{
	if (myCloseFunctor != NULL)
		myCloseFunctor->invoke();
	if (myDoClose && ::close(myFD))
	{
		myFD = -1;
		return(false);
	}
	else
	{
		myFD = -1;
		return(true);
	}
}

RobotSocket::~RobotSocket()
{
	close();
}


/*
 *  创建指定type的套接字
 * */
bool RobotSocket::create(Type type)
{
	if ((type == TCP) && ((myFD = socket(AF_INET, SOCK_STREAM, 0)) < 0))
	{
		myErrorStr = "[socket]创建TCP套接字失败";
		perror(myErrorStr.c_str());
		return(false);
	}
	else if ((type == UDP) && ((myFD = socket(AF_INET, SOCK_DGRAM, 0)) < 0))
	{
		myErrorStr = "[socket]创建UDP套接字失败";
		perror(myErrorStr.c_str());
		return(false);
	}
	myType = type;
	/*
	*
	* 这里还有点问题 2012-12-29
	*
	*
	* */
	if (getSockName()) //获取与该套接字关联的套接字结构体
		return(true);
	else
		return(false);
}

int RobotSocket::sendTo(const void *msg, int len)
{
	int ret;
	ret = ::sendto(myFD, (char*)msg, len, 0, (struct sockaddr*)&mySin, sizeof(mySin));
	if (ret > 0)
	{
		mySends ++;
		myBytesSent += ret;
	}
	return ret;
}

int RobotSocket::sendTo(const void *msg, int len, struct sockaddr_in *sin)
{ 
	int ret;
	ret = ::sendto(myFD, (char*)msg, len, 0, (struct sockaddr*)sin, sizeof(struct sockaddr_in));
	if (ret > 0)
	{
		mySends ++;
		myBytesSent += ret;
	}
	return ret;
}


/*
 *
 *  客服端指定服务器的sin来获取数据
 *  服务器连接套接字指定客户端的sin来获取数据
 * */
int RobotSocket::recvFrom(void *msg, int len, sockaddr_in *sin)
{

	socklen_t i = sizeof(sockaddr_in);
	int ret;
	ret = ::recvfrom(myFD, (char*)msg, len, 0, (struct sockaddr*)sin, &i);
	if (ret > 0)
	{
		myRecvs ++;
		myBytesRecvd += ret;
	}
	return ret;
}


/*
 */
int RobotSocket::write(const void *buff, size_t len)
{

	if (myFD < 0)
	{
		return 0;
	}
	struct timeval tval;
	fd_set fdSet;
	tval.tv_sec = 0;
	tval.tv_usec = 0;
	FD_ZERO(&fdSet);
	FD_SET(myFD, &fdSet);

	if (select(myFD + 1, NULL, &fdSet, NULL, &tval) <= 0)  //等待套接字可读
		return 0;

	int ret;
	ret = ::write(myFD, (char*)buff, len);  //TCP套接字写入

	if (ret > 0)
	{
		mySends ++;
		myBytesSent += ret;
	}
	return ret;
}


int RobotSocket::read(void *buff, size_t len, unsigned int msWait)
{
	if (myFD < 0)
	{
		return 0;
	}

	int ret;
	if (msWait != 0)
	{
		struct timeval tval;
		fd_set fdSet;
		tval.tv_sec = msWait / 1000;
		tval.tv_usec = (msWait % 1000) * 1000;
		FD_ZERO(&fdSet);
		FD_SET(myFD, &fdSet);
		if (select(myFD + 1, &fdSet, NULL, NULL, &tval) <= 0) //时间大于0，阻塞等待一会
			return 0;
	}
	ret = ::recv(myFD, (char*)buff, len, 0);           //为0.
	if (ret > 0)
	{
		myRecvs ++;
		myBytesRecvd += ret;
	}
	return ret;
}

