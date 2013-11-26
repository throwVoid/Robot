#include "RobotSocket.h"
/*
 * power by RenZhenwen
 * date: start 2012-12-25 ʥ����
 * date: finish 2012-12-29
 * email: admin@unix8.net 
 * */
/*
 * ������������TCP�ģ������׽ӿڵ�SO_LINGERѡ��
 *
 * */
bool RobotSocket::setLinger(int time)
{
	struct linger lin;
	/*
	struct linger
	{
	int l_onoff;
	int l_linger;
	}
	*/
	if (time)
	{
		lin.l_onoff=1;
		lin.l_linger=time; //������׽ӿڻ������������ݣ���ô���̾�����ֱ���������ݷ��겢��ACK�Ѿ��յ���
	}
	else
	{
		lin.l_onoff=0;
		lin.l_linger=time;
	}

	if (setsockopt(myFD, SOL_SOCKET, SO_LINGER, &lin, sizeof(lin)) != 0)
	{
		myErrorStr="�����׽���ѡ��LINGERʧ��";
		perror(myErrorStr.c_str());
		return(false);
	}
	else
		return(true);
}

/*
 * ��ֹUDP�Ĺ㲥
 * */
bool RobotSocket::setBroadcast()
{
	if (setsockopt(myFD, SOL_SOCKET, SO_BROADCAST, NULL, 0) != 0)
	{
		myErrorStr = "���ù㲥�׽���ѡ��ʧ��";
		perror(myErrorStr.c_str());
		return(false);
	}
	else
		return(true);
}
/*
 * �����ַ����
 * */
bool RobotSocket::setReuseAddress()
{
	int opt=1;

	if (setsockopt(myFD, SOL_SOCKET, SO_REUSEADDR,
		 (char*)&opt, sizeof(opt)) != 0)
	{
		myErrorStr="��ַ����ѡ������ʧ��";
		perror(myErrorStr.c_str());
		return(false);
	}
	else
		return(true);
}

/*
 *  �����׽ӿڵķ�����
 * */
bool RobotSocket::setNonBlock()
{
	if (fcntl(myFD, F_SETFL, O_NONBLOCK) != 0)
	{
		myErrorStr="���÷�����ʧ��";
		perror(myErrorStr.c_str());
		return(false);
	}
	else
	{
		myNonBlocking = true;
		return(true);
	}
}

/*
 * �������һ��TCP���ӣ���������TCP_nodelayѡ����ֹʹ��nagle�㷨
 * */
bool RobotSocket::setNoDelay(bool flag)
{
	if(myType != TCP) return false;
	int f = flag?1:0;
	int r = setsockopt(myFD, IPPROTO_TCP, TCP_NODELAY, (char*)&f, sizeof(f));
	return (r != -1);
}

