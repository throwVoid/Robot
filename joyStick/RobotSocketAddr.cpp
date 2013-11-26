
#include "RobotSocket.h"
/*
 * power by RenZhenwen
 * date: start 2012-12-25 ʥ����
 * date: finish 2012-12-29
 * email: admin@unix8.net 
 * */

/*
 *  ���������õ������ĵ�ַ
 *@ host Ϊname������ֵ��ֵ�������addr��
 *
 * */
bool RobotSocket::hostAddr(const char *host, struct in_addr &addr)  
{
	struct hostent *hp;
	if (!(hp = gethostbyname(host)))
	{	
		string errorStr = "[gethostbyname]����������ȡ������ַʧ��";
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
 * �ɵ�ַ�õ�������
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
 * �õ�����������
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
	bytes = (unsigned char *)inAddr();  //�ṹ���ʾ��IP��ַת�����ֽڱ�ʾ������
	if (bytes != NULL)
		sprintf(myIPString, "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
}
/*
 * ����һ���׽��ֽṹ�壬��Դ�ǲ���fd��
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
		myErrorStr="[getsockname]��ȡsockname���� ";
		perror(myErrorStr.c_str());
		return(false);
	}
	else
		return(true);
}
/*
 * ��in_addr��ַ�ṹת����һ���ַ�����
 *
 * */
void RobotSocket::inToA(struct in_addr *addr, char *buff)
{
	strcpy(buff, inet_ntoa(*addr));
}

/*
 * �����Ƿ��׽�����Ч�����������Ե�
 * 1.socket fd�Ƿ��Ѿ�����
 * 2. �����׽��ֽṹ�����Ч��
 *  getsockname�ڡ�Unix�����̡���һ���99ҳ
 * */
bool RobotSocket::getSockName() 
{
	socklen_t size;

	if (myFD < 0)
	{
		myErrorStr = "[myFD]��ǰ�׽�����������Ч";
		perror(myErrorStr.c_str());
		return(false);
	}

	size = sizeof(mySin);
	if (getsockname(myFD, (struct sockaddr *)&mySin, &size) != 0)
	{
		myErrorStr="[getsockname]��ȡ�׽���nameʧ��";
		perror(myErrorStr.c_str());
		return(false);
	}
	return(true);
}

/*
 * �����ֽ���ת���������ֽ���
 * */
unsigned int RobotSocket::hostToNetOrder(int i)
{
	return(htons(i));
}
/*
 * �����ֽ���ת���������ֽ���
 * */
unsigned int RobotSocket::netToHostOrder(int i)
{
	return(ntohs(i));
}
