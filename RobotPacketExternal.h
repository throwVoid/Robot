#pragma once
#ifndef ROBOT_MAIN_PACKET_EXTERNAL_H_
#define ROBOT_MAIN_PACKET_EXTERNAL_H_
/*
 *date: 2013/03/02
 *author:Renzhenwen
 *机器人与外部节点数据帧封装头文件
 */

#include "RobotBasePacket.h"
#include <cstring>
using namespace std;
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
class RobotPacketExternal : public RobotBasePacket
{
public:
	//帧标志
	typedef enum FrameFlag {Start = 0xfb, End = 0xfe} FrameFlag;					
	//上行（UPLINK）：源（PC104）  ------- 宿（控制台或其他PC104）
	typedef enum PacketType 
	{
		//上行的数据返回帧
		UPLINK_DATA_RETURN = 0xB3,
		//下行的查询帧
		DOWNLINK_QUERY_COMMAND = 0x31,
		//下行的不带参数控制帧
		DOWNLINK_CONTROL_NOARGS_COMMAND = 0x32,
		//下行的带参数控制帧
		DOWNLINK_CONTROL_WITHARGS_COMMAND = 0x33,
		//机器人之间的信息交换
		BETWEEN_ROBOTS_INFO_CHANGE = 0x34
		}PacketType;
	typedef enum ObjectType
	{
		OBJECT_MOTOR = 0x01,
		OBJECT_LIGHT = 0x04,
		OBJECT_VEDIO = 0x03,
		OBJECT_MODE = 0x0C,
		OBJECT_LINKS_FRONT = 0x0D,
		OBJECT_LINKS_BACK = 0x0E,
		OBJECT_COORS = 0x0F,
		OBJECT_ALL_STOP = 0x11
	}ObjectType;
	explicit RobotPacketExternal(unsigned char *buf = NULL, unsigned char t_maxSize = 110,
				unsigned char fs = Start, unsigned char fe  = End , unsigned char dt = UPLINK_DATA_RETURN);
	~RobotPacketExternal(void) throw();
	//如果校验和正确则返回true
	bool verifyCheckSum(void);
	//计算校验和
	unsigned char calCheckSum(void);			
	//完成包封装	
	void finalizePacket(void);					
	//重载的封包器,直接由数组封包
	//按字节封包(封包前务必使实际数据已经压入帧,包封装完成后不能再修改)
	void finalizePacket(const unsigned char* arr, int len){copyFromCharArry(arr, len);}
	//获取数据包实际的负载数据长度，必须在一个完整包情况下应用
	int getValidDataLen(void);	  

	/*以下为设置和获取成员变量的函数*/
	unsigned char getId(void)const {return id;}				
	unsigned char getIdByIndex(void)const {return myBuf[2];}				
	void setIdByIndex(unsigned char value)const {myBuf[2] = value;}				
	void setId(unsigned char t_id) {id = t_id;}	
	unsigned char getRobotNoByIndex(void) const {return myBuf[1];}
	unsigned char getRobotNo(void) const {return robotNo;}
	void setRobotNo(unsigned char t_robotNo) {robotNo = t_robotNo;}
	unsigned char getCommandByIndex(void) const {return myBuf[3];}
	unsigned char getCommand(void) const {return command;}
	void setCommand(unsigned char t_command) {command = t_command;}
	unsigned char getObjectFeatureHighByIndex(void) const {return myBuf[4];}
	unsigned char getObjectFeatureHigh(void) const {return objectFeatureHigh;}
	void setObjectFeatureHigh(unsigned char t_objectFeatureHigh) {objectFeatureHigh= t_objectFeatureHigh;}
	unsigned char getObjectFeatureLowByIndex(void) const {return myBuf[5];}
	unsigned char getObjectFeatureLow(void) const {return objectFeatureLow;}
	void setObjectFeatureLow(unsigned char t_objectFeatureLow) {objectFeatureLow= t_objectFeatureLow;}
	unsigned char getRealDataLenByIndex(void) const {return myBuf[6];}
	unsigned char getRealDataLen(void) const {return realDataLen;}
	void setRealDataLen(unsigned char t_realDataLen) {realDataLen = t_realDataLen;}
	unsigned char getCheckSumByIndex(void) const {return myBuf[myBuf[6] + 7];}
	unsigned char getCheckSum(void) const {return checkSum;}
	void setCheckSum(unsigned char t_checkSum) {checkSum = t_checkSum;}

	unsigned char getHead() const {return myBuf[0];}
	unsigned char getFoot(void) const {return myBuf[myBuf[6] + 8];}
	unsigned char getByteByIndex(int i) const {return myBuf[i];}

protected:
	unsigned char head;  			//帧头	
	unsigned char robotNo;			//机器人编号
	unsigned char id;				//帧编号
	unsigned char command;			//控制命令字
	unsigned char objectFeatureHigh;//目标识别特征
	unsigned char objectFeatureLow;	//目标识别特征
	unsigned char realDataLen;		//实际数据长度
	unsigned char checkSum;			//校验和
	unsigned char tail;				//尾巴
private:
/*将一个数组整体拷贝给帧结构*/
	void copyFromCharArry(const unsigned char *arr, int len)
	{
		if(arr != NULL)
		{
			bcopy(arr, getBuf(), len);
			setLength(len); //设置长度
		}
		else
			throw string("arr is NULL");
	}
};
#ifdef ROBOT_NAMESPACE
}
#endif
#endif
