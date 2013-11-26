#pragma once
#ifndef CONFIG_DEFINE
#define CONFIG_DEFINE

/*
 * power by zhenwenRen
 * Date:2013-01-22
 * */

#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif

#define LINUX

//发送给机器人的控制命令带有时间长度
//#define ROBOT_RUNTIME

//发送给机器人的控制命令带有编号
//#deifne PACKET_HAVE_ID
//

//#define JOYSTICK
//#define ROBOT_HAVE_LOG
//#define ROBOT_SENSOR_DATA_DELAY_SEND
#ifdef ROBOT_NAMESPACE
}
#endif
#endif
