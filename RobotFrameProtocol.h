#pragma once 
#ifndef ROBOT_MAIN_FRAMEPROTOCOL_H_
#define ROBOT_MAIN_FRAMEPROTOCOL_H_
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
class FrameProtocol
{
	typedef enum RobotType {Task = 0x01, Router = 0x02, Trans = 0x04} RobotType; 
	typedef enum RobotFrameSign  //帧的标志 帧头+帧尾
	{
		FrameHead = 0xFB, 		//帧头
		FrameTail = 0xFE
	}RobotFrameSign;

};
#ifdef ROBOT_NAMESPACE
}
#endif
#endif
