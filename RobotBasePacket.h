#pragma once
#ifndef ROBOT_MAIN_BASEPACKET_H_
#define ROBOT_MAIN_BASEPACKET_H_

/*
 * Power by RenZhenwen
 * Date:2012-12-07
 * Email:admin@unix8.net
 *
 * */
#include <string>
//#include "RobotDeBug.h"
//#include "RobotMath.h"
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
class RobotBasePacket
{

public:  
	/*内存区地址+内存区长度+头部长度+尾部长度*/
	explicit RobotBasePacket(unsigned char * buf = NULL, unsigned short bufferSize = 0,  
								unsigned short headerLength = 0, unsigned short footerLength = 0);
	virtual ~RobotBasePacket(void) throw();
	//组装帧
	virtual void finalizePacket(void) = 0;	
	virtual void finalizePacket(const unsigned char*, int) = 0;
	//把包的实际负载清和尾部除掉,留下帧头部方便后续增加。原理是：读和写指针都归位
	void empty(void);		
	//验证数据帧是否可用
	bool isValid(void);						
	//重置可用标识
	void resetValid(void);						
	//以16进制打印数据帧
	virtual void printHex(void);					 


	/*以下函数是为写入不同的数据类型到缓冲区，注意长度的变化*/
	void byteToBuf(char val);
	void byte2ToBuf(short val);
	void byte4ToBuf(int val);
	void uByteToBuf(unsigned char val);
	void uByte2ToBuf(unsigned short val);
	void uByte4ToBuf(unsigned int val);
	void dataToBuf(const char *data, int length);
	void dataToBuf(const unsigned char *data, int length);


	char bufToByte(void);	
	short bufToByte2(void);	
	int bufToByte4(void);
	unsigned char bufToUByte(void);
	unsigned short bufToUByte2(void);
	unsigned int bufToUByte4(void);
	void bufToData(char *data, int length);
	void bufToData(unsigned char *data, int length);

	//恢复读取指针
	void resetRead(void);
	//获取包的现有长度，包括头部和负载，如果已经装载上尾部，则还包含尾部字节数
	unsigned short getLength(void) { return myLength; }
	//获取包去除首后的长度,这里并没有包含尾部的原因是：尾部的封装交给了派生类来管理
	unsigned short getDataLength(void) { return myLength - myHeaderLength /*- myFooterLength*/; }
	//获取读索引
	unsigned short getReadLength(void) { return myReadLength; }							
	//一帧数据中有好多数据已经被读不包括头部
	unsigned short getDataReadLength(void) { return myReadLength - myHeaderLength; }			
	//获取头部长度
	unsigned short getHeaderLength(void)	{ return myHeaderLength; }
	//获取尾部长度
	unsigned short getFooterLength(void)	{ return myFooterLength; }
	//获取包的最大长度
	unsigned short getMaxLength(void) { return myMaxLength; }
	//返回数据包头部指针
	/*const*/  unsigned char * getBuf(void)	{ return myBuf; }
	//设置外部缓冲区
	void setBuf(unsigned char *buf, unsigned short bufferSize);			
	//设置包的最大长度，如果设置的比目前的长度还小的话则什么也不做，不会缩减长度
	void setMaxLength(unsigned short bufferSize);
	//设置包的现有总长度，包括头部
	bool setLength(unsigned short length);	
	//设置读索引位置
	void setReadLength(unsigned short readLength);		
	//设置头部长度
	bool setHeaderLength(unsigned short length);			
	//数据帧的复制
	void dupPacket(RobotBasePacket *packet);
protected:
	//测试下一个读写位置是否有效
	bool isNextGood(int bytes);										
	//测试缓冲区是否有可用的空间来增加数据
	bool hasWriteCapacity(int bytes);				
	/*头部长度*/
	unsigned short myHeaderLength;							
	/*尾部长度*/
	unsigned short myFooterLength;						
	/*最大包长度*/
	unsigned short myMaxLength;							
	/*read索引*/
	unsigned short myReadLength;							
	/*实际的数据缓冲区*/
	unsigned char *myBuf;										
	/*实际的数据缓冲区长度*/
	unsigned short myLength;								
	/*指示外部缓冲还是只使用类的内部缓冲*/
	bool myOwnMyBuf;								
	/*主要测试读写的时候有没有出错*/
	bool myIsValid;														
};  //end for class
#ifdef ROBOT_NAMESPACE
}
#endif
#endif //ROBOT_MAIN_BASEPACKET_H_
