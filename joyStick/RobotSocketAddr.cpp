
#include "RobotSocket.h"
/*
 * power by RenZhenwen
 * date: start 2012-12-25 圣诞节
 * date: finish 2012-12-29
 * email: admin@unix8.net 
 * */

/*
 *  由主机名得到主机的地址
 *@ host 为name，返回值在值结果参数addr中
 *
 * */
bool RobotSocket::hostAddr(const char *host, struct in_addr &addr)  
{
	struct hostent *hp;
	if (!(hp = gethostbyname(host)))
	{	
		string errorStr = "[gethostbyname]由主机名获取主机地址失败";
		perror(errorStr.c_str());
		memset(&addr, 0, sizeof(in_addr));
		return(false);
	}
	else
	{
	/*void bcopy(const void *src, void *dest, size_t n);*/
		bcopy(hp->h_addr, &addr, hp->h_length);
		return(true);
	}
}

/*
 *
 * 由地址得到主机名
 * */
bool RobotSocket::addrHost(struct in_addr &addr, char *host)
{
	struct hostent *hp;
	hp = gethostbyaddr((char*)&addr.s_addr, sizeof(addr.s_addr), AF_INET);
	if (hp)
		strcpy(host, hp->h_name);
	else
		strcpy(host, inet_ntoa(addr));
	return(true);
}

/*
 * 得到本地主机名
 * */
std::string RobotSocket::getHostName()   
{
	char localhost[maxHostNameLen()];

	if (gethostname(localhost, sizeof(localhost)) == 1)
		return("");
	else
		return(localhost);
}

void RobotSocket::setIPString(void)
{
	unsigned char *bytes;
	bytes = (unsigned char *)inAddr();  //结构体表示的IP地址转换到字节表示的序列
	if (bytes != NULL)
		sprintf(myIPString, "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
}
/*
 * 拷贝一个套接字结构体，来源是参数fd。
 * 
 * */
bool RobotSocket::copy(int fd, bool doclose)
{
	socklen_t len;

	myFD = fd;
	myDoClose = doclose;
	myType = Unknown;

	len = sizeof(struct sockaddr_in);
	if (getsockname(myFD, (struct sockaddr*)&mySin, &len))
	{
		myErrorStr="[getsockname]获取sockname出错 ";
		perror(myErrorStr.c_str());
		return(false);
	}
	else
		return(true);
}
/*
 * 把in_addr地址结构转换到一个字符串内
 *
 * */
void RobotSocket::inToA(struct in_addr *addr, char *buff)
{
	strcpy(buff, inet_ntoa(*addr));
}

/*
 * 测试是否套接字有效，有两个测试点
 * 1.socket fd是否已经创建
 * 2. 测试套接字结构体的有效性
 *  getsockname在《Unix网络编程》第一卷的99页
 * */
bool RobotSocket::getSockName() 
{
	socklen_t size;

	if (myFD < 0)
	{
		myErrorStr = "[myFD]当前套接字描述符无效";
		perror(myErrorStr.c_str());
		return(false);
	}

	size = sizeof(mySin);
	if (getsockname(myFD, (struct sockaddr *)&mySin, &size) != 0)
	{
		myErrorStr="[getsockname]获取套接字name失败";
		perror(myErrorStr.c_str());
		return(false);
	}
	return(true);
}

/*
 * 主机字节序转换成网络字节序
 * */
unsigned int RobotSocket::hostToNetOrder(int i)
{
	return(htons(i));
}
/*
 * 网络字节序转换成主机字节序
 * */
unsigned int RobotSocket::netToHostOrder(int i)
{
	return(ntohs(i));
}
