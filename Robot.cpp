/*
 * Powered by zhenwenRen
 * Date:2012-12-07  - 2013-01-23
 * Email:admin@unix8.net
 * */

#include "Robot.h"
#include "RobotNetEpoll.h"
#include "RobotPacketExternal.h"
#include "RobotMediator.h"
#include "RobotTask.h"
#include "RobotWheelTask.h"
#include "RobotLightTask.h"
#include "RobotException.h"
#include "RobotFile.h"
#include "RobotFileStream.h"
#include "StreamWriter.h"
#include "RobotSleep.h"
#ifdef JOYSTICK
	#include "RobotJoyStick.h"
#endif
#ifdef TASK_QUEUE_PROCESS
	#include "RobotDataQueue.h"
#endif

#include "RobotSemaphore.h"



#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif



#ifdef  ROBOT_SENSOR_DATA_DELAY_SEND
//用于统计包的数量
unsigned char Robot::ACK = 1;
#endif

//速度映射表
const short Robot::speed[SPEED_NUM + 1] = {
	 0,200,200,200,
	 150,150,150,
	 100,100,100,
	 50,50,50,
	 20,20,20,0,
	-20,-20,-20,
	-50,-50,-50,
	-100,-100,-100,
	-150,-150,-150,
	-200,-200,-200
}; 


//机器人编号与IP地址映射表
void Robot::addToSocketMap(int no, string tIp)
{
	//查找是否已经存在该机器人ip信息
	map<int, string>::iterator iter = socketMap.find(no);

	if(iter == socketMap.end())	
		socketMap[no] = tIp;
	else
		iter->second = tIp;
	//PRINT("增加%d-%s",no,tIp.c_str());
}

//机器人关闭前资源释放
void Robot::robotResourceClear(void)
{
 	RELEASE_HEAP_MEMORY(robotWheelTask);
 	RELEASE_HEAP_MEMORY(robotLightTask);
 	RELEASE_HEAP_MEMORY(netEpoll);
 	RELEASE_HEAP_MEMORY(robotPathPlan);
 	RELEASE_HEAP_MEMORY(wheelAction);
 	RELEASE_HEAP_MEMORY(listenSocket);
 	RELEASE_HEAP_MEMORY(consoleSocket);
 	RELEASE_HEAP_MEMORY(frontRobotSocket);
 	RELEASE_HEAP_MEMORY(backRobotSocket);

#ifdef JOYSTICK
 	RELEASE_HEAP_MEMORY(joyStick);
#endif

#ifdef ROBOT_HAVE_LOG
 	RELEASE_HEAP_MEMORY(logFile);
 	RELEASE_HEAP_MEMORY(logFileStream);
 	RELEASE_HEAP_MEMORY(logFileWriter);
#endif
 	RELEASE_HEAP_MEMORY(semaPhore);
}


Robot::Robot(Mediator* media, unsigned char tNo, Point tP, string tName, string tDes )
:mediator(media)
#ifdef  ROBOT_SENSOR_DATA_DELAY_SEND
,heatPacket(NULL, 20, RobotPacketExternal::Start,RobotPacketExternal::End,RobotPacketExternal::UPLINK_DATA_RETURN)
,tempAndMonistPacket(NULL, 20, RobotPacketExternal::Start,RobotPacketExternal::End,RobotPacketExternal::UPLINK_DATA_RETURN)
,tempAndMonistPacketSave(NULL, 20, RobotPacketExternal::Start,RobotPacketExternal::End,RobotPacketExternal::UPLINK_DATA_RETURN)
,minInterval(2)
#endif
,robotPacketInternal()
,robotPacketExternal()
,robotNo(tNo)
,robotPoint(tP)
,robotName(tName)
,robotDesc(tDes)
,modeLock()
,consoleIsConnected(false)
,haveFrontRobot(false)
,frontRobotIsConnected(false)
,haveBackRobot(false)
,backRobotIsConnected(false)
,robotIsRuning(false)
,robotDriverIsReady(false)
,frontRobotIsAccepted(false)
,backRobotIsAccepted(false)
,TCPServerIsBuilded(false)
,UDPServerIsBuilded(false)
{  
	consoleSocket  = NULL;
	robotWheelTask = NULL;
	robotLightTask = NULL;
	netEpoll = NULL;
	robotPathPlan = NULL;
	listenSocket = NULL;
	frontRobotSocket = NULL;
	backRobotSocket = NULL;

#ifdef JOYSTICK
	joyStick = NULL;
#endif

#ifdef ROBOT_HAVE_LOG
	logFile = NULL;
	logFileStream = NULL;
	logFileWriter = NULL;
#endif

	//轮子控制动作
	wheelAction = new RobotActionDesired(0,0);

	//用于激活编队网络连接的信号灯
	semaPhore = new RobotSemaphore(0);

	oldTotalDist = 0;
	//机器人位姿ity,int stopbits) 

	oldRobotPoint = robotPoint;

	//测试是否是第一个电子罗盘与里程数据帧
	isFirstFrame = true;

	//建立服务器失败时重试次数
	failureTryCnt = 3;

	//加权滤波系数
	coefficientSensorMeger = 0.0;
	coefficientMotionModel = 1.0;

	//前后机器人连接到本地服务器的文件描述符
	frontRobotFd = -1;
	backRobotFd = -1;
	disObs[3] = disObs[9] = 100;

	//设置机器人程序启动
	setRobotIsRuning(true);

	//初始化为遥控操作模式
	modeLock.setMode(RobotModeLock::MODE_JOYSTICK);
	//modeLock.setMode(RobotModeLock::MODE_WANDER);
	
#ifdef  ROBOT_SENSOR_DATA_DELAY_SEND
	dataFilePath = "./SensorData";

	//对传感器数据帧的处理
	tempAndMonistPacketSave.setRobotNo(3);
	tempAndMonistPacketSave.setId(3);
	tempAndMonistPacketSave.setObjectFeatureHigh(0x08);
	tempAndMonistPacketSave.setObjectFeatureLow(0x00);
	tempAndMonistPacketSave.setRealDataLen(0x02);
	//对传感器数据帧的处理
	tempAndMonistPacket.setRobotNo(3);
	tempAndMonistPacket.setId(3);
	tempAndMonistPacket.setObjectFeatureHigh(0x08);
	tempAndMonistPacket.setObjectFeatureLow(0x01);
	tempAndMonistPacket.setRealDataLen(0x02);

	heatPacket.setRobotNo(3);
	heatPacket.setId(ACK);
	heatPacket.setObjectFeatureHigh(0x09);
	heatPacket.setObjectFeatureLow(0x01);
	heatPacket.setRealDataLen(0x02);
	minAck = maxAck = ACK;
	//信号灯标志，0为传感器数据发送标志，1为机器人前后网络连接标志,-1为断线标志
	semaphoreFlag = -1;
	semaPhore->Post();
	//软件启动后，上位机还没有连接，这时候应该记录传感器数据
#endif
}



//日志记录初始化
#ifdef ROBOT_HAVE_LOG
void Robot::initRobotLog(const std::string path, const std::string mode)
{
	logFile = new RobotFile(path, mode);
	logFileStream = new RobotFileStream(*logFile);
	logFileWriter = new StreamWriter(*logFileStream);
}

void Robot::openRobotLog()
{
	initRobotLog(logPath, "w+");
}
#endif


//停止机器人
void Robot::stop(void)
{
#ifndef TASK_QUEUE_PROCESS
	robotWheelTask->setWheel(0, 0);
	robotWheelTask->doTask();
	mediator->send(robotWheelTask->getTaskBuf(), robotWheelTask->getTaskBufLen());
#else
	//串口模块指针+左轮速度+右轮速度+优先级
	RobotTask * at = new RobotWheelTask(0, 0, RobotTask::Common);
	RobotTaskPriorityQueue::getInstance()->taskPriorityQueue.pushTask(at);
	RobotTaskPriorityQueue::getInstance()->taskPriorityQueue.runTopTask();
#endif
}


//处理网络数据
void Robot::processNetEpoll(void)
{
	//接受Epoll返回的状态值
	RobotNetEpoll::EpollDataStatus epollStatus; 

	//左轮速度，右轮速度
	int leftSpeed, rightSpeed;
	//nenents：当前触发的事件数，s为开始(Start)/e为结束(End)/b为基(Base)
	int nevents, i, s, e, b;
	int fd;
	while(true)
	{
		//阻塞接收可用事件数
		nevents = netEpoll->handleEvents();

		//对每一个事件进行处理
		for(i = 0; i < nevents; ++i)
		{
			b = 0;
			//初始化缓冲区长度
			bufLen = SOCKET_BUFF_SIZE;
			//处理事件，并将收到的数据拷贝到socketBuf中。需要关注缓冲区溢出问题
			//这里的fd都为值结果参数，bufLen是以指针形式传入
			//------------------
			//epollStatus返回事件类型,socketBuf返回数据首地址，bufLen返回数据长度， fd返回当前事件来源描述符
			//------------------
			epollStatus = netEpoll->DispactchEvent(i, socketBuf, &bufLen, fd);

			//如果值结果参数为初始化长度，则认为数据过多，溢出处理
			if(unlikely(bufLen == SOCKET_BUFF_SIZE)) continue;

			switch(epollStatus)
			{ //有数据返回处理

				case RobotNetEpoll::EpollSuccessWithData://{
				//使用while，是为了预防一次性接受到多组数据帧
				while(1) 
				{
					//检索完整的数据帧
					s = b; 
					while((s < bufLen) && (socketBuf[s] != RobotPacket::Start)) {++s;}
					if(s >= bufLen)
						break;
					e = s + 1;
					while((s < bufLen) && (socketBuf[e] != RobotPacket::End)) {++e;}
					if(e >= bufLen)
						break;
					b = e + 1;

					//对帧的完整性进行初选,依据是长度
					if((*(socketBuf + s + 6) + 8)  !=  (e-s)) 
						continue;

					//对于完整的帧进行封装
					robotPacketExternal.finalizePacket(socketBuf + s, e - s + 1);

					#ifdef PACKET_HAVE_SUM_VERIFY
					//对帧的完整性进行校验，依据是校验码
					if(!robotPacketExternal.verifyCheckSum())
						continue;
					#endif
					
					//robotPacketExternal.printHex();

					//获取帧的属性
					switch(robotPacketExternal.getCommandByIndex())
					{
						
#ifdef  ROBOT_SENSOR_DATA_DELAY_SEND
						case RobotPacketExternal::UPLINK_DATA_RETURN://0xB3
						{
							minAck = robotPacketExternal.getIdByIndex();
							PRINT("收到ACK编号为%d",minAck);
						}
						break;
#endif
						//数据帧为带参数的控制帧
						case RobotPacketExternal::DOWNLINK_CONTROL_WITHARGS_COMMAND://0x33
						{
							//控制目标为灯
							if(unlikely(RobotPacketExternal::OBJECT_LIGHT == robotPacketExternal.getByteByIndex(4)))//0x04
							{	
								//灯关
								if(0x03 == robotPacketExternal.getByteByIndex(7))
								{
									robotLightTask->setLight(0, 0x00);
									robotLightTask->doTask();
									mediator->send(robotLightTask->getTaskBuf(), robotLightTask->getTaskBufLen());
								}
								//灯开
								else if(0x02 == robotPacketExternal.getByteByIndex(7))
								{
									robotLightTask->setLight(0, 0x01);
									robotLightTask->doTask();
									mediator->send(robotLightTask->getTaskBuf(), robotLightTask->getTaskBufLen());
								}
								break;
								
							}

							//<模式切换>
							else if(unlikely(RobotPacketExternal::OBJECT_MODE == robotPacketExternal.getByteByIndex(4)))//0x03
							{
								//modeLock.setMode(!modeLock.getMode());			
								//
								//上一个状态为手柄模式,转换为智能避障模式，否则转换为手柄模式
								if(modeLock.getMode() == RobotModeLock::MODE_JOYSTICK) 
								{
									mediator->serialResume();
									modeLock.setMode(RobotModeLock::MODE_WANDER);
								}
								else
								{
									mediator->serialSuspend();
									modeLock.setMode(RobotModeLock::MODE_JOYSTICK);
								}

								PRINT("《《《《《《《收到模式切换命令，切换为到[%s]》》》》》》\n",
											(modeLock.getMode() == RobotModeLock::MODE_JOYSTICK) ? "手柄模式": "智能模式");

#ifdef ROBOT_HAVE_LOG
#endif
								//robotPoint.setPose(0,0,0);
								//oldRobotPoint.setPose(0, 0, 0);
								break;
							}
							//<电机控制>
							else if(likely(RobotPacketExternal::OBJECT_MOTOR == robotPacketExternal.getByteByIndex(4)))//0x01
							{
								if(modeLock.getMode() == RobotModeLock::MODE_JOYSTICK)
								{
									leftSpeed = judgeLeftSpeed(
										robotPacketExternal.getByteByIndex(7),
										robotPacketExternal.getByteByIndex(8));
									rightSpeed = judgeRightSpeed(
										robotPacketExternal.getByteByIndex(7), 
										robotPacketExternal.getByteByIndex(8));

					/*
									if(leftSpeed > 0 && rightSpeed > 0)//前进
										if(disObs[3] <= 70) break;
									if(leftSpeed < 0 && rightSpeed < 0)//后退
										if(disObs[9] <= 70) break;*/
									PRINT("1发送给机器人的控制速度:左轮[%hd]-右轮[%hd]\n",leftSpeed, rightSpeed);

#ifdef ROBOT_HAVE_LOG
#endif

									robotWheelTask->setWheel(leftSpeed, rightSpeed);
									robotWheelTask->doTask();
									mediator->send(robotWheelTask->getTaskBuf(), robotWheelTask->getTaskBufLen());
								}
							}
							//<上位机设置机器人目标坐标点>
							else if(unlikely(RobotPacketExternal::OBJECT_COORS== robotPacketExternal.getByteByIndex(4)))//0x013
							{
								int tx = RobotMath::charToSignedShort(
									robotPacketExternal.getByteByIndex(7), 
									robotPacketExternal.getByteByIndex(8));
								int ty = RobotMath::charToSignedShort(
									robotPacketExternal.getByteByIndex(9), 
									robotPacketExternal.getByteByIndex(10));

								PRINT("设置机器人坐标为%d,%d",tx,ty);

								setGoal(tx, ty, 0);
							}
							//<收到前面机器人存在信息>
							else if(unlikely(RobotPacketExternal::OBJECT_LINKS_FRONT== robotPacketExternal.getByteByIndex(4)))//0x0D
							{
								//robotPacketExternal.printHex();
								//前面机器人编号
								frontRobotNo  = robotPacketExternal.getByteByIndex(5);

								map<int, string>::iterator iter = socketMap.find(frontRobotNo);
								if(iter != socketMap.end())
								{
									setHaveFrontRobot(true);
									//如前边的机器人已经连接
									if(!getFrontRobotIsConnected())
									{
										setFrontRobotIp((iter->second).c_str(), strlen((iter->second).c_str()));
										semaphoreFlag = 1;
										//将信号灯挂出
										semaPhore->Post();
									}
									else
									{
										//如果新设置的前方机器人IP地为新的IP地址
										if(strcmp(frontRobotIp, (iter->second).c_str()) != 0)
										{
											frontRobotSocket->close();
											RELEASE_HEAP_MEMORY(frontRobotSocket);
											frontRobotSocket = new RobotSocket();

											setFrontRobotIp((iter->second).c_str(), strlen((iter->second).c_str()));
											semaphoreFlag = 1;
											//将信号灯挂出
											semaPhore->Post();
										}
									}
								}
							}
							//<收到后面机器人存在信息>
							else if(unlikely(RobotPacketExternal::OBJECT_LINKS_BACK== robotPacketExternal.getByteByIndex(4)))//0x0E
							{
								//robotPacketExternal.printHex();
								//后面机器人编号
								backRobotNo  = robotPacketExternal.getByteByIndex(5);

								map<int, string>::iterator iter = socketMap.find(backRobotNo);
								if(iter != socketMap.end())
								{
									//setBackRobotIp(socketMap[backRobotNo].c_str(), sizeof(socketMap[backRobotNo].c_str()));
									//PRINT("%s",(iter->second).c_str());
									setHaveBackRobot(true);

									if(!getBackRobotIsConnected())
									{
										setBackRobotIp((iter->second).c_str(), strlen((iter->second).c_str()));
										semaphoreFlag = 1;
										semaPhore->Post();
									}
									else
									{
										//如果新设置的后方机器人IP地位新的IP地址
										if(strcmp(backRobotIp,(iter->second).c_str()) != 0)
										{
											backRobotSocket->close();
											RELEASE_HEAP_MEMORY(backRobotSocket);
											backRobotSocket = new RobotSocket();
											setBackRobotIp((iter->second).c_str(), strlen((iter->second).c_str()));
											semaphoreFlag = 1;
											//将信号灯挂出
											semaPhore->Post();
										}// for if
									}// for else
								}//for if(iter)
							}
							else if(unlikely(RobotPacketExternal::OBJECT_ALL_STOP== robotPacketExternal.getByteByIndex(4)))//0x0E
							{
									modeLock.setMode(RobotModeLock::MODE_JOYSTICK);
							}
							break;
						}//end for " case RobotPacketExternal::DOWNLINK_CONTROL_WITHARGS_COMMAND"
						break;



						//机器人之间的信息交换
						case RobotPacketExternal::BETWEEN_ROBOTS_INFO_CHANGE:
						{
							if(fd == frontRobotFd)
							{
								frontRobotCtlWord = RobotMath::charToShort(robotPacketExternal.getByteByIndex(4),
										robotPacketExternal.getByteByIndex(5));

								//收到编队前方机器人信息
								frontRobotPoint.setX(
									RobotMath::charToSignedShort(robotPacketExternal.getByteByIndex(7),
									robotPacketExternal.getByteByIndex(8)));
								frontRobotPoint.setY(
									RobotMath::charToSignedShort(robotPacketExternal.getByteByIndex(9),
									robotPacketExternal.getByteByIndex(10)));
								frontRobotPoint.setTh(
									RobotMath::charToSignedShort(robotPacketExternal.getByteByIndex(11),
									robotPacketExternal.getByteByIndex(12)));

								//设置路径规划目标
								setGoal(frontRobotPoint);
							}
							else if(fd == backRobotFd)
							{
								backRobotCtlWord = RobotMath::charToShort(robotPacketExternal.getByteByIndex(4),
										robotPacketExternal.getByteByIndex(5));

								//收到编队后方机器人信息
								backRobotPoint.setX(
									RobotMath::charToSignedShort(robotPacketExternal.getByteByIndex(7),
									robotPacketExternal.getByteByIndex(8)));
								backRobotPoint.setY(
									RobotMath::charToSignedShort(robotPacketExternal.getByteByIndex(9),
									robotPacketExternal.getByteByIndex(10)));
								backRobotPoint.setTh(
									RobotMath::charToSignedShort(robotPacketExternal.getByteByIndex(11),
									robotPacketExternal.getByteByIndex(12)));

							}
							else
								::close(fd);
						}// end for "case RobotPacketExternal::BETWEEN_ROBOTS_INFO_CHANGE"
						break;

						case RobotPacketExternal::DOWNLINK_QUERY_COMMAND: break;
						case RobotPacketExternal::DOWNLINK_CONTROL_NOARGS_COMMAND:break;
						default:break;
					}//end for " switch(robotPacketExternal.getCommandByIndex())"
				}//end for " while(1) "
				break;

				case RobotNetEpoll::EpollAccept:
				{
					robotPacketExternal.finalizePacket(socketBuf, bufLen);

					char tmp[20];
					sprintf(tmp,"%s", robotPacketExternal.getBuf());

					PRINT("当前IP地址为[%s]的主机连接到该机器人", tmp);

					if(0 == memcmp(tmp, consoleIp, strlen(consoleIp)))
					{
						PRINT("该连接为控制台，已经准备完毕!");
						consoleSocket->copy(fd, true);
						consoleSocket->setNonBlock();
						//consoleSocket->setKeepAlive();
						//控制台已经连接
						setConsoleIsConnected(true);
#ifdef  ROBOT_SENSOR_DATA_DELAY_SEND
						minAck = maxAck = 1;
						ACK = 0;
						//控制台链接好，则可以将保存的文件发送过去
						semaphoreFlag = 0;
						semaPhore->Post();
#endif
					}
					else if(0 == memcmp(tmp, frontRobotIp, strlen(frontRobotIp)))
					{
						//编队前方机器人连接
						frontRobotFd = fd;
						//accept前方机器人的连接请求
						setFrontRobotIsAccepted(true);
					}
					else if(0 == memcmp(tmp, backRobotIp, strlen(backRobotIp)))
					{
						//编队后方机器人连接
						backRobotFd = fd;
						//accept后方机器人的连接请求
						setBackRobotIsAccepted(true);
					}
				}//end for " case RobotNetEpoll::EpollAccept"
				break;

				case RobotNetEpoll::EpollError:
				{
						if(fd == consoleSocket->getFD())
						{
							PRINT("-->链接到控制台网络被异常断开-\n");
							setConsoleIsConnected(false);
#ifdef  ROBOT_SENSOR_DATA_DELAY_SEND
							semaphoreFlag = -1;
							semaPhore->Post();
#endif
						}
				}
				break;
				case RobotNetEpoll::EpollSuccessNoData:break;
				case RobotNetEpoll::EpollSocketClosed:
				{
					//某个套接字失效
					if(frontRobotFd == fd)
					{
						//失效前方连接套接字
						frontRobotFd = -1;
						setFrontRobotIsAccepted(false);
						PRINT("-->编队队伍中前方机器人已经主动断开至该机器人的连接<--\n");
					}
					else if(backRobotFd == fd)
					{
						backRobotFd = -1;
						setBackRobotIsAccepted(false);
						PRINT("-->编队队伍中后方机器人已经主动断开至该机器人的连接<--\n");
					}
					else if(fd == consoleSocket->getFD())
					{
						PRINT("-->控制台已经主动断开至该机器人的连接<--\n");
						setConsoleIsConnected(false);
#ifdef  ROBOT_SENSOR_DATA_DELAY_SEND
						semaphoreFlag = -1;
						semaPhore->Post();
#endif
					}
					else if(fd == UDPBroadcastSocket->getFD())
					{
						//throw RobotException("UDP服务器出错");
						RELEASE_HEAP_MEMORY(UDPBroadcastSocket);
						//重启UDP服务器
						initUDPServer();
					}
				}// end for " case RobotNetEpoll::EpollSocketClosed"
			//	break; } //end for case
			}//end for " switch(epollStatus)"
		}//end for " for(i = 0; i < nevents; ++i)"
	} //end for "while(true)"
}
//处理串口数据
void Robot::processData(const unsigned char* arr, int len)
{
	//组帧
	copyAsFrame(arr, len);
	//robotPacketInternal.printHex();
#ifdef  ROBOT_SENSOR_DATA_DELAY_SEND
	addSensorData(1,2);
#endif
	if(likely(RobotPacket::SonarData == robotPacketInternal.getCommandByIndex()))
	{
		//	统计超声波数据
		for(int i = 0; i < SONAR_NUM;  ++i)
		{
			//毫米为单位
			disObs[i] = RobotMath::charToShort(robotPacketInternal.getByteByIndex(3 +  (i << 1)),
								robotPacketInternal.getByteByIndex((i << 1) + 4));
		}
		//以下都针对的是：超声波传感器数据
		if(modeLock.getMode() != RobotModeLock::MODE_JOYSTICK)
		{
			//路径规划
			//if(modeLock.getMode() == RobotModeLock::MODE_WANDER)
				wheelAction = robotPathPlan->testFire();
				if(wheelAction == NULL) return;
				/* else wheelAction = robotPathPlan->testLinksFire(); }*/
			//直接使用类的成员Task发送
			robotWheelTask->setWheel(wheelAction->getLeftVel(), wheelAction->getRightVel());

			PRINT("发送给机器人的控制速度:左轮[%hd]-右轮[%hd]\n", wheelAction->getLeftVel(), wheelAction->getRightVel());

			robotWheelTask->doTask();
			//robotWheelTask->printPacket();
			mediator->send(robotWheelTask->getTaskBuf(), robotWheelTask->getTaskBufLen());
			//PRINT("已经发送出去");
#if 0
			//动态分配任务的内存区
			RobotTask * at = new RobotWheelTask(wheelAction->getLeftVel(),
							wheelAction->getRightVel(), 1, RobotTask::Common);
	#if 0
			RobotTaskPriorityQueue::getInstance()->cond.lock();
	#endif
			RobotTaskPriorityQueue::getInstance()->taskPriorityQueue.pushTask(at);
			RobotTaskPriorityQueue::getInstance()->taskPriorityQueue.runTopTask();
	#if 0
			RobotTaskPriorityQueue::getInstance()->cond.signal();
			RobotTaskPriorityQueue::getInstance()->cond.unlock();
	#endif
	#endif
		}
		return;

		//最新运行总距离
		newTotalDist = RobotMath::charToShort(robotPacketInternal.getByteByIndex(27),
																					robotPacketInternal.getByteByIndex(28));
		//最新电子罗盘测得角度
		newAngle = static_cast<float>(
				RobotMath::charToShort(robotPacketInternal.getByteByIndex(29),
				robotPacketInternal.getByteByIndex(30))) * 0.1;

		//最新机器人头部偏转
		newDeltaAngle = static_cast<float>(
				RobotMath::charToSignedShort(robotPacketInternal.getByteByIndex(31), 
				robotPacketInternal.getByteByIndex(32)))  * 0.057325;

		newDeltaS =  static_cast<float>(
				RobotMath::charToSignedShort(robotPacketInternal.getByteByIndex(33), 
				robotPacketInternal.getByteByIndex(34)));

		//如果是第一次上传数据
		if(isFirstFrame)
		{
			isFirstFrame = false;

			//记录第一次的数据作为基准
			oldTotalDist = newTotalDist;

			//机器人初始化位姿
			oldRobotPoint.setX(robotPoint.getX());
			oldRobotPoint.setY(robotPoint.getY());
			//oldRobotPoint.setTh(newAngle);
			oldRobotPoint.setTh(0);

			return;
		}
		//获得前后两个状态的运行距离差,dist
		deltaDist = newTotalDist - oldTotalDist ;	
		//更新前一个状态的距离
		oldTotalDist = newTotalDist; 

		deltaAngle =  newAngle - oldRobotPoint.getTh();
		deltaRad = RobotMath::degToRad(deltaAngle);

		//多传感器融合定位
		calXBySensorMerge =  cos(deltaRad) * deltaDist * cos(oldRobotPoint.getThRad()) -
								sin(deltaRad) * deltaDist * sin(oldRobotPoint.getThRad()) + oldRobotPoint.getX();

		calYBySensorMerge = cos(deltaRad) * deltaDist * sin(oldRobotPoint.getThRad()) +
								sin(deltaRad) * deltaDist * cos(oldRobotPoint.getThRad()) + oldRobotPoint.getY();

		calAngleBySensorMerge = newAngle;
		
		//依靠运动模型定位
		calXByMotionModel = oldRobotPoint.getX() + 
								newDeltaS * cos(oldRobotPoint.getThRad() + (RobotMath::degToRad(newDeltaAngle) * 0.5));

		calYByMotionModel = oldRobotPoint.getY() + 
								newDeltaS * sin(oldRobotPoint.getThRad() + (RobotMath::degToRad(newDeltaAngle) * 0.5));

		calAngleByMotionModel = oldRobotPoint.getTh() + newDeltaAngle ;
		
		robotPoint.setX(calXBySensorMerge * coefficientSensorMeger + calXByMotionModel* coefficientMotionModel);
		robotPoint.setY(calYBySensorMerge * coefficientSensorMeger + calYByMotionModel* coefficientMotionModel);
		robotPoint.setTh(calAngleBySensorMerge * coefficientSensorMeger + calAngleByMotionModel * coefficientMotionModel);

		//PRINT("共前行了%d厘米，本次前行了%d厘米，当前角度为%.2f",newTotalDist, deltDist, robotPoint.getTh());
		//PRINT("机器人本次转动角度[%.2f],与机器人本次运动距离[%d],机器人deltaS[%.2f]\n",newDeltaAngle, deltaDist, newDeltaS);

		PRINT("机器人当前坐标(%.2f, %.2f, %.2f)\n\n", robotPoint.getX(), robotPoint.getY(), robotPoint.getTh());

		oldRobotPoint = robotPoint;
		//return;
		
	}
}//end for func


//建立epoll tcp服务器
bool Robot::setUpSerivceListen(const char* ip, int port, RobotSocket::NetType socketType)
{
	//为控制台的连接准备资源
	consoleSocket = new RobotSocket();
	//监听套接字，作为服务器端
	listenSocket = new RobotSocket(/*ip, port, true, socketType*/);			

	if(listenSocket->open(port, socketType, ip) == false)
	{
		int tmpTryCnt = 4;

		while(--tmpTryCnt)
		{
			if(listenSocket->findValidPort(++port, ip))
			{
				PRINT("TCP端口%d\n",port);

				break;
			}
		}
		
		if(tmpTryCnt != 0)
		{
			listenSocket->Listen();
		}
		else
			return false;
	}
	listenSocket->setReuseAddress();

	if(0 == netEpoll->openEpoll(30))
	{
  	netEpoll->registerHandler(listenSocket->getFD(), RobotNetEpoll::ACCEPT_MASK);
		return true;
	}
	else
	{
		RELEASE_HEAP_MEMORY(listenSocket);
		RELEASE_HEAP_MEMORY(consoleSocket);
		return false;
	}
}


bool Robot::setUpUDPBroadcast(const char* broadcastIp, int port)
{
	UDPBroadcastSocket = new RobotSocket(/*broadcastIp, port, true, RobotSocket::UDP*/);
	if(UDPBroadcastSocket->open(port, RobotSocket::UDP, broadcastIp) == false)
	{
		int tmpTryCnt = 4;
		while(--tmpTryCnt)
		{
			if(UDPBroadcastSocket->findValidPort(++port, broadcastIp))
			{
				PRINT("UDP端口%d\n",port);

				break;
			}
		}
		if(tmpTryCnt == 0)
			return false;
	}
	if(!UDPBroadcastSocket->setBroadcast(true)) 
		return false;
	if(!UDPBroadcastSocket->setReuseAddress())
		return false;
	broadcastSockaddr.sin_family = AF_INET;
	broadcastSockaddr.sin_port = robotBroadcastPort;
	inet_pton(AF_INET, robotBroadcastIp, &broadcastSockaddr.sin_addr);

	//注册
 	netEpoll->registerHandler(UDPBroadcastSocket->getFD(), RobotNetEpoll::READ_MASK);
	return true;
}

bool Robot::initUDPServer()
{
	bool ret = setUpUDPBroadcast(robotBroadcastIp, robotBroadcastPort);
	if(!ret)
		setUDPServerIsBuilded(false);
	else
		setUDPServerIsBuilded(true);
	return ret;
}
//递归启动网络模块
bool Robot::initTCPServer()
{
	netEpoll = new RobotNetEpoll();

	//建立Epoll机制服务器:参数：IP（或者主机名）+端口
	if(!setUpSerivceListen(robotIp, robotPort))
	{
		PRINT("《《《《《《《Epoll建立服务器失败, 稍后5秒重试》》》》》》\n");
		//网络建立不成功则将权利交给智能模式
		if(--failureTryCnt == 0)
			return false;
		//并释放已经申请的内存
		RELEASE_HEAP_MEMORY(netEpoll);
		//网络服务线程返回后结束
		sleep(5);
		return initTCPServer();
	}//end for if
	//TCP服务器已经建立
	setTCPServerIsBuilded(true);	
	return true;	
}

void Robot::setGoal(float x, float y, float theTa)
{
	robotGoalPoint.setX(x);
	robotGoalPoint.setY(y);
	robotGoalPoint.setTh(theTa);
	robotPathPlan->setGoal(robotGoalPoint);
}
void Robot::cancelGoal(void) { robotPathPlan->cancelGoal(); }
void Robot::setGoal(Point &p) { 
	robotGoalPoint = p;
	robotPathPlan->setGoal(p); 
}

#ifdef  ROBOT_SENSOR_DATA_DELAY_SEND
void Robot::sendSensorData()
{
	if(openSensorDataRead())
	{
		char tmpData[2]; 
		fs.read(tmpData, sizeof(tmpData));
		while(!fs.eof())
		{
			fs.read(tmpData, sizeof(tmpData));
			//consoleSocket->write(tmpData, 2);
			tempAndMonistPacketSave.empty();
			tempAndMonistPacketSave.byteToBuf(tmpData[0]);
			tempAndMonistPacketSave.byteToBuf(tmpData[1]);
			tempAndMonistPacketSave.finalizePacket(); 
			PRINT("传感器信息转发,长度为%d",tempAndMonistPacketSave.getLength());
  		//tempAndMonistPacketSave.printHex();
			consoleSocket->write(tempAndMonistPacketSave.getBuf(), tempAndMonistPacketSave.getLength());
			RobotSleep::RobotSleepSec(1);
		}
		closeSensorDataFile();
	}
}

void Robot::emptyData()
{
	fs.open(dataFilePath,ios::trunc | ios::out);
	closeSensorDataFile();
}
//打开数据文件
bool Robot::openSensorDataSave()
{
	closeSensorDataFile();
	fs.open(dataFilePath,ios::binary | ios::trunc | ios::out);
	if(!fs) return false;
	return true;
}
//关闭打开的数据文件
void Robot::closeSensorDataFile()
{
	if(fs.is_open()){
		fs.close();
		fs.clear();
		}
}
bool Robot::openSensorDataRead()
{
	closeSensorDataFile();
	fs.open(dataFilePath,ios::binary | ios::in);
	if(!fs) return false;
	return true;
}
void Robot::addSensorData(char tempSensor, char monistSensor)
{
	//如果控制台已经连接
	if(consoleIsConnected)
	{
		tempAndMonistPacket.empty();
		tempAndMonistPacket.byteToBuf(tempSensor);
		tempAndMonistPacket.byteToBuf(monistSensor);
		tempAndMonistPacket.finalizePacket(); 

		PRINT("控制台连接，传感器信息直接发送");
  	//tempAndMonistPacket.printHex();
		consoleSocket->write(tempAndMonistPacket.getBuf(), tempAndMonistPacket.getLength());
	}
	else if(fsIsUseToWrite)
	{
		if(fs.is_open())
			PRINT("控制台没有连接，传感器信息直接保存");
		writeData[0] = tempSensor;
		writeData[1] = monistSensor;
		fs.write(writeData, sizeof(writeData));
	}
}
#endif


#ifdef JOYSTICK
bool Robot::initJoyStick()
{
	//构造手柄控制类对象指针
	joyStick = new RobotJoyStick();
	return true;
}
#endif

//初始化路径规划
bool  Robot::initPathPlanModule()
{
	//构造路径规划类对象指针
	robotPathPlan = new RobotPathPlan(this);
	return true;
}


//设置机器人服务器绑定IP
void Robot::setIp(const char *tIp,int len)
{
	memcpy(robotIp, tIp, len);
	robotIp[len] = 0;
	PRINT("机器人IP地址为[%s]",robotIp);
}


void Robot::setConsoleIp(const char* tIp, int len)
{
	memcpy(consoleIp, tIp, len);
	consoleIp[len] = '\0';
	PRINT("控制台IP地址为[%s]",consoleIp);
}


void Robot::setFrontRobotIp(const char* tIp, int len)
{
	memcpy(frontRobotIp, tIp, len);
	frontRobotIp[len] = '\0';
	PRINT("前方机器人IP地址为[%s]",frontRobotIp);
}


void Robot::setBackRobotIp(const char* tIp, int len)
{
	memcpy(backRobotIp, tIp, len);
	backRobotIp[len] = '\0';
	PRINT("后方机器人IP地址为[%s]",backRobotIp);
}


void Robot::setBroadcastIp(const char* tIp, int len)
{
	memcpy(robotBroadcastIp, tIp, len);
	robotBroadcastIp[len] = '\0';
	PRINT("广播地址为[%s]",robotBroadcastIp);
}

//设置机器人建立服务器端口
void Robot::setPort(int port) 
{
	robotPort = port;
	PRINT("机器人服务器端口为[%d]", robotPort);
}


void Robot::setBroadcastPort(int port) 
{
	robotBroadcastPort = port;
	PRINT("机器人UDP广播服务器端口为[%d]", robotBroadcastPort);
}


bool Robot::initTask()
{
	robotWheelTask = new RobotWheelTask(0, 0);
	robotLightTask = new RobotLightTask(1, 0);
	return true;
}


void Robot::initClientModule(void)
{
	frontRobotSocket = new RobotSocket();
	backRobotSocket = new RobotSocket();
}


//连接前方机器人
bool Robot::connectFrontRobot(void)
{
	//寻找服务器正在使用的端口
	for(int i = 0; i <= 3; ++i)
	{
		if(frontRobotSocket->connect(frontRobotIp, robotPort + i, RobotSocket::TCP))
			return true;	
	}
	return false;
}

//连接后方机器人
bool Robot::connectBackRobot(void)
{
	for(int i = 0; i <= 3; ++i)
	{
		if(backRobotSocket->connect(backRobotIp, robotPort + i, RobotSocket::TCP))
			return true;	
	}
	return false;
}

//接受网络事件主循环
void Robot::startAndWaitForExit() 
{
	if(getTCPServerIsBuilded() || getUDPServerIsBuilded())
	{
		processNetEpoll();
	}
	else
		throw RobotException("未建立任何网络服务器，异常退出");
}

#ifdef  ROBOT_SENSOR_DATA_DELAY_SEND
void Robot::keepAliveCheck(void)
{
	while(true)
	{
		if(consoleIsConnected)
		{
			++ACK;
			if(ACK >= 250)
			{
				ACK = 0;
				minAck = ACK;
			}
			//heatPacket.setIdByIndex(ACK);
			heatPacket.setId(ACK);
			maxAck = ACK;
			if(maxAck - minAck > minInterval)
			{
					  PRINT("超时未收到ACK,%d----%d",maxAck, minAck);
						consoleSocket->close();
						netEpoll->removeHandler(consoleSocket->getFD());
						semaphoreFlag = -1;
						semaPhore->Post();
						setConsoleIsConnected(false);
						minAck = maxAck = 0;
				/*关闭套接字*/
			}
			else
			{
				heatPacket.empty();
				heatPacket.byteToBuf(0);
				heatPacket.byteToBuf(0);
				heatPacket.finalizePacket(); 
				consoleSocket->write(heatPacket.getBuf(), heatPacket.getLength());
				//heatPacket.printHex();
			}
		}
		sleep(10);
	}
}
#endif
//处理编队时连接前方和后方机器人
void Robot::processSensorData()
{
	while(true)
	{
		//PRINT("信号灯等待");
		//如果前边机器人未连接
		if(semaPhore->Wait() == 0)
		{
			//网络接通
			if(semaphoreFlag == 0)
			{
				#ifdef  ROBOT_SENSOR_DATA_DELAY_SEND
				if(consoleIsConnected)
				{
					PRINT("控制台连接，正在发送保存的数据");
					//准备发送传感器数据，所以现在不能写
					fsIsUseToWrite= false;
					sendSensorData();
					emptyData();
					PRINT("发送保存的数据完成");
					//上边将文件中的数据全部发送出去并清空。
					//接下来继续打开该文件来等待网络断开后写入
					//现在可以写
					openSensorDataSave();
					fsIsUseToWrite= true;
				}
				#endif
				continue;
			}
			//网络断开
			else if(semaphoreFlag == -1)
			{
#ifdef  ROBOT_SENSOR_DATA_DELAY_SEND
				PRINT("控制台断开，正在创建传感器文件并收集传感器信息");
				//文件被打开用于存放数据
				openSensorDataSave();
				//网络断开，现在可以写
				fsIsUseToWrite = true;
#endif
				continue;
			}
			//PRINT("信号灯释放");
			if(getHaveFrontRobot() && !getFrontRobotIsConnected())
			{
				if(connectFrontRobot())
				{
					setFrontRobotIsConnected(true);
					PRINT("连接到前方机器人成功");
				}
				else
					PRINT("连接到前方机器人失败");
			}
			if(getHaveBackRobot() && !getBackRobotIsConnected())
			{
				if(connectBackRobot())
				{
					setBackRobotIsConnected(true);
					PRINT("连接到后方机器人成功");
				}
				else
					PRINT("连接到后方机器人失败");
			}
		}
	}
}




int Robot::getClosestSonarRange(int startI, int endI) const
{
	int myMin = INT_MAX;
	for(int i = startI; i < endI; ++i)
			if(disObs[i] < myMin)
					myMin = disObs[i];
	return ((myMin == INT_MAX) ? 0 : myMin);
}


int Robot::checkRangeDevicesCurrentPolar(int startI, int endI)
{
	return  getClosestSonarRange(startI, endI);
}





#ifdef JOYSTICK
void Robot::processJoyStick(void)
{
	#define JOY_BUFF 100
	unsigned char joyBuf[JOY_BUFF];
	int joyDataLen;
	//准备建立网络
	joyStick->netSetupPrepare();					
	//服务器端主动建立，被动连接
	if(joyStick->netSetup()){}						
	else 
	{
		// 网络建立不成功则将权利交给智能模式
		modeLock.setMode(ModeLock::MODE_WANDER); 						
		delete joyStick;
		joyStick = 0;
		return;
	}
	while(1)
	{
		/*接收手柄消息*/
		joyDataLen = joyStick->receiveMessage(joyBuf, JOY_BUFF);
		//客户端关闭
		if(joyDataLen == 0)
			break;
		//防止溢出
		else if(joyDataLen == JOY_BUFF)
			continue;
		//为控制命令（在手柄模式时有效）
		if((modeLock.getMode() == ModeLock::MODE_JOYSTICK) && (joyBuf[2] == Type::RealCtlData))
		{
			mediator->sendMsg(joyBuf, joyDataLen);
		}
		//为模式切换命令
		else if(joyBuf[2] == Type::ModeChange) 		
		{
			//模式切换
			if(modeLock.getMode() == ModeLock::MODE_JOYSTICK) 
				modeLock.setMode(ModeLock::MODE_WANDER);
			else
				modeLock.setMode(ModeLock::MODE_JOYSTICK);
	
		}//end for else
	}//end for while(1)
	stop();
	//权利交给智能模式
	modeLock.setMode(ModeLock::MODE_WANDER); 						
	delete joyStick;
	joyStick = 0;
}
#endif


//使用任务优先队列来处理任务
#ifdef TASK_QUEUE_PROCESS
void Robot::processTaskQueue(void)
{
	int ret;
	while(1)
	{
		RobotTaskPriorityQueue::getInstance()->cond.lock();
		if(!(RobotTaskPriorityQueue::getInstance()->taskPriorityQueue.empty()))	{}
		else
		{
			ret = RobotTaskPriorityQueue::getInstance()->cond.timedWait(2000);
			if(ret != 0 )
			{
				RobotTaskPriorityQueue::getInstance()->cond.unlock();
				continue;
			}
		}

		RobotTaskPriorityQueue::getInstance()->taskPriorityQueue.runTopTask();
		RobotTaskPriorityQueue::getInstance()->cond.unlock();
	}
}
#endif


Robot::~Robot(void) throw() 
{
	robotResourceClear();
	exit(0);
}

void Robot::setHaveFrontRobot(bool b)	
{
	haveFrontRobot = b;
}


bool Robot::getHaveFrontRobot()
{
	return haveFrontRobot ;
}


void Robot::setHaveBackRobot(bool b)	
{
	haveBackRobot = b;
}


bool Robot::getHaveBackRobot()
{
	return haveBackRobot;
}


void Robot::setFrontRobotIsConnected(bool b)
{
	frontRobotIsConnected = b;
}


bool Robot::getFrontRobotIsConnected()
{
	return frontRobotIsConnected;
}


void Robot::setBackRobotIsConnected(bool b)
{
	backRobotIsConnected = b; 
}


bool Robot::getBackRobotIsConnected()
{
	return 	backRobotIsConnected;
}


void Robot::setRobotIsRuning(bool b)
{
	robotIsRuning = b;
}


bool Robot::getRobotIsRuning()
{
	return robotIsRuning;
}


void Robot::setRobotDriverIsReady(bool b)
{
	robotDriverIsReady = b;
}


bool Robot::getRobotDriverIsReady()
{
	return robotDriverIsReady;
}


void Robot::setConsoleIsConnected(bool b)
{
	consoleIsConnected = b;
}

bool Robot::getConsoleIsConnected(void)
{
	return 	consoleIsConnected;
}


void Robot::setFrontRobotIsAccepted(bool b)
{
	frontRobotIsAccepted = b;
}

bool Robot::getFrontRobotIsAccepted(void)
{
	return	frontRobotIsAccepted;
}

void Robot::setBackRobotIsAccepted(bool b)
{
	backRobotIsAccepted = b;
}


bool Robot::getBackRobotIsAccepted(void)
{
	return	backRobotIsAccepted;
}


void Robot::setTCPServerIsBuilded(bool b)
{
	TCPServerIsBuilded = b;
}

bool Robot::getTCPServerIsBuilded()
{
	return	TCPServerIsBuilded;
}

void Robot::setUDPServerIsBuilded(bool b)
{
	UDPServerIsBuilded = b;
}

bool Robot::getUDPServerIsBuilded()
{
	return	UDPServerIsBuilded;
}

#ifdef ROBOT_HAVE_LOG
void Robot::setLogPath(string path)
{
	logPath = path;
}

std::string& Robot::getLogPath() 
{
	return logPath;
}
#endif

#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif

