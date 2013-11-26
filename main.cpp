#include <signal.h>
#include <string>
#include "Robot.h"
#include "RobotTask.h"
#include "RobotMath.h"
#include "RobotInclude.h"
#include "RobotThread.h"
#include "RobotSerial.h"
#include "RobotMediator.h"
#include "RobotException.h"
#include "./Config/RobotConfig.h"
#include "robot-config.h"
using namespace std;
/*
 *The project is designed by zhenwenRen
 *Use the editer "Vim"
 *Use the complier "G++" with "gnu++0x"
 *Use the debugger "gdb", "gdbtui" and "cgdb"
 *Email: admin@unix8.net
 *QQ:47923517
 * */				
int main(int argc, char **argv)
{
	if(argc != 3)
	{
		PRINT("指定参数不全([robot+IP+PORT] [robot+IP地址+端口号])\n机器人将读取配置文件作为参数\n");
	}
	signal(SIGINT, signalFunc);
	//初始化线程管理器
	//RobotThread::initThreads();
	//const char ConfigFile[]= "/root/Main2013/Config/config.txt";
	const char ConfigFile[]= "./Config/config.txt";

	string arg;

	//加载配置文件
	RobotConfig* configSettings = new RobotConfig(ConfigFile);


	//中介者
	Mediator mediator;

	//定义机器人模块
	Robot robot(&mediator, 1, Point(configSettings->Read("robotX",0),configSettings->Read("robotY",0),0));

	//定义串口模块
	RobotSerial robotSerial(&mediator);

	//在中介者中注册机器人与串口模块
	mediator.registRobot(&robot);
	mediator.registRobotSerial(&robotSerial);



	#ifdef ROBOT_HAVE_LOG
	//设置日志目录
	arg = "log";
	arg = configSettings->Read("logpath", arg);
	robot.setLogPath(arg);
	robot.openRobotLog();
	#endif

	//串口模块初始化
	arg = "/etc/ttyS0";
	arg = configSettings->Read("serialName", arg);

	//配置串口名与数据帧长度
	robotSerial.setSerial(arg.c_str());
	robotSerial.setFrame1Len(configSettings->Read("serialDataLen1", 0));
	robotSerial.setFrame2Len(configSettings->Read("serialDataLen2", 0));

	//配置机器人编号
	robot.setNo(configSettings->Read("robotNo",0));

	//配置机器人服务器IP与端口
	if(argc == 3)
	{
		robot.setIp(argv[1],strlen(argv[1]));
		robot.setPort(atoi(argv[2]));
	}
	else
	{
		//设置TCP服务器IP地址
		arg = configSettings->Read("ipAddress",arg);
		robot.setIp(arg.c_str(), strlen(arg.c_str()));
		//设置TCP服务器端口
		robot.setPort(configSettings->Read("port", 0));

	#if 0
		//设置UDP广播服务器IP地址
		arg = configSettings->Read("broadcastAddr",arg);
		robot.setBroadcastIp(arg.c_str(), strlen(arg.c_str()));
		//设置UDP服务器端口
		robot.setBroadcastPort(configSettings->Read("broadcastPort", 0));
	#endif
	}

	//配置控制台IP地址
	arg = configSettings->Read("consoleIpAddress",arg);
	robot.setConsoleIp(arg.c_str(), strlen(arg.c_str()));

	
	//配置机器人队伍
	arg = "null";
	arg = configSettings->Read("Robot1", arg);
	if(arg != "null")
		robot.addToSocketMap(1, arg.c_str());

	arg = "null";
	arg = configSettings->Read("Robot2", arg);
	if(arg != "null")
		robot.addToSocketMap(2, arg.c_str());

	arg = "null";
	arg = configSettings->Read("Robot3", arg);
	if(arg != "null")
		robot.addToSocketMap(3, arg.c_str());

	arg = configSettings->Read("Robot4", arg);
	if(arg != "null")
		robot.addToSocketMap(4, arg.c_str());

	arg = configSettings->Read("Robot5", arg);
	if(arg != "null")
		robot.addToSocketMap(5, arg.c_str());

	arg = configSettings->Read("Robot6", arg);
	if(arg != "null")
		robot.addToSocketMap(6, arg.c_str());


	//机器人初始化任务
	robot.initTask();

	//初始化路径规划模块
	robot.initPathPlanModule();

	robot.initClientModule();

	//机器人初始化TCP模块
	if(!(robot.initTCPServer()))
	{
		robot.robotResourceClear();
		std::cout<<RobotException("服务器建立失败").Stack()<<std::endl;
		exit(1);
	}
#if 0								
	//机器人初始化UDP模块
	if(!robot.initUDPServer())
	{
		
	}
#endif
  /*初始化完毕，启动工作线程*/

	//启动epoll线程
	//RobotFunctorC<Robot> netEpollFunctor(robot, &Robot::processNetEpoll);
	//RobotThread netEpollThread(&netEpollFunctor, true);
	
	#ifdef ROBOT_SENSOR_DATA_DELAY_SEND
	//传感器数据检测
	RobotFunctorC<Robot> processSensorDataFunctor(robot, &Robot::processSensorData);
	RobotThread processSensorDataThread(&processSensorDataFunctor, true);
	#endif
	if(robotSerial.openSerial() == RobotSerial::STATUS_OPEN_FAILED) 
	{
		delete configSettings;
		std::cout<<RobotException("打开串口失败").Stack()<<std::endl;
		exit(1);
	}
	//准备好串口
	robotSerial.sendSerialIsReady();
	//等待机器人准备完成
	robotSerial.waitReadyACK();
	//启动串口读模块
	RobotFunctorC<RobotSerial> serialWriteFunctor(robotSerial, &RobotSerial::_write_rs232);
	RobotThread serialWriteThread(&serialWriteFunctor, true);
	//启动串口写模块
	RobotFunctorC<RobotSerial> serialReadFunctor(robotSerial, &RobotSerial::_read_rs232);
	RobotThread serialReadThread(&serialReadFunctor, true);

	#ifdef JOYSTICK
	//手柄模块
	//机器人初始化手柄控制模块
	robot.initJoyStick();
	RobotFunctorC<Robot> netJoyStickFunctor(robot, &Robot::processJoyStick);
	RobotThread joyStickThread(&netJoyStickFunctor, true);
	#endif
	#ifdef TASK_QUEUE_PROCESS
	//任务队列处理模块
	RobotFunctorC<Robot> taskFunctor(robot, &Robot::processTaskQueue);
	RobotThread taskThread(&taskFunctor, true);
	#endif
	#ifdef  ROBOT_SENSOR_DATA_DELAY_SEND
	//启动机器人在线检测线程
	RobotFunctorC<Robot> keepAliveFunctor(robot, &Robot::keepAliveCheck);
	RobotThread keepAliveThread(&keepAliveFunctor, true);
	#endif
	//卸载配置文件读取模块
	delete configSettings;
	robot.startAndWaitForExit();
	return 0;
}

