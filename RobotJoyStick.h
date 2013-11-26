#pragma once
#ifndef ROBOT_MAIN_JOYSTICK_H_
#define ROBOT_MAIN_JOYSTICK_H_
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <termios.h>
#include <stdlib.h>
#include <fcntl.h>

#include "RobotSocket.h"
/*
 *   远程遥控手柄类（方便调试和控制）
 *  Power by zhenwenRen
 *  Email: admin@unix8.net
 * */
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
class RobotJoyStick
{
public:
	/*手柄模式  +  非模式切换而是真实的按键控制（有可能智能模式时也按键，需要屏蔽此类） +  出错*/
	typedef enum JoyStickReceiveStatus {JoyStickMode = 0x01, RealData = 0x02, Error = 0x04} JoyStickReceiveStatus;

	explicit RobotJoyStick(); 
	~RobotJoyStick(void);
	bool netSetup(void); 
	/*
	 *这里将网络初始化和网络建立放在不同的函数里是因为：
	 * 如果网络连接不成功，则可能会一直处于手柄模式，而使智能模式得不到运行
	 * */
	//网络建立准备
	void netSetupPrepare(void);   				
	//接收网络消息
	inline int receiveMessage(unsigned char* buf, int len);									
	//关闭套接字，释放对象	
	void joyExit(void);									
private:		
	//机器人的监听套接字	
	RobotSocket *serviceSocket;					
	//管理客户端套接字连接
	RobotSocket clientSocket;																			
};
inline int RobotJoyStick::receiveMessage(unsigned char* buf, int len)
{
	return clientSocket.read(buf, len);
}
#ifdef ROBOT_NAMESPACE
}
#endif
#endif//ROBOT_MAIN_JOYSTICK_H_
