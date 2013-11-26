#pragma once
#ifndef ROBOT_MAIN_PATHPLAN_H_
#define ROBOT_MAIN_PATHPLAN_H_

/*
 * Power by RenZhenwen
 * Date:2012-12-07
 * Email:admin@unix8.net
 * */
#include "RobotActionDesired.h"
#include "RobotMath.h"
#include "RobotDataQueue.h"
#include "RobotCoorTrans.h"

//#include "RobotDef.h"
//#include <utility>
//#include <iostream>
//using namespace std;
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
struct ObsPose
{
	float x;	//障碍物X坐标
	float y;	//障碍物Y坐标
	float angle;	//障碍物在机器人坐标系中的角度
	float dis;		//障碍物距离
	float getX() const {return x;}
	float getY() const {return y;}
	float getAngle() const {return angle;}
	float getDis() const {return dis;}

	void setX(float t_) { x = t_;}
	void setY(float t_) { y = t_;}
	void setAngle(float t_) { angle = t_;}
	void setDis(float t_) { dis = t_;}
};
class Robot;
class RobotPathPlan
{
public:
	#define FRONT_RANGE 0.33
	#define SIDE_RANGE 0.22
	#define WALL_RANGE 0.05
  #define SONAR 12
  #define UNVALIED 5
  #define DEBUG_INFO \
  {\
		printf("当前正在运行第%d行,请查看\n",__LINE__);\
	 }

	RobotPathPlan(Robot *robot_ = 0, Point goalPoint_ = Point(0.0, 0.0, 0.0), 
							float obsRange_ = 2, float safeRange_= 0.5, float closeDist = 1);
	~RobotPathPlan();
	//到达目的地判断++
	bool haveAchievedGoal(void);
	//取消当前的目标位置++
	void cancelGoal(void);  
	//设置目的地++
	void setGoal(Point goal);
	//获取当前的目的地++
	Point getGoal(void) { return myGoal; }

	//设置距离目标的最短距离++
	void setCloseDist(float closeDist_) { closeDist = closeDist_; }			
	//获取距离目标的最短距离++
	float getCloseDist(void) { return closeDist; }							
	//设置向目标移动的速度++
	void setSpeed(float speed) { mySpeed = speed; }								
	//获取速度++
	float getSpeed(void) { return mySpeed; }	
	//取消move动作+
	void setCancel(void);
	//判断取消动作状态+
	int isCanceled(void);
	//更新超声波传感器数据
	void updateObsDist(void);	
	//参数为上一个状态
	RobotActionDesired *fire(/*RobotActionDesired currentDesired*/); 
	RobotActionDesired *testFire();
	RobotActionDesired *testRoundFire();
	RobotActionDesired *testLinksFire();
	RobotActionDesired *testGoalFire();
	RobotActionDesired *getDesired(void) { return &myDesired; }
	const RobotActionDesired *getDesired(void) const  { return &myDesired; }
	//机器人 指针，由外部传入
	Robot *myRobot;									
private:
	//将地图中的目标点变换到机器人坐标
	void transMapGoal2RobotGoal(); 
	//利用人工势场法计算角度
	void calcAngleByAPF(); 
	//利用声纳检测障碍物，并添加至声纳障碍物列表
	void detectObsBySonar(); 
	//判断是否进入局部最小点
	bool isMinPoint(); 
	//沿边法避障
	bool voidObsByFollowOs();
	//判断是否在目标周围
	bool isAroundGoal(void);  

	void setDesired(short angle_ = 0.0, short speed_ = -1.0);
	//是否进入巷道
	bool isRoad();				
	//判断方向
	int judgeDir();				
	//是否在安全限度内，返回true表示已经出现紧急
	bool isInSafeRange(float t_)  
	{
		if(t_ <=0 )
			return false;
		else if(t_ > safeRange)
			return false;
		else
			return true;
	}
	//是否在安全限度内，返回true表示已经出现紧急
	bool isInObsRange(float t_)  
	{
		if(t_ <=0 )
			return false;
		else if(t_ > obsRange)
			return false;
		else
			return true;
	}
	
/*
 *	状态机共用体
 *
 * */
	enum FollowObsState
	{
		STATA_START_PREPARE_FOLLOW,
		STATE_START_FOLLOW, //沿着障碍物走]
		STATE_FOLLOW_DONE,//沿边结束
		STATA_START_PREPARE_TURNING,//准备旋转
		STATE_START_TURNING,			//开始旋转
		STATE_TURNING_DONE,			//旋转结束
		STATE_NO_TODO
	};
	enum VoidObsMethState
	{
		STATE_NONE,//没有
		STATE_APF,	//APF
		STATE_PREPARE_FOLLOWOBS,//APF转向沿边
		STATE_FOLLOWOBS   //沿边
	};
	enum State
	{
		STATE_NO_GOAL, 
		STATE_ACHIEVED_GOAL,
		STATE_GOING_TO_GOAL
	};  
	enum FireState
	{
		LINE_GO,
		TURN_GO,
		TURN_GO_1,
		BACK_GO
	};
	
	FireState fireState;
	int curDir;
	float s1, s2,s3;

	//沿边状态
	FollowObsState myFollowObsState;
	//避障碍方法			
	VoidObsMethState myVoidObsMethState;				
	//是否有目标
	State myState;
	//目标点
	Point myGoal;
	//障碍物作用距离,单位 mm
	float obsRange; 
	float invertObsRange;
	//安全距离
	float safeRange;	
	float closeDist;

	//开始角度
	float myStartAngle;
	float mySpeed;

	//目标点坐标
	float goalXInRobot;			
	float goalYInRobot;			
	float goalX, goalY, angleRobot, robotX, robotY,goalRadInRobot,goalAngleInRobot;
	int dir;
	//是否重新设置目标
	bool newGoal;								
	RobotActionDesired myDesired;
	//障碍物和超声波信息
	ObsPose  obsPose[16];  
};
#ifdef ROBOT_NAMESPACE
}
#endif
#endif //ROBOT_MAIN_PATHPLAN_H_
