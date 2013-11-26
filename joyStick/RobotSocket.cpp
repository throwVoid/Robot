#include "RobotSocket.h"

/*
 * power by RenZhenwen
 * date: start 2012-12-25 ʥ����
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
 * ��Կͻ��ˣ���ŷ��������׽��ֽṹ
 * ��Է�������Ϊ�����׽��֣��洢���������׽��ֽṹ��Ϊ�����׽��֣���洢�ͻ����׽��ֽṹ��Ϣ
 * */






void RobotSocket::internalInit(void)
{
	myCloseFunctor = NULL;
	sprintf(myIPString, "none");
	resetTracking();
}
/*
 *@ Ϊ�����׽��ֺ���ؿ��ƵĹ��캯��
 * */
RobotSocket::RobotSocket() : 
  myType(Unknown),   	//�׽�������
  myError(NoErr),	//����ԭ��
  myErrorStr(),		//��������
  myDoClose(true),	
  myFD(-1),		//�׽���
  myNonBlocking(false), //������
  mySin()		//�׽��ֽṹ
{
	internalInit();
}




/*
 * ָ����myDoClose����Ҫ����ȫ��close���ر������׽���
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
 *  ����ָ��type���׽���
 * */
bool RobotSocket::create(Type type)
{
	if ((type == TCP) && ((myFD = socket(AF_INET, SOCK_STREAM, 0)) < 0))
	{
		myErrorStr = "[socket]����TCP�׽���ʧ��";
		perror(myErrorStr.c_str());
		return(false);
	}
	else if ((type == UDP) && ((myFD = socket(AF_INET, SOCK_DGRAM, 0)) < 0))
	{
		myErrorStr = "[socket]����UDP�׽���ʧ��";
		perror(myErrorStr.c_str());
		return(false);
	}
	myType = type;
	/*
	*
	* ���ﻹ�е����� 2012-12-29
	*
	*
	* */
	if (getSockName()) //��ȡ����׽��ֹ������׽��ֽṹ��
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
 *  �ͷ���ָ����������sin����ȡ����
 *  �����������׽���ָ���ͻ��˵�sin����ȡ����
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

	if (select(myFD + 1, NULL, &fdSet, NULL, &tval) <= 0)  //�ȴ��׽��ֿɶ�
		return 0;

	int ret;
	ret = ::write(myFD, (char*)buff, len);  //TCP�׽���д��

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
		if (select(myFD + 1, &fdSet, NULL, NULL, &tval) <= 0) //ʱ�����0�������ȴ�һ��
			return 0;
	}
	ret = ::recv(myFD, (char*)buff, len, 0);           //Ϊ0.
	if (ret > 0)
	{
		myRecvs ++;
		myBytesRecvd += ret;
	}
	return ret;
}

