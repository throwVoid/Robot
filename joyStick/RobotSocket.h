#pragma once
#ifndef ARSOCKET_H
#define ARSOCKET_H
/*
 * power by RenZhenwen
 * date: start 2012-12-25 圣诞节
 * date: finish 2012-12-29
 * email: admin@unix8.net 
 * */

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#include <string>
#include <cstring>
#include "RobotFunctor.h"

class RobotFunctor;
class RobotSocket
{
public:
	enum Type {UDP, TCP, Unknown};  //套接字类型
	enum Error {NoErr, NetFail, ConBadHost, ConNoRoute, ConRefused}; //连接时可能的出错状态

	/*构造函数，不会指定是客户端还是服务器端*/
	RobotSocket();

	/*构造函数-作为客户端主动去连接一个指定目的地的服务器*/
	RobotSocket(const char *host, int port, Type type); 

	/*构造函数-作为服务器端主动打开端口port，并等待连接，doClose用来指定是否在套接字析构时关闭连接*/
	RobotSocket(int port, bool doClose, Type type); 
	RobotSocket(const  char* host, int port, bool doClose, Type type); 


	/*析构函数*/
	~RobotSocket();


	/*
	*  函数暂时没任何功能，用于扩展
	* */
	static bool init();					//初始化操作系统套接字系统
	static void shutdown();				//关闭操作系统套接字系统
	static bool ourInitialized;				//用于指示是否已经初始化，已经设置成true了
	/*
	*
	* */


	/*拷贝套接字结构*/
	bool copy(int fd, bool doclose);
	void copy(RobotSocket *s)
	{myFD=s->myFD; myDoClose=false; mySin=s->mySin;}			
	/*更改套接字拥有者，参数指代的套接字在析构的时候将不会关闭这个描述符*/
	void transfer(RobotSocket *s)
	{myFD=s->myFD; myDoClose=true; s->myDoClose=false; mySin=s->mySin;
	 myType=s->myType; strcpy(myIPString, s->myIPString); }

	/*作为一个客户端去主动连接服务器，默认为TCP连接*/
	bool connect(const char *host, int port, Type type = TCP);

	/* 打开一个端口，作为服务器
	* openOnIp如果已经指定，则指定IP
	* */
	bool open(int port, Type type, const char *openOnIP = NULL);

	/*仅仅创建套接字和检测该套接字是否有效（这其中会赋值mySin）*/
	bool create(Type type);

	/* 查找起点为startPoint的端口，绑定到套接字
	* 如果指定了openonIp则会在这个IP地址上查找端口
	* */
	bool findValidPort(int startPort, const char *openOnIP = NULL);

	/*连接指定服务器的指定端口*/
	bool connectTo(const char *host, int port);
	/*连接指定的套接字结构体服务器*/
	bool connectTo(struct sockaddr_in *sin);

	/*服务器接收客户端的连接，客户端的套接字类信息是由参数返回*/
	bool accept(RobotSocket *sock);

	/*关闭套接字，如果注册了关闭时的回调函数，则还会调用函数*/
	bool close();

	/*
	* 下面两个函数用于TCP套接字
	* */
	/*将数据写到套接口并发送*/
	int write(const void *buff, size_t len);

	/*从套接口读数据，如果设置了时间，则最多等待msWait*/
	int read(void *buff, size_t len, unsigned int msWait = 0);

	/*
	* 下面两个函数用于udP套接字
	* */
	/*调用sendto来进行发送，使用地址结构为mySin,套接字为myFD,会修改发送的次数和字节数（UDP）*/
	int sendTo(const void *msg, int len);  //
	int sendTo(const void *msg, int len, struct sockaddr_in *sin);  //指定套接字结构，不使用mySIn（UDP）
	int recvFrom(void *msg, int len, sockaddr_in *sin);  //这里关心数据的来源（UDP）

	/*转换一个hoststring到地址结构体*/
	static bool hostAddr(const char *host, struct in_addr &addr);

	/*转换一个地址结构体为一个字符串*/
	static bool addrHost(struct in_addr &addr, char *host);

	/*获取本地计算机的名字*/
	static std::string getHostName();


	/* 获取套接字的名字,存储在RobotSocket::mySin*/
	bool getSockName();

	/*获取套接字结构体的地址*/
	struct sockaddr_in * sockAddrIn() {return(&mySin);}

	/*获取Ip地址的地址*/
	struct in_addr * inAddr() {return(&mySin.sin_addr);}  //返回struct in_addr结构体表示的IP地址的指针

	/*获取端口*/
	unsigned short int inPort() {return(mySin.sin_port);}

	/*转换地址为numerical地址字符串*/
	static void inToA(struct in_addr *addr, char *buff);

	/*获取套接字结构体大小*/
	static const size_t sockAddrLen() {return(sizeof(struct sockaddr_in));}

	/*获取系统支持的最大主机名长度*/
	static const size_t maxHostNameLen() {return(MAXHOSTNAMELEN);}

	/*主机字节序转换成网络字节序*/
	static unsigned int hostToNetOrder(int i);

	/*网络字节序转换成主机字节序*/
	static unsigned int netToHostOrder(int i);

	/*设置套接字linger选项*/
	bool setLinger(int time);

	/*设置广播选项*/
	bool setBroadcast();

	/*设置地址重用*/
	bool setReuseAddress();

	/*设置套接口的阻塞选项*/
	bool setNonBlock();

	/*设置doClose*/
	void setDoClose(bool yesno) {myDoClose=yesno;}

	/*返回套接字描述符*/
	int getFD() const {return(myFD);}


	/*获取协议族*/
	Type getType() const {return(myType);}

	/*获取最近的一次错误描述*/
	const std::string & getErrorStr() const {return(myErrorStr);}

	/*获取最近一次错误类型*/
	Error getError() const {return(myError);}

	/*把套接口结构体中的IP地址转换成字符串*/
	const char *getIPString(void) const { return myIPString; }
	/*设置关闭套接字的时候的动作*/
	void setCloseCallback(RobotFunctor *functor) 
	{ myCloseFunctor = functor; }
	/*获取关闭时采取的动作*/
	RobotFunctor *getCloseCallback(void) { return myCloseFunctor; }
	/*获取已经发送（write）的次数*/
	long getSends(void) { return mySends; }
	/*获取已经发送的字节数*/
	long getBytesSent(void) { return myBytesSent; }
	/*获取接收的次数*/
	long getRecvs(void) { return myRecvs; }
	/*获取接收的字节数*/
	long getBytesRecvd(void) { return myBytesRecvd; }
	/*重置发送和接收计数*/
	void resetTracking(void) 
	{ mySends = 0; myBytesSent = 0; myRecvs = 0; myBytesRecvd = 0; }

	/*禁止时延*/
	bool setNoDelay(bool flag);

protected:
	/*把mySin.sin_addr结构体表示的地址转换成点分十进制字符串存入myIpString*/
	void setIPString(void);
	/*初始化一些string buff*/
	void internalInit(void);

	Type myType;
	Error myError;
	std::string myErrorStr;
	bool myDoClose;
	int myFD;   				//套接字描述符
	bool myNonBlocking;			//套接字阻塞标志
	struct sockaddr_in mySin;		//网络套接字地址结构

	char myIPString[128];  		//IP地址字符串结构

	long mySends;				//记录发送的次数
	long myBytesSent;			//记录发送的字节数
	long myRecvs;				//记录接收的次数
	long myBytesRecvd;			//记录接收的字节数

	RobotFunctor *myCloseFunctor;
};


#endif 
  
