#pragma once
#ifndef _ROBOT_PACKET_
#define _ROBOT_PACKET_

/*
 *date: 2012/11/6
 *author:Renzhenwen
 *机器人数据帧封装头文件，暂未封装机器人类型和数据帧属性。
 */

#include "RobotBasePacket.h"
//#include "RobotTypeDef.h"
#include <cstring>
using namespace std;

//PC104与远程控制台，PC104与其他机器人PC104采用RobotPacketExtenal通信
//PC104与机器人通信则采用精简版协议RobotPacket通信
//目的：减少机器人单片机程序的改动和减少串口数据量
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
class RobotPacket: public RobotBasePacket
{
public:
	//帧标志
	typedef enum FrameFlag {Start = 0xfb, End = 0xfe} FrameFlag;	
	//数据包类型(机器人与PC104之间)
	typedef enum PacketType {ControlWheelCommand = 0x00, ControlLightCommand = 0x01,
													SonarData = 0x02, CompassAndSpeedMeter = 0x04} PacketType;  
	explicit RobotPacket(unsigned char *buf = NULL, unsigned char t_maxSize = 40,
				unsigned char fs = Start, unsigned char fe  = End , unsigned char dt = ControlWheelCommand);
	~RobotPacket(void) throw();
	//如果校验和正确则返回true
	bool verifyCheckSum(void);
	//计算校验和
	unsigned char calCheckSum(void);			
	//完成包封装	
	void finalizePacket(void);					
	//重载的封包器
	void finalizePacket(const unsigned char* arr, int len){copyFromCharArry(arr, len);}
	//获取数据包实际的负载数据长度，必须在一个完整包情况下应用
	int getValidDataLen(void);	  

	//unsigned char getId(void)const {return id;}				
	//void setId(unsigned char t_id) {id = t_id;}	

	unsigned char getLen(void) const {return len;}
	void setLen(unsigned char t_len) {len = t_len;}
	unsigned char getLenByIndex() {return myBuf[1];}

	unsigned char getCommand(void) const {return command;}
	void setCommand(unsigned char t_command) {command = t_command;}
	unsigned char getCommandByIndex() {return myBuf[2];}

	unsigned char getCheckSum(void) const {return checkSum;}
	void setCheckSum(unsigned char t_checkSum) {checkSum = t_checkSum;}
	unsigned char getCheckSumByIndex(void) const {return myBuf[myBuf[1]];}

	unsigned char getByteByIndex(int i) {return myBuf[i];}
protected:
	unsigned char head;  			//帧头	
	unsigned char len;				//帧长
	//unsigned char id;				//帧编号
	unsigned char command;			//控制命令字
	unsigned char checkSum;			//校验和
	unsigned char tail;				//尾巴
private:
	void copyFromCharArry(const unsigned char *arr, int tLen) //将一个数组整体拷贝给帧，包括校验
	{
		if(arr != NULL)
		{
			bcopy(arr, getBuf(), tLen);
			setLength(tLen); //设置长度
		}
		else
			throw std::string("arr is NULL");
	}
};
#ifdef ROBOT_NAMESPACE
}
#endif
#endif
