
#include "RobotSocket.h"

/*
 * power by RenZhenwen
 * date: start 2012-12-25 ʥ����
 * date: finish 2012-12-29
 * email: admin@unix8.net 
 * */
/*
 *
 * 	��Ϊ�ͻ����׽���ʱ���õĹ��캯��
 * @host Ϊ�ͻ���׼�����ӵķ�����IP
 * @port ���ӵĶ˿�
 * @type �׽�������
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
	internalInit();			//��ʼ��һЩ����
	connect(host, port, type);   //��������
}



/*
 * ��type�׽�������������Ŀ�ķ���������
 * hostΪNULL�������ӱ����ķ�����
 * ����׽��ֵĴ�����������������
 * */
bool RobotSocket::connect(const char *host, int port, Type type)
{
	char localhost[maxHostNameLen()];

	if (!host) //host ΪNULL����Ϊ����localhost 
	{
		if (gethostname(localhost, sizeof(localhost)) == 1)
		{
			myError=ConBadHost;
			myErrorStr="[gethostbyname]����������ȡ������ַʧ��";
			myErrorStr+=localhost;
			myErrorStr+="'";
			perror(myErrorStr.c_str());
			return(false);
		}
		host=localhost; //�Ѿ���ȡ�����ؼ������
	}

	bzero(&mySin, sizeof(mySin));
	if (!hostAddr(host, mySin.sin_addr))//�ѻ�ȡ��������ת���ɵ�ַ�ṹ��
		return(false);
	setIPString();		//����IP�ַ���
	mySin.sin_family=AF_INET;
	mySin.sin_port=hostToNetOrder(port);

	if ((type == TCP) && ((myFD=socket(AF_INET, SOCK_STREAM, 0)) < 0))
	{
		myError=NetFail;
		myErrorStr="[socket]����TCP�׽���ʧ��";
		perror(myErrorStr.c_str());
		return(false);
	}
	else if ((type == UDP) && ((myFD=socket(AF_INET, SOCK_DGRAM, 0)) < 0))
	{
		myError=NetFail;
		myErrorStr="[socket]����UDP�׽���ʧ��";
		perror(myErrorStr.c_str());
		return(false);
	}

	myType=type;

	if (::connect(myFD, (struct sockaddr *)&mySin,
		sizeof(struct sockaddr_in)) < 0)
	{
		myErrorStr="�����׽���ʧ��";
		switch (errno)
		{
		case ECONNREFUSED:
			myError=ConRefused;
			myErrorStr+="; ���ӱ���";
			break;
		case ENETUNREACH:
			myError=ConNoRoute;
			myErrorStr+="; û�е���Ŀ�ĵص�·��";
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
 * ��Ϊ�ͻ���ȥ����һ��������
 * �׽����Ѿ�����
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
		  myErrorStr="Զ����������ʧ��";
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

/*����ȥ���ӷ�����*/
bool RobotSocket::connectTo(struct sockaddr_in *sin)
{
	if (::connect(myFD, (struct sockaddr *)sin,
		sizeof(struct sockaddr_in)) < 0)
	{
		myErrorStr="[connect]connectʧ��";
		perror(myErrorStr.c_str());
		return(0);
	}

	return(1);
}
