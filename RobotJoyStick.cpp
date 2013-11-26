/*
 *   远程遥控手柄类（方便调试和控制）
 *  Powered by zhenwenRen
 *  Email: admin@unix8.net
 * */
#include "RobotJoyStick.h"
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
RobotJoyStick::RobotJoyStick(){}
RobotJoyStick::~RobotJoyStick(void) 
{ 
	joyExit();
	delete serviceSocket; 
	serviceSocket = 0; 
}
#if 0
void RobotJoyStick::netSetupPrepare(void)
{
	serviceSocket = new RobotSocket("127.0.0.1",7777, true, RobotSocket::TCP);
	return	(serviceSocket->accept(&clientSocket));
}
#else
void RobotJoyStick::netSetupPrepare(void)
{
		/*最初直接使用上面的注释语句，而且没netSetup函数，这样分两个函数的目的是检测状态，防止手柄网络不通阻塞*/
	serviceSocket = new RobotSocket(true, RobotSocket::TCP);
}
bool RobotJoyStick::netSetup()
{
	return ((serviceSocket->open(7777, RobotSocket::TCP, "127.0.0.1")) && (serviceSocket->accept(&clientSocket)));
	//return true;
}
#endif
void RobotJoyStick::joyExit(void)
{
	serviceSocket->close(); 
	clientSocket.close();
}
#ifdef ROBOT_NAMESPACE
}
#endif
