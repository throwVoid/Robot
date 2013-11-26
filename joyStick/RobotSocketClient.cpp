
#include "RobotSocket.h"

/*
 * power by RenZhenwen
 * date: start 2012-12-25 圣诞节
 * date: finish 2012-12-29
 * email: admin@unix8.net 
 * */
/*
 *
 * 	作为客户端套接字时调用的构造函数
 * @host 为客户端准备连接的服务器IP
 * @port 连接的端口
 * @type 套接字类型
 *
 * */
RobotSocket::RobotSocket(const char *host, int port, Type type) : 
  myType(type),
  myError(NoErr),
  myErrorStr(),
  myDoClose(true),
  myFD(-1),
  myNonBlocking(false),
  mySin()
{
	internalInit();			//初始化一些数组
	connect(host, port, type);   //主动连接
}



/*
 * 以type套接字类型来连接目的服务器主机
 * host为NULL，则连接本机的服务器
 * 完成套接字的创建，服务器的链接
 * */
bool RobotSocket::connect(const char *host, int port, Type type)
{
	char localhost[maxHostNameLen()];

	if (!host) //host 为NULL，则为本机localhost 
	{
		if (gethostname(localhost, sizeof(localhost)) == 1)
		{
			myError=ConBadHost;
			myErrorStr="[gethostbyname]由主机名获取主机地址失败";
			myErrorStr+=localhost;
			myErrorStr+="'";
			perror(myErrorStr.c_str());
			return(false);
		}
		host=localhost; //已经获取到本地计算机名
	}

	bzero(&mySin, sizeof(mySin));
	if (!hostAddr(host, mySin.sin_addr))//把获取的主机名转换成地址结构体
		return(false);
	setIPString();		//设置IP字符串
	mySin.sin_family=AF_INET;
	mySin.sin_port=hostToNetOrder(port);

	if ((type == TCP) && ((myFD=socket(AF_INET, SOCK_STREAM, 0)) < 0))
	{
		myError=NetFail;
		myErrorStr="[socket]创建TCP套接字失败";
		perror(myErrorStr.c_str());
		return(false);
	}
	else if ((type == UDP) && ((myFD=socket(AF_INET, SOCK_DGRAM, 0)) < 0))
	{
		myError=NetFail;
		myErrorStr="[socket]创建UDP套接字失败";
		perror(myErrorStr.c_str());
		return(false);
	}

	myType=type;

	if (::connect(myFD, (struct sockaddr *)&mySin,
		sizeof(struct sockaddr_in)) < 0)
	{
		myErrorStr="连接套接字失败";
		switch (errno)
		{
		case ECONNREFUSED:
			myError=ConRefused;
			myErrorStr+="; 连接被拒";
			break;
		case ENETUNREACH:
			myError=ConNoRoute;
			myErrorStr+="; 没有到达目的地的路由";
			break;
		default:
			myError=NetFail;
			break;
		}
		
		::close(myFD);
		myFD = -1;
		perror(myErrorStr.c_str());
		return(false);
	}
	return(true);
}



/*
 * 作为客户端去连接一个服务器
 * 套接字已经创建
 * */
bool RobotSocket::connectTo(const char *host, int port)
{
	char localhost[maxHostNameLen()];

	if (myFD < 0)
		return(false);

	if (!host)
	{
		if (gethostname(localhost, sizeof(localhost)) == 1)
		{
		  myErrorStr="远程主机连接失败";
		  myErrorStr+=localhost;
		  myErrorStr+="'";
		  perror(myErrorStr.c_str());
		  return(false);
		}
		host=localhost;
	}

	bzero(&mySin, sizeof(mySin));
	if (!hostAddr(host, mySin.sin_addr))
	return(false);
	setIPString();
	mySin.sin_family=AF_INET;
	mySin.sin_port=hostToNetOrder(port);

	return(connectTo(&mySin));
}

/*主动去连接服务器*/
bool RobotSocket::connectTo(struct sockaddr_in *sin)
{
	if (::connect(myFD, (struct sockaddr *)sin,
		sizeof(struct sockaddr_in)) < 0)
	{
		myErrorStr="[connect]connect失败";
		perror(myErrorStr.c_str());
		return(0);
	}

	return(1);
}
