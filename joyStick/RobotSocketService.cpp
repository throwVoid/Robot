#include "RobotSocket.h"


/*
 * power by RenZhenwen
 * date: start 2012-12-25 ʥ����
 * date: finish 2012-12-29
 * email: admin@unix8.net 
 * */

/**
 * 			��Ϊ��������ʱʹ�õĹ��캯��
 *@doClose ���׽�������ʱ���Ƿ��Զ��رո������׽���
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
	open(port, type);	//�����򿪣���������
}
/**
 * 			��Ϊ��������ʱʹ�õĹ��캯��
 *@doClose ���׽�������ʱ���Ƿ��Զ��رո������׽���
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
	open(port, type, host);	//�����򿪣���������

}
/*
 * ��Ϊ�������׽���
 * ����׽��ֵĴ�������
 * 
 * */
bool RobotSocket::open(int port, Type type, const char *openOnIP)
{
	int ret;
	char localhost[maxHostNameLen()];

	if ((type == TCP) && ((myFD=socket(AF_INET, SOCK_STREAM, 0)) < 0))
	{
		myErrorStr="[socket]����TCP�׽���ʧ��";
		perror(myErrorStr.c_str());
		return(false);
	}
	else if ((type == UDP) && ((myFD=socket(AF_INET, SOCK_DGRAM, 0)) < 0))
	{
		myErrorStr="[socket]����UDP�׽���ʧ��";
		perror(myErrorStr.c_str());
		return(false);
	}

	myType=type;
	bzero(&mySin, sizeof(mySin));

	if (openOnIP != NULL)
	{

		if (!hostAddr(openOnIP, mySin.sin_addr))
		{
			/*���ܲ��ҵ���IP����*/
			return(false); 
		}
		else
		{
    			/*�ɹ�*/
		}
	}
	else
	{
		mySin.sin_addr.s_addr = htonl(INADDR_ANY);  //������䱾���е�IP�����񽻸��ں�
	}

	setIPString();
	mySin.sin_family = AF_INET;
	mySin.sin_port = hostToNetOrder(port);

	if ((ret=bind(myFD, (struct sockaddr *)&mySin, sizeof(mySin))) < 0)
	{
		myErrorStr="[bind]��ʧ��";
		sprintf(localhost, "%d", port);
		myErrorStr+=localhost;
		perror(myErrorStr.c_str());
		return(false);
	}

	if ((type == TCP) && (listen(myFD, 5) < 0))
	{
		myErrorStr="[listen]��ʧ��";
		perror(myErrorStr.c_str());
		return(false);
	}

	return(true);
}

/*
 * ע��㣺���Ϊ������ģʽ��û���׽��������Ӳ�������һ������
 * ʹ��getFd����Ƿ��Ǹ���Ч��FD
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
    myErrorStr="acceptʧ��";
    perror(myErrorStr.c_str());
    return(false);
  }

  return(true);
}

/*
 * ����ָ���˿�
 * ���openOnIPָ���ˣ���Ὣ�׽��ְ󶨵��ö˿ں�IP��ַ 
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

