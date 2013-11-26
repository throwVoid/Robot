#pragma once
#ifndef ARSOCKET_H
#define ARSOCKET_H
/*
 * power by RenZhenwen
 * date: start 2012-12-25 ʥ����
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
	enum Type {UDP, TCP, Unknown};  //�׽�������
	enum Error {NoErr, NetFail, ConBadHost, ConNoRoute, ConRefused}; //����ʱ���ܵĳ���״̬

	/*���캯��������ָ���ǿͻ��˻��Ƿ�������*/
	RobotSocket();

	/*���캯��-��Ϊ�ͻ�������ȥ����һ��ָ��Ŀ�ĵصķ�����*/
	RobotSocket(const char *host, int port, Type type); 

	/*���캯��-��Ϊ�������������򿪶˿�port�����ȴ����ӣ�doClose����ָ���Ƿ����׽�������ʱ�ر�����*/
	RobotSocket(int port, bool doClose, Type type); 
	RobotSocket(const  char* host, int port, bool doClose, Type type); 


	/*��������*/
	~RobotSocket();


	/*
	*  ������ʱû�κι��ܣ�������չ
	* */
	static bool init();					//��ʼ������ϵͳ�׽���ϵͳ
	static void shutdown();				//�رղ���ϵͳ�׽���ϵͳ
	static bool ourInitialized;				//����ָʾ�Ƿ��Ѿ���ʼ�����Ѿ����ó�true��
	/*
	*
	* */


	/*�����׽��ֽṹ*/
	bool copy(int fd, bool doclose);
	void copy(RobotSocket *s)
	{myFD=s->myFD; myDoClose=false; mySin=s->mySin;}			
	/*�����׽���ӵ���ߣ�����ָ�����׽�����������ʱ�򽫲���ر����������*/
	void transfer(RobotSocket *s)
	{myFD=s->myFD; myDoClose=true; s->myDoClose=false; mySin=s->mySin;
	 myType=s->myType; strcpy(myIPString, s->myIPString); }

	/*��Ϊһ���ͻ���ȥ�������ӷ�������Ĭ��ΪTCP����*/
	bool connect(const char *host, int port, Type type = TCP);

	/* ��һ���˿ڣ���Ϊ������
	* openOnIp����Ѿ�ָ������ָ��IP
	* */
	bool open(int port, Type type, const char *openOnIP = NULL);

	/*���������׽��ֺͼ����׽����Ƿ���Ч�������лḳֵmySin��*/
	bool create(Type type);

	/* �������ΪstartPoint�Ķ˿ڣ��󶨵��׽���
	* ���ָ����openonIp��������IP��ַ�ϲ��Ҷ˿�
	* */
	bool findValidPort(int startPort, const char *openOnIP = NULL);

	/*����ָ����������ָ���˿�*/
	bool connectTo(const char *host, int port);
	/*����ָ�����׽��ֽṹ�������*/
	bool connectTo(struct sockaddr_in *sin);

	/*���������տͻ��˵����ӣ��ͻ��˵��׽�������Ϣ���ɲ�������*/
	bool accept(RobotSocket *sock);

	/*�ر��׽��֣����ע���˹ر�ʱ�Ļص��������򻹻���ú���*/
	bool close();

	/*
	* ����������������TCP�׽���
	* */
	/*������д���׽ӿڲ�����*/
	int write(const void *buff, size_t len);

	/*���׽ӿڶ����ݣ����������ʱ�䣬�����ȴ�msWait*/
	int read(void *buff, size_t len, unsigned int msWait = 0);

	/*
	* ����������������udP�׽���
	* */
	/*����sendto�����з��ͣ�ʹ�õ�ַ�ṹΪmySin,�׽���ΪmyFD,���޸ķ��͵Ĵ������ֽ�����UDP��*/
	int sendTo(const void *msg, int len);  //
	int sendTo(const void *msg, int len, struct sockaddr_in *sin);  //ָ���׽��ֽṹ����ʹ��mySIn��UDP��
	int recvFrom(void *msg, int len, sockaddr_in *sin);  //����������ݵ���Դ��UDP��

	/*ת��һ��hoststring����ַ�ṹ��*/
	static bool hostAddr(const char *host, struct in_addr &addr);

	/*ת��һ����ַ�ṹ��Ϊһ���ַ���*/
	static bool addrHost(struct in_addr &addr, char *host);

	/*��ȡ���ؼ����������*/
	static std::string getHostName();


	/* ��ȡ�׽��ֵ�����,�洢��RobotSocket::mySin*/
	bool getSockName();

	/*��ȡ�׽��ֽṹ��ĵ�ַ*/
	struct sockaddr_in * sockAddrIn() {return(&mySin);}

	/*��ȡIp��ַ�ĵ�ַ*/
	struct in_addr * inAddr() {return(&mySin.sin_addr);}  //����struct in_addr�ṹ���ʾ��IP��ַ��ָ��

	/*��ȡ�˿�*/
	unsigned short int inPort() {return(mySin.sin_port);}

	/*ת����ַΪnumerical��ַ�ַ���*/
	static void inToA(struct in_addr *addr, char *buff);

	/*��ȡ�׽��ֽṹ���С*/
	static const size_t sockAddrLen() {return(sizeof(struct sockaddr_in));}

	/*��ȡϵͳ֧�ֵ��������������*/
	static const size_t maxHostNameLen() {return(MAXHOSTNAMELEN);}

	/*�����ֽ���ת���������ֽ���*/
	static unsigned int hostToNetOrder(int i);

	/*�����ֽ���ת���������ֽ���*/
	static unsigned int netToHostOrder(int i);

	/*�����׽���lingerѡ��*/
	bool setLinger(int time);

	/*���ù㲥ѡ��*/
	bool setBroadcast();

	/*���õ�ַ����*/
	bool setReuseAddress();

	/*�����׽ӿڵ�����ѡ��*/
	bool setNonBlock();

	/*����doClose*/
	void setDoClose(bool yesno) {myDoClose=yesno;}

	/*�����׽���������*/
	int getFD() const {return(myFD);}


	/*��ȡЭ����*/
	Type getType() const {return(myType);}

	/*��ȡ�����һ�δ�������*/
	const std::string & getErrorStr() const {return(myErrorStr);}

	/*��ȡ���һ�δ�������*/
	Error getError() const {return(myError);}

	/*���׽ӿڽṹ���е�IP��ַת�����ַ���*/
	const char *getIPString(void) const { return myIPString; }
	/*���ùر��׽��ֵ�ʱ��Ķ���*/
	void setCloseCallback(RobotFunctor *functor) 
	{ myCloseFunctor = functor; }
	/*��ȡ�ر�ʱ��ȡ�Ķ���*/
	RobotFunctor *getCloseCallback(void) { return myCloseFunctor; }
	/*��ȡ�Ѿ����ͣ�write���Ĵ���*/
	long getSends(void) { return mySends; }
	/*��ȡ�Ѿ����͵��ֽ���*/
	long getBytesSent(void) { return myBytesSent; }
	/*��ȡ���յĴ���*/
	long getRecvs(void) { return myRecvs; }
	/*��ȡ���յ��ֽ���*/
	long getBytesRecvd(void) { return myBytesRecvd; }
	/*���÷��ͺͽ��ռ���*/
	void resetTracking(void) 
	{ mySends = 0; myBytesSent = 0; myRecvs = 0; myBytesRecvd = 0; }

	/*��ֹʱ��*/
	bool setNoDelay(bool flag);

protected:
	/*��mySin.sin_addr�ṹ���ʾ�ĵ�ַת���ɵ��ʮ�����ַ�������myIpString*/
	void setIPString(void);
	/*��ʼ��һЩstring buff*/
	void internalInit(void);

	Type myType;
	Error myError;
	std::string myErrorStr;
	bool myDoClose;
	int myFD;   				//�׽���������
	bool myNonBlocking;			//�׽���������־
	struct sockaddr_in mySin;		//�����׽��ֵ�ַ�ṹ

	char myIPString[128];  		//IP��ַ�ַ����ṹ

	long mySends;				//��¼���͵Ĵ���
	long myBytesSent;			//��¼���͵��ֽ���
	long myRecvs;				//��¼���յĴ���
	long myBytesRecvd;			//��¼���յ��ֽ���

	RobotFunctor *myCloseFunctor;
};


#endif 
  
