#include "RobotPacket.h"
/*
 * Power by RenZhenwen
 * Date:2012-12-07
 * Email:admin@unix8.net
 * */
//unsigned char RobotPacket::idNo = 0;
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
RobotPacket::RobotPacket(unsigned char *buf, unsigned char t_maxSize, unsigned char h,
						 unsigned char t, unsigned char t_command) : RobotBasePacket(buf, t_maxSize, 3, 2)
{
	head = h;						//头部标识
	tail = t;						//尾部标识
	command = t_command;			//
}

RobotPacket::~RobotPacket(void) throw(){}

/*下面这个函数只能针对一个完整的数据帧*/
int RobotPacket::getValidDataLen(void)
{
	return getLength() - getHeaderLength() - getFooterLength();
}

void RobotPacket::finalizePacket(void)
{
	int tLen = getLength();
	setLength(0);
	uByteToBuf(head);
	uByteToBuf(tLen);
	uByteToBuf(command);
	setLength(tLen);
	unsigned char chkSum = calCheckSum();
	uByteToBuf(chkSum);
	uByteToBuf(tail);
}

unsigned char RobotPacket::calCheckSum(void)			//计算校验和
{
	unsigned char sum = 0;
	//这里还没有封装尾部
	for(int i = 0; i < getLength(); ++i)
	{
		sum += myBuf[i];
	}
	return  sum;
}

bool RobotPacket::verifyCheckSum(void) //校验和验证
{
	unsigned char sum = 0;
	for(int i = 0; i < getLenByIndex(); ++i)
		sum += myBuf[i];
	return ((sum == getCheckSumByIndex()) ? true:false);
}
#ifdef ROBOT_NAMESPACE
}
#endif
