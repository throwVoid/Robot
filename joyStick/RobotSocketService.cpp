#include "RobotSocket.h"


/*
 * power by RenZhenwen
 * date: start 2012-12-25 圣诞节
 * date: finish 2012-12-29
 * email: admin@unix8.net 
 * */

/**
 * 			做为服务器端时使用的构造函数
 *@doClose 当套接字析构时，是否自动关闭该连接套接字
*/
RobotSocket::RobotSocket(int port, bool doClose, Type type) :   
  myType(type),
  myError(NoErr),
  myErrorStr(),
  myDoClose(doClose),
  myFD(-1),
  myNonBlocking(false),
  mySin()
{
	internalInit();
	open(port, type);	//主动打开，被动连接
}
/**
 * 			做为服务器端时使用的构造函数
 *@doClose 当套接字析构时，是否自动关闭该连接套接字
*/
RobotSocket::RobotSocket(const char* host, int port, bool doClose, Type type) :   
  myType(type),
  myError(NoErr),
  myErrorStr(),
  myDoClose(doClose),
  myFD(-1),
  myNonBlocking(false),
  mySin()
{
	internalInit();
	open(port, type, host);	//主动打开，被动连接

}
/*
 * 作为服务器套接字
 * 完成套接字的穿件，绑定
 * 
 * */
bool RobotSocket::open(int port, Type type, const char *openOnIP)
{
	int ret;
	char localhost[maxHostNameLen()];

	if ((type == TCP) && ((myFD=socket(AF_INET, SOCK_STREAM, 0)) < 0))
	{
		myErrorStr="[socket]创建TCP套接字失败";
		perror(myErrorStr.c_str());
		return(false);
	}
	else if ((type == UDP) && ((myFD=socket(AF_INET, SOCK_DGRAM, 0)) < 0))
	{
		myErrorStr="[socket]创建UDP套接字失败";
		perror(myErrorStr.c_str());
		return(false);
	}

	myType=type;
	bzero(&mySin, sizeof(mySin));

	if (openOnIP != NULL)
	{

		if (!hostAddr(openOnIP, mySin.sin_addr))
		{
			/*不能查找到此IP主机*/
			return(false); 
		}
		else
		{
    			/*成功*/
		}
	}
	else
	{
		mySin.sin_addr.s_addr = htonl(INADDR_ANY);  //任意分配本机有的IP，任务交给内核
	}

	setIPString();
	mySin.sin_family = AF_INET;
	mySin.sin_port = hostToNetOrder(port);

	if ((ret=bind(myFD, (struct sockaddr *)&mySin, sizeof(mySin))) < 0)
	{
		myErrorStr="[bind]绑定失败";
		sprintf(localhost, "%d", port);
		myErrorStr+=localhost;
		perror(myErrorStr.c_str());
		return(false);
	}

	if ((type == TCP) && (listen(myFD, 5) < 0))
	{
		myErrorStr="[listen]绑定失败";
		perror(myErrorStr.c_str());
		return(false);
	}

	return(true);
}

/*
 * 注意点：如果为非阻塞模式，没有套接字来连接并不代表一个错误
 * 使用getFd检查是否是个有效的FD
 * */
bool RobotSocket::accept(RobotSocket *sock)
{
  socklen_t len;
  unsigned char *bytes;
  
  len=sizeof(struct sockaddr_in);
  sock->myFD=::accept(myFD, (struct sockaddr*)&(sock->mySin), &len);
  sock->myType=myType;
  bytes = (unsigned char *)sock->inAddr();
  sprintf(sock->myIPString, "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], 
	  bytes[3]);
  printf("%d  %s\n",sock->myFD, sock->myIPString);
  if ((sock->myFD < 0 && !myNonBlocking) || 
      (sock->myFD < 0 && errno != EWOULDBLOCK && myNonBlocking))
  {
    myErrorStr="accept失败";
    perror(myErrorStr.c_str());
    return(false);
  }

  return(true);
}

/*
 * 不用指定端口
 * 如果openOnIP指定了，则会将套接字绑定到该端口和IP地址 
 * */
bool RobotSocket::findValidPort(int startPort, const char *openOnIP)
{
	//char localhost[maxHostNameLen()];

	for (int i=0; i+startPort < 65000; ++i)
	{
		bzero(&mySin, sizeof(mySin));
		setIPString();
		if (openOnIP != NULL)
		{
			if (!hostAddr(openOnIP, mySin.sin_addr))
			{
				return(false); 
			}
			else
			{
				//printf("Opening on %s\n", openOnIP);
			}
		}
		else
		{
			mySin.sin_addr.s_addr=htonl(INADDR_ANY);
		}

		mySin.sin_family=AF_INET;
		mySin.sin_port=hostToNetOrder(startPort+i);

		if (bind(myFD, (struct sockaddr *)&mySin, sizeof(mySin)) == 0)
			break;
	}
	return(true);
}

