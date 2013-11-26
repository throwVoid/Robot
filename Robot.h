#pragma once
#ifndef ROBOT_MAIN_ROBOT_H_
#define ROBOT_MAIN_ROBOT_H_
/*
 *
 * Powered by RenZhenwen
 * date:2012-12-07
 * 该文件为机器人类最重要的文件，负责各个模块的组织与信息沟通
 * 包括：网络服务器，多机器人信息交互，路径规划，遥操作，机器人控制等
 * */

#include "RobotSocket.h"
#include "RobotInclude.h"
#include "RobotModeLock.h"
#include "RobotMath.h"
#include "RobotPacket.h"
#include "RobotPacketExternal.h"
#include "robot-config.h"


#include <map>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>

#include <pthread.h>

using namespace std;

#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif

class RobotModeLock;
class RobotWheelTask;
class RobotLightTask;
class RobotPathPlan;
class RobotNetEpoll;
class RobotModeLock;
class RobotActionDesired;
class RobotSocket;
class Mediator;

#ifdef ROBOT_HAVE_LOG
class RobotFile;
class RobotFileStream;
class StreamWriter;
#endif

class RobotSemaphore;

class Robot
{
public:

	//增加分支预测命中率
	#define likely(x) __builtin_expect(!!(x),1)
	#define unlikely(x) __builtin_expect(!!(x),0)


	#if !defined(THROW_ERROR)
		#include <iostream>
		#include <unistd.h>
		#define THROW_MEMORY_ERROR do{ cerr<<RobotException("Fatal Memory Error!").Stakc()<<endl; exit(1);} while(0)
		#define THROW_OTHER_ERROR do{ cerr<<RobotException("Fatal other Error!").Stakc()<<endl; exit(1);} while(0)
	#endif


	#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
						  void operator=(const TypeName&)


	//释放资源
	#define RELEASE_HEAP_MEMORY(x) \
		do{\
				if((x) != NULL)\
				{\
					delete (x);\
					(x) = NULL;\
				}\
			}while(0)

	//主机信息
	class HostInfo
	{
	public:
		int fd;
		int packetCount;
		string robotIp;
		HostInfo(): fd(-1), packetCount(0){}
	};


	/*机器人编号+机器人起始点+机器人名字+机器人描述*/
	explicit Robot(Mediator* media, unsigned char tNo, Point tP, string tName = "small robot", string tDes = "As a router");
	~Robot(void) throw();
	//机器人程序退出资源清理
	void robotResourceClear(void);								
	//阻塞直到机器人退出
	void startAndWaitForExit(void);


	//机器人名字
	const  string& getName(void) const{return robotName;}			
	//设置机器人名字
	void setName(string name) {robotName = name;}
	//机器人描述
	const  string& getDes(void) const{return robotDesc;}				
	//设置机器人描述
	void setDes(string des) {robotDesc = des;}
	//获取机器人编号
	const int getNo(void) const{return robotNo;}
	//设置机器人编号
	void setNo(int tNo) {robotNo = tNo;}



	/*网络相关*/
	//设置机器人服务器建立时绑定的IP地址
	void setIp(const char *tIp, int len);
	//设置服务器端口号
	void setPort(int port);
	//设置控制台IP地址
	void setConsoleIp(const char* tIp, int len);
	//设置前面机器人IP地址
	void setFrontRobotIp(const char* tIp, int len);
	//设置后面机器人IP地址
	void setBackRobotIp(const char* tIp, int len);
	//设置广播服务器UDP端口
	void setBroadcastPort(int port);
	//设置广播服务器广播地址
	void setBroadcastIp(const char* tIp, int len);
	//读取配置文件，并将机器人IP读入map
	void addToSocketMap(int no, string tIp);




	/*获取和设置机器人相关位置信息*/	
	//设置机器人X基坐标
	void setX(float x) {robotPoint.setX(x);}
	//设置机器人Y基坐标							
	void setY(float y) {robotPoint.setY(y);}
	//设置机器人头部朝向(角度)
	void setTh(float th) {robotPoint.setTh(th);}						
	//设置机器人头部朝向弧度表示
	void setThRad(float th) {robotPoint.setThRad(th);}			
	//设置机器人位置
	void setPose(float x, float y, float th = 0) {robotPoint.setPose(x, y, th);}
	//设置机器人位置
	void setPose(Point position) {robotPoint.setPose(position);}								
	//获取机器人X坐标
	float getX(void) const {return robotPoint.getX();}
	//获取机器人Y坐标
	float getY(void) const {return robotPoint.getY();}
	//获取机器人头部朝向
	float getTh(void) const {return robotPoint.getTh();}	
	//获取机器人头部朝向弧度
	float getThRad(void) const {return robotPoint.getThRad();}
	//获取机器人位置
	void  getPose(float *x, float *y, float *th = NULL) const {robotPoint.getPose(x, y, th);}		
	//获取机器人位姿
	const Point& getPose(void) const {return robotPoint;}						
	//求机器人到一点的距离
	float findDistanceTo(Point position) const {return robotPoint.findDistanceTo(position);}
	//求机器人到一点的距离平方值
	float squaredFindDistanceTo(Point position) const {return robotPoint.squaredFindDistanceTo(position);}
	//机器人到一点的角度
	float findAngleTo(Point position) const {return robotPoint.findAngleTo(position);}
	//设置目标
	inline void setGoal(Point&);
	inline void setGoal(float x, float y, float theTa);
	//取消目标
	void cancelGoal(void);



	//获取指定超声波传感器距离
	inline	int getSonarRange (int i) const;
	//获取startI-endI编号范围内超声波传感器的最小距离
	int getClosestSonarRange(int startI, int endI) const;
	int checkRangeDevicesCurrentPolar(int startI, int endI);	




//机器人决策与功能函数
#ifdef JOYSTICK
	//初始化和启动手柄模块
	bool initJoyStick();
	void processJoyStick(void);
#endif
#ifdef TASK_QUEUE_PROCESS
	//机器人以任务优先队列作为数据结构来处理任务
	void processTaskQueue(void);	
#endif
	//初始化日志模块
#ifdef ROBOT_HAVE_LOG
	void initRobotLog(const std::string path, const std::string mode);
	void openRobotLog();
	void setLogPath(string path);
	std::string& getLogPath();
#endif

	//对串口数据的处理
	void processData(const unsigned char*,int);										
	//处理Epoll服务器
	void processNetEpoll(void);	
	void processSensorData();

	//机器人初始化
	//初始化机器人的任务
	bool initTask();
	//初始化TCP网络模块
	bool initTCPServer();
	//初始化UDP网络模块
	bool initUDPServer();
	//初始化客户端模块
	void initClientModule(void);
	//路径规划初始化和启动
	bool initPathPlanModule();

    //机器人编队控制
	//主动连接编队前方机器人
	bool connectFrontRobot(void);
	//主动连接后方机器人
	bool connectBackRobot(void);

private:
	Mediator *mediator;							//中介者


#ifdef  ROBOT_SENSOR_DATA_DELAY_SEND
public:
	//数据摆渡后恢复
	//打开数据文件来保存数据
	bool openSensorDataSave();
	//关闭传感器数据保存文件
	void closeSensorDataFile();
	bool openSensorDataRead();
	void addSensorData(char tempSensor, char monistSensor);
	void sendSensorData();
	void emptyData();

public:
	//网络机器人在线检测处理
	void keepAliveCheck();
private:
	//传感器文件保存路径
	std::string dataFilePath;
	//文件流
	fstream fs;
	//文件是否打开的标志
	bool fsIsUseToWrite;
	//用来保存温度适度传感器数据
	char writeData[2];
	RobotPacketExternal heatPacket;
	RobotPacketExternal tempAndMonistPacket;
	RobotPacketExternal tempAndMonistPacketSave;
	int minAck;
	int maxAck;
	const int minInterval;
	//机器人ACK
	static unsigned char ACK;
#endif


private:
	DISALLOW_COPY_AND_ASSIGN(Robot);

	//日志相关
#ifdef ROBOT_HAVE_LOG
	RobotFile *logFile;
	RobotFileStream *logFileStream;
	StreamWriter *logFileWriter;
	std::string log;
	std::string logPath;
#endif
	

	//机器人数据传输帧
	//用来接受机器人发送的数据帧的结构
	RobotPacket robotPacketInternal;
	//用来接受控制台或者其他机器人帧的结构
	RobotPacketExternal robotPacketExternal;


	//机器人属性信息
	unsigned char robotNo;					//编号
	Point robotPoint;								//机器人位置信息
	Point oldRobotPoint;						//机器人旧的位置信息
	Point robotGoalPoint;
	string robotName;								//名字
	string robotDesc;								//描述	

	//机器人网络属性信息
	//机器人服务器IP
	char robotIp[20];							
	//机器人服务器端口
	int robotPort;

	//连接到机器人的远程控制台IP
	char consoleIp[20];

	//编队时前方机器人IP地址
	char frontRobotIp[20];
	//编队时后方机器人IP地址
	char backRobotIp[20];

	//机器人UDP服务器广播地址
	char robotBroadcastIp[20];
	//机器人UDP广播服务器端口
	int robotBroadcastPort;

	//Epoll使用的缓冲区和长度
	#define SOCKET_BUFF_SIZE 1024
	unsigned char socketBuf[SOCKET_BUFF_SIZE + 1];
	int bufLen;

	//网络服务器
	RobotNetEpoll *netEpoll;

	//服务器监听套接字
	RobotSocket *listenSocket;						

	//控制台套接字
	RobotSocket *consoleSocket;
	RobotSocket *UDPBroadcastSocket;
	struct sockaddr_in broadcastSockaddr;


	//检测是否时第一次数据（里程机与电子罗盘）
	bool isFirstFrame;
	//失败时重试次数
	int failureTryCnt;

	//速度信息
	#define SPEED_NUM 31
	static const short speed[SPEED_NUM + 1];
	//static const char direct[SPEED_NUM];

	/*
	//串口收到的数据缓冲区
	#define SERIAL_BUFF_SIZE 30
	unsigned char serialData[SERIAL_BUFF_SIZE];
	*/

	//超声波信息
	#define SONAR_NUM  12
	//记录超声波传感器的数据
	int disObs[SONAR_NUM];

	//机器人里程计测量的数据		
	long oldTotalDist;

	//机器人电子罗盘相关数据
	float oldDeltaAngle;

	//机器人任务
	RobotWheelTask *robotWheelTask;
	RobotLightTask *robotLightTask;



	//机器人路径规划类
	RobotPathPlan *robotPathPlan;

	//返回的轮子速度和角度控制，设为类的成员的目的是防止频繁的new和delete导致碎片
	RobotActionDesired *wheelAction;


	//编队相关
	//连接到前后机器人的套接字
	RobotSocket *frontRobotSocket;
	RobotSocket *backRobotSocket;

	//前后机器人的位置
	Point frontRobotPoint;
	Point backRobotPoint;

	//前后机器人的控制命令
	unsigned char frontRobotCtlWord;
	unsigned char backRobotCtlWord;

	//连接到该服务器后的accept套接字
	int frontRobotFd;
	int backRobotFd; 

	map<int,string> socketMap;


	//以下成员用于定位相关
	short newTotalDist ;
	float newAngle;
	float newDeltaAngle;
	float newDeltaS;
	short deltaDist; 
	float deltaAngle;
	float deltaRad;
	float calXBySensorMerge; 
	float calYBySensorMerge; 
	float calAngleBySensorMerge; 
	float calXByMotionModel; 
	float calYByMotionModel; 
	float calAngleByMotionModel;
	float coefficientSensorMeger;
	float coefficientMotionModel;


	//手柄操作
#ifdef JOYSTICK
	RobotJoyStick *joyStick;
#endif

	//手柄和智能模式切换标志类
	RobotModeLock modeLock;								

	int semaphoreFlag;
	RobotSemaphore *semaPhore;

private:
	//建立epoll服务器
	bool setUpSerivceListen(const char* ip, int port, RobotSocket::NetType socketType = RobotSocket::TCP);
	//建立UDP广播服务器
	bool setUpUDPBroadcast(const char* ip, int port);


	//机器人停止
	void stop(void);										

	//判断机器人左右轮子速度，根据方向和油门
	inline short judgeLeftSpeed(unsigned char s, unsigned char d);
	inline short judgeRightSpeed(unsigned char s, unsigned char d);


  //网络数据帧处理
	//获取帧地址
	unsigned char* getInternalPacketBuf() {return robotPacketInternal.getBuf();}
	//将网络数据包进行组帧
	void  copyAsFrame(const unsigned char* buf, int len){robotPacketInternal.finalizePacket(buf, len);}


private:
	int frontRobotNo;///<前方机器人编号
	int backRobotNo;///<后方机器人编号

	bool consoleIsConnected;///<控制台连接状态
	bool haveFrontRobot;///<编队中是否有前方机器人状态
	bool frontRobotIsConnected;///<编队时是否已经连接到前方机器人
	bool haveBackRobot;///<编队中是否有后方机器人状态
	bool backRobotIsConnected;///<编队时是否已经连接到后方机器人
	bool robotIsRuning;///<机器人主循环已经启动
	bool robotDriverIsReady;///<机器人底层驱动是否已经准备好

	///<backRobotIsConnected与backRobotIsAccept之间的区别
	///<backRobotIsConnected为本机器人连接后方机器人建立的连接状态
	///<backRobotIsAccept为后方机器人连接到本机器人并被accept后的状态
	//
	//
	bool frontRobotIsAccepted;///<如果机器人前方有机器人，本机器人是否已经accept他的请求
	bool backRobotIsAccepted;///<

	bool TCPServerIsBuilded; ///<TCP服务器是否创建成功
	bool UDPServerIsBuilded;///<UDP服务器是否创建成功
public:
	void setConsoleIsConnected(bool);
	bool getConsoleIsConnected(void);
	void setHaveFrontRobot(bool);	
	bool getHaveFrontRobot();	
	void setHaveBackRobot(bool);	
	bool getHaveBackRobot();	
	void setFrontRobotIsConnected(bool);	
	bool getFrontRobotIsConnected();	
	void setBackRobotIsConnected(bool);	
	bool getBackRobotIsConnected();	
	void setRobotIsRuning(bool);	
	bool getRobotIsRuning();	
	void setRobotDriverIsReady(bool);	
	bool getRobotDriverIsReady();	
	void setFrontRobotIsAccepted(bool);
	bool getFrontRobotIsAccepted(void);
	void setBackRobotIsAccepted(bool);
	bool getBackRobotIsAccepted(void);
	void setTCPServerIsBuilded(bool);
	bool getTCPServerIsBuilded();
	void setUDPServerIsBuilded(bool);
	bool getUDPServerIsBuilded();

};


//获取第I个超声波的距离,大于安全距离则为10米
inline int Robot::getSonarRange(int i) const {return disObs[i];}  

//机器人轮子速度裁决
inline short Robot::judgeLeftSpeed(unsigned char s, unsigned char d) 
{
	if((d == 13) || (d == 14) || (d == 15) || (d == 16) || (d == 17)) return speed[s];
	else
		return (/*speed[s] * */ 20 *((d < 15) ? 1: -1));
}

inline short Robot::judgeRightSpeed(unsigned char s, unsigned char d) 
{
	if((d == 13) || (d == 14) || (d == 15) || (d == 16) || (d == 17)) return speed[s];
	else 
		return (/*speed[s] * */20 * ((d < 15) ? -1: 1));
}


#ifdef ROBOT_NAMESPACE
}
#endif


#endif //ROBOT_MAIN_ROBOT_H_
