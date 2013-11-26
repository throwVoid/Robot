#include "RobotPacketExternal.h"
/*
 * Power by RenZhenwen
 * Date:2013-03-01
 * Email:admin@unix8.net
 * */
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
//unsigned char RobotPacketExternal::idNo = 0;
RobotPacketExternal::RobotPacketExternal(unsigned char *buf, unsigned char t_maxSize, unsigned char h,
						 unsigned char t, unsigned char t_command) : RobotBasePacket(buf, t_maxSize, 7, 2)
{
	//头部标识
	head = h;						
	//尾部标识
	tail = t;
	//帧的属性
	command = t_command;
}

RobotPacketExternal::~RobotPacketExternal(void) throw()
{
}

/*下面这个函数只能针对一个完整的数据帧*/
int RobotPacketExternal::getValidDataLen(void)
{
	return myLength-getHeaderLength()-getFooterLength();
}
//按字节封包(封包前务必使实际数据已经压入帧,包封装完成后不能再修改)
void RobotPacketExternal::finalizePacket(void)
{
#if 0
	int len = myLength;											//暂存
	int chkSum;
	myLength = 0;
	uByteToBuf(head);											//追加头部识别			
	uByteToBuf(len - getHeaderLength()+1);						//+1使得ID也通过校验
	uByteToBuf(dataType);
	uByteToBuf(id);
	/*ID通过方法设置*/
	myLength = len;
	chkSum = calCheckSum();
	byteToBuf((chkSum >> 8) & 0xff );							//两个字节的校验码
	byteToBuf(chkSum & 0xff );
	uByteToBuf(tail);											//追加尾部
#endif
	int len = getLength();   //加了数据后的长度
	setLength(0);
	uByteToBuf(head);
	uByteToBuf(robotNo);
	uByteToBuf(id);
	uByteToBuf(command);
	uByteToBuf(objectFeatureHigh);
	uByteToBuf(objectFeatureLow);
	uByteToBuf(realDataLen);
	setLength(len);
	checkSum = calCheckSum();
	uByteToBuf(checkSum);
	uByteToBuf(tail);
}

unsigned char RobotPacketExternal::calCheckSum(void)			//计算校验和
{
#if 0
	int i;
	unsigned char n;
	int c = 0;
	i = 3;											// 从id开始校验
	n = myBuf[1];									//长度
	while (n > 1) 
	{
		c += ((unsigned char)myBuf[i]<<8) | (unsigned char)myBuf[i+1];
		c = c & 0xffff;
		n -= 2;
		i += 2;
	}
	if (n > 0) 
		c = c ^ (int)((unsigned char) myBuf[i]);
	return c;
#endif
	unsigned char sum = 0;
	int tLen = getHeaderLength() + getRealDataLenByIndex();
	for(int i = 0; i < tLen; i++)
	{
		sum += myBuf[i];
	}
	return  sum;
}

bool RobotPacketExternal::verifyCheckSum(void) //校验和验证
{
#if 0
	short chksum;
	unsigned char c1, c2;
	if (myLength - 3 < myHeaderLength)
		return false;

	c2 = myBuf[myLength - 3];
	c1 = myBuf[myLength - 2];
	chksum = (c1 & 0xff) | (c2 << 8);

	if (chksum == calcCheckSum()) 
	{
		return true;
	} 
	else 
	{
		return false;
	}
#endif
	unsigned char sum = 0;
	int tLen = getHeaderLength() + getRealDataLenByIndex();
	for(int i = 0; i < tLen; i++)
	{
		sum += myBuf[i];
	}
	if(sum == getCheckSumByIndex())
		return true;
	else
		return false;
}
#ifdef ROBOT_NAMESPACE
}
#endif
