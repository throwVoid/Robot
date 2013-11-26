#include "RobotPathPlan.h"
#include "RobotDataQueue.h"
#include "RobotMath.h"
#include "RobotCoorTrans.h"
#include <assert.h>

/*
 * power by RenZhenwen
 * date:2012-12-07
 *
 * */

#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
RobotPathPlan::~RobotPathPlan(void)
{
}

/*
 * obsRange - 障碍物影响距离
 * safeRange -机器人认为安全的距离
 * coseDist - 可以认为达到目标点的距离
 *
 * */
RobotPathPlan::RobotPathPlan(Robot *robot_, Point goalPoint_,  float obsRange_, float safeRange_, float closeDist_) : 
myRobot(robot_), myGoal(goalPoint_), obsRange(obsRange_), safeRange(safeRange_), closeDist(closeDist_)
{

    obsPose[0].angle =  1.57; 
    obsPose[1].angle =  0.872;
    obsPose[2].angle =  0.523;
    obsPose[3].angle =  0.174;
    obsPose[4].angle =  -0.174;
    obsPose[5].angle =  -0.523;
    obsPose[6].angle =  -0.872;
    obsPose[7].angle =  -1.57;
    obsPose[8].angle =  -1.57;

    obsPose[9].angle =  130.0 / 180.0 * 3.14;
    obsPose[10].angle =  150.0 / 180.0 * 3.14;
    obsPose[11].angle =  170.0 / 180.0 * 3.14;
/*  obsPose[12].angle =  -170.0 / 180.0 * 3.14;
    obsPose[13].angle = -150.0 / 180.0 * 3.14;
    obsPose[14].angle =  -130.0 / 180.0 * 3.14;
    obsPose[15].angle =  -90.0 / 180.0 * 3.14;
	
    obsRange = 2;							//障碍物影响距离
    safeRange = obsRange /4;				//安全距离
*/
    invertObsRange = 1.0 / obsRange;		//障碍物影响因子
    myFollowObsState = STATE_NO_TODO;  //不需要沿边
    myState = STATE_NO_GOAL;
    myVoidObsMethState = STATE_NONE;
    newGoal = false;
}


void RobotPathPlan::detectObsBySonar()//利用声纳检测障碍物，并添加至声纳障碍物列表
{
    for(int i = 0; i < SONAR; ++i)
    {				
        obsPose[i].dis = static_cast<float>(myRobot->getSonarRange(i)) / 1000;//读取声纳传感器新的障碍物信息

	//	printf("第%d个超声波传感器数据= %d,转换后= %.4f\n",i+1,myRobot->getSonarRange(i), obsPose[i].dis);
        if(obsPose[i].dis <= obsRange)
        {
			obsPose[i].x = obsPose[i].dis * cos(obsPose[i].angle);
			obsPose[i].y = obsPose[i].dis * sin(obsPose[i].angle);
		}
		else
		{
			obsPose[i].dis = UNVALIED;  //如果在安全范围外则设置为10米
		}
    }
	for(int i = 0; i < SONAR; i++)
	{
		if(obsPose[i].dis < 0.5)
			printf("超声波[%d],距离为[%.2f],小于0.5米\n",i+1,obsPose[i].dis);
	}
}

bool RobotPathPlan::isMinPoint()	//判断是否进入局部最小点
{

	if((goalAngleInRobot >= 80) || (goalAngleInRobot < -80))//机器人头部与目标夹角
	{
		DEBUG_INFO;
		return true;
	}
	if(isInSafeRange(obsPose[2].dis) && isInSafeRange(obsPose[3].dis) && isInSafeRange(obsPose[4].dis))
		return true;

	   //障碍太近，认为局部最小点
    	if(safeRange / 2 > myRobot->getClosestSonarRange(-90.0, 90.0))
    	{ 
		DEBUG_INFO;
        	return true;
    	}
	float angleDelta;
	angleDelta = myStartAngle - myRobot->getTh();
    	if(abs((int)angleDelta) > 90)  //机器人往回走，认为局部最小点
    	{
		DEBUG_INFO;
        	return true;
    	}
	return false;
}




bool RobotPathPlan::isRoad()    //巷道模式
{
	if(isInSafeRange(obsPose[3].dis))  //前方有障碍，则准备旋转
   	 {
		if(isAroundGoal())  //到达目标
		{
			setDesired(0,0);
			myState = STATE_ACHIEVED_GOAL;
			myVoidObsMethState = STATE_NONE;
			return true;
		}
		if(isInObsRange(obsPose[1].dis) || isInObsRange(obsPose[2].dis))//避免在巷道时，过于倾斜
		{
			setDesired(30,100);
			return true;
		}
		else if(isInObsRange(obsPose[4].dis) || isInObsRange(obsPose[5].dis))
		{
			setDesired(-30,100);
			return true;
		}

		if(myVoidObsMethState == STATE_APF ) //正在人工势场法时，发现最小点
		{
			DEBUG_INFO;
			myVoidObsMethState = STATE_PREPARE_FOLLOWOBS;//进入局部最小点，准备沿边
			setDesired(0,0);
		}
		else //正在沿边是再次发现局部最小点
		{
			DEBUG_INFO;
			myVoidObsMethState = STATE_FOLLOWOBS;   //继续沿边
			setDesired(0,0);
		}
		return true;	
    }
	
    if((isInObsRange(obsPose[0].dis) && isInObsRange(obsPose[6].dis))  ||
      (isInObsRange(obsPose[1].dis) && isInObsRange(obsPose[5].dis))  ||
	(isInObsRange(obsPose[11].dis) && isInObsRange(obsPose[7].dis))  
      )
    {
		if(isAroundGoal())  //到达目标
		{
			setDesired(0,0);
			myState = STATE_ACHIEVED_GOAL;
			myVoidObsMethState = STATE_NONE;
			return true;
		}
		if(isInSafeRange(obsPose[3].dis)||(isInObsRange(obsPose[3].dis) && isInSafeRange(obsPose[2].dis) && isInSafeRange(obsPose[4].dis)))
		{
			
			if(myVoidObsMethState != STATE_FOLLOWOBS)
			{
				setDesired(0,0);//是局部最小点则准备沿边
				myVoidObsMethState = STATE_PREPARE_FOLLOWOBS;DEBUG_INFO;
			}
			else
			{
				setDesired(0,50);
				myVoidObsMethState = STATE_FOLLOWOBS;
			}
			return true;
		}
		if(myRobot->getClosestSonarRange(-90,0)/1000 < safeRange) //在巷道中避免碰撞墙
		{
			setDesired(-30,100);
			return true;
		}
		else if(myRobot->getClosestSonarRange(0,90)/1000 < safeRange)
		{
			setDesired(30,100);
			return true;
		}
		else
		{
			setDesired(0,150);
			return true;
		}
    }
	return false;
}

void RobotPathPlan::transMapGoal2RobotGoal()//将地图中的目标点变换到机器人坐标系
{
	robotX = myRobot->getX() / 1000;//机器人在地图中的坐标，单位：m
	robotY = myRobot->getY() / 1000;
	angleRobot = myRobot->getTh();	//机器人前进方向与X轴的夹角，单位度
	goalX = myGoal.getX() / 1000;	//目标点在地图中的坐标，单位：m
	goalY = myGoal.getY() / 1000;
	printf("机器人当前位置[x=%.2f,y=%.2f,%.2f]\n",robotX,robotY,angleRobot);	
	Point p =CoorTrans::MapCoor2RobotCoor(robotX,robotY,angleRobot,goalX,goalY);
	goalXInRobot = p.getX();
	goalYInRobot = p.getY();
    	goalAngleInRobot = p.getTh();  //角度
	goalRadInRobot = p.getThRad();	
}

//利用人工势场法计算角度
void RobotPathPlan::calcAngleByAPF()
{
	float powerGoal, disGoal;
	float disObs,invertDisObs;
	float inta;
	float sumX,sumY;
	float powerObs,angleObs;
	if(isRoad())  
	{
		printf("进入巷道模式\n"); return;
	}
	//printf("目标点在机器人中的位置[x=%.2f,y=%.2f,%.2f]\n",goalXInRobot,goalYInRobot,goalAngleInRobot);	
	powerGoal = disGoal = sqrt(goalX * goalX + goalY * goalY);
    	sumX = goalXInRobot;
    	sumY = goalYInRobot;
	//printf("PowerGoal=%.2f;sumX=%.2f;sumY=%.2f\n",powerGoal,sumX,sumY);
	for(int i = 0; i < 6; ++i)
	{
		if(UNVALIED == obsPose[i].dis )
		{
			continue;				
		}
		disObs = RobotMath::distanceBetween(0,0,obsPose[i].x,obsPose[i].y ); //计算每个障碍物点到目标点的斥力
		invertDisObs =  1.0 / disObs;
		inta = disGoal * disGoal/ disObs;
		powerObs = 10 * (invertDisObs - invertObsRange) * (invertDisObs - invertObsRange) * (1 / (disObs * disObs * disObs)) * powerGoal * powerGoal;  //斥力
		angleObs = RobotMath::degToRad(RobotMath::fixAngle(RobotMath::radToDeg(obsPose[i].angle) -180));
	      	//调试很久才发现这里错误，改成现在这种清晰点，主要原因是开始的代码需要 弧度->角度-弧度，并且需要倒置
		sumX += powerObs * cos(angleObs);
		sumY += powerObs * sin(angleObs);
		//printf("斥力[%.2f],角度[%.2f],sumX(%.2f),sumY(%.2f)\n",powerObs,RobotMath::fixAngle(RobotMath::radToDeg(angleObs)),sumX,sumY);
		powerObs = 10 * (invertDisObs - invertObsRange) * (invertDisObs - invertObsRange) * powerGoal; 
		sumX += powerObs * cos(goalRadInRobot);
		sumY += powerObs * sin(goalRadInRobot);
	}
//	printf("角度是%.2f\n",CoorTrans::angleModify(sumX,sumY));
	setDesired(CoorTrans::angleModify(sumX,sumY));
	myVoidObsMethState = STATE_APF;
}



//沿边法避障
bool RobotPathPlan::voidObsByFollowOs()
{

	//if(isRoad())//沿边时不再巷道模式
	//	return true;
	if(isAroundGoal())  //到达目标
	{
		setDesired(0,0);
		myState = STATE_ACHIEVED_GOAL;
		myVoidObsMethState = STATE_NONE;
		return true;
	}
	if(isRoad())
	{
		return true;
	}

	if(myRobot->getClosestSonarRange(0,90) / 1000 <= safeRange) //在沿边的时候距离左面障碍物太近
	{
		DEBUG_INFO;
		myVoidObsMethState = STATE_FOLLOWOBS;//下一次继续沿边
		setDesired(-20,200);//稍微向右转
		return true;
	}
	if(myRobot->checkRangeDevicesCurrentPolar(-90,0) / 1000 <  safeRange) //在沿边的时候距离左面障碍物太近
	{
		DEBUG_INFO;
		myVoidObsMethState = STATE_FOLLOWOBS;//下一次继续沿边
		setDesired(20,200);//稍微向右转
		return true;
	}
	if(goalAngleInRobot >= 0) //目标在左面
	{
			
		//printf("%.2f安全距离出发\n",myRobot->checkRangeDevicesCurrentPolar(0,90));
		if(isInObsRange(obsPose[0].dis) ||
				isInObsRange(obsPose[1].dis) ||
				isInObsRange(obsPose[11].dis))
		{
			DEBUG_INFO;	
			if(isInObsRange(obsPose[0].dis))
			{
				DEBUG_INFO ;
				setDesired(20,100);
			}
			else
				setDesired(0,200);
			myVoidObsMethState = STATE_FOLLOWOBS;//下一次继续沿边
		}
		else  //左边已经没有障碍物了
		{
			DEBUG_INFO ;
			setDesired(0,0);
			myVoidObsMethState = STATE_APF; //下一次采用人工势场
		}
	}
	else if(goalAngleInRobot  < 0) //障碍物在右面
	{
		if(isInObsRange(obsPose[5].dis) ||
				isInObsRange(obsPose[6].dis) ||
				isInObsRange(obsPose[7].dis))
		{
			
			if(isInObsRange(obsPose[5].dis))
			{
				DEBUG_INFO ;
				setDesired(-20,200); //左面距离障碍物安全，则直接前进
			}
			else
				setDesired(0,200);
			myVoidObsMethState = STATE_FOLLOWOBS;//下一次继续沿边
		}
		else  //右边已经没有障碍物了
		{
			DEBUG_INFO ;
			setDesired(0,0);
			myVoidObsMethState = STATE_APF; //下一次采用人工势场
		}
	}

	return true;
}






bool RobotPathPlan::haveAchievedGoal(void)
{
  if (myState == STATE_ACHIEVED_GOAL)
    return true;
  else
    return false;
}

void RobotPathPlan::cancelGoal(void)
{
	myState = STATE_NO_GOAL;
	newGoal = false;
}

void RobotPathPlan::setGoal(Point goal)
{
    myState = STATE_GOING_TO_GOAL;
    myGoal = goal;
    newGoal = true;

}



bool RobotPathPlan::isAroundGoal(void)//判断是否已经到达目标
{
	if(myRobot->getPose().findDistanceTo(myGoal) < closeDist) //1米之内
	{
		return true;
	}
	return false;
}

void RobotPathPlan::setDesired(short angle_, short speed_)
{
 /*
  */
  myDesired.setLeftVel(angle_);
  myDesired.setRightVel(speed_);
  return;
  /*
  * */

	if(speed_ == -1.0)  //默认由角度算速度
	{
		speed_ = abs((int)angle_);
		speed_ = 180 - speed_;
		speed_ = speed_ / 180.0;
		speed_ = speed_ * 200;
	}
	if(angle_ >= 60) angle_ = 40;
	else if(angle_ <= -60) angle_ = -40;
	if(RobotMath::fabs(angle_) <=7) angle_ = 0;
	//myDesired.setDeltaHeading(angle_);
	//myDesired.setVel(speed_);
}
int RobotPathPlan::judgeDir()//开始沿边，判断方向
{
	if(goalAngleInRobot >= 0) 
	{
		if(isInObsRange(obsPose[1].dis) || isInObsRange(obsPose[2].dis))
			return 0;
		else
			return 1; //左转
	}
	else if(goalAngleInRobot < 0)//右转
	{

		if(isInObsRange(obsPose[4].dis) || isInObsRange(obsPose[5].dis))
			return 0;
		else
			return -1;
	}
	return 0;
}

RobotActionDesired *RobotPathPlan::testFire()
{
        detectObsBySonar();                     //探测超声波
        if(obsPose[3].dis <= 0.5)
        {
                if(obsPose[2].dis < obsPose[4].dis)
                {
                        setDesired(-30, 30);
                }
                else if(obsPose[2].dis >= obsPose[4].dis)
                {
                        setDesired(30, -30);
                }
        }
        else if(obsPose[2].dis < 0.35)
        {
                setDesired(-30, 30);
        }
        else if(obsPose[4].dis < 0.35)
        {
                setDesired(30, -30);
        }
/*
        else if(obsPose[1].dis < 0.1)
        {
                setDesired(20, -20);
        }
        else if(obsPose[5].dis < 0.1)
        {
                setDesired(-20, 20);
        }
*/
        else
                setDesired(30, 30);

        return &myDesired;;

}

RobotActionDesired *RobotPathPlan::fire(/*RobotActionDesired currentDesired*/)
{ 	
	return		testFire();
	
	if(newGoal)  //是一个新的点
    	{
        	myStartAngle = myRobot->getPose().findAngleTo(myGoal); //最初机器人与目标连线 与基坐标X轴夹角
        	newGoal = false;
	}
	if(myState ==STATE_NO_GOAL)
	{
		setDesired(0,0);
        	return &myDesired;
	}
	if(isAroundGoal())  //到达了目标周围
	{
		printf("Reach the goal\n");
	//	exit(0);
		myState = STATE_ACHIEVED_GOAL;
		myVoidObsMethState = STATE_NONE;
        	setDesired(0,0);
        	return &myDesired;
	}
	printf("--------------------分割线-------------------------------\n");
	detectObsBySonar();			//探测超声波
	transMapGoal2RobotGoal();		//地图转换

	//calcAngleByAPF();//计算角度
	//return &myDesired;//返回控制


	if(myVoidObsMethState == STATE_PREPARE_FOLLOWOBS) //已经准备好沿边
	{
		DEBUG_INFO;
		dir = judgeDir();  //准备沿边的方向
		if(1 == dir )
			setDesired(40,100);
		else if(dir == -1)
			setDesired(-40,100);  //最开始寻找目标点的大方向
		else
			setDesired(0,50);

		myVoidObsMethState = STATE_FOLLOWOBS;//开始沿边
		return &myDesired;
	}
	if(myVoidObsMethState == STATE_FOLLOWOBS) //上一步为沿边
	{
		DEBUG_INFO;
		voidObsByFollowOs();  //这个函数里已经设置好速度和下一次的动作
		/*可能转向APF，或STATE_FOLLOWOBS*/
		return &myDesired;
	}
	else if(myVoidObsMethState == STATE_APF) //上一个状态为APF
	{
		/*可能转向STATE_APF2STATE_FOLLOWOBS，STATE_APF*/
		if(!isMinPoint())//是局部最小点
		{
			DEBUG_INFO;
			calcAngleByAPF();//计算角度
		}
		else
		{
			DEBUG_INFO;
			setDesired(0,0);//是局部最小点则准备沿边
			myVoidObsMethState = STATE_PREPARE_FOLLOWOBS;

		}
		return &myDesired;
	}
	else if(myVoidObsMethState == STATE_NONE)		//上一步是没动作
	{   
		if(!isMinPoint())//不是局部最小点
		{
			DEBUG_INFO;
			calcAngleByAPF();//计算角度

		}
		else
		{ 
			DEBUG_INFO;
			setDesired(0,0);
			myVoidObsMethState = STATE_PREPARE_FOLLOWOBS;
		}
		return &myDesired;
	}
	printf("不存在的情况");
	setDesired(0,0);
	return &myDesired;
}
void RobotPathPlan::setCancel(void)
{
	myState = STATE_NO_GOAL;
	setDesired(0,0);
}

int RobotPathPlan::isCanceled(void)
{
	if(myState == STATE_NO_GOAL)
		return true;
	else
		return false;
}
#ifdef ROBOT_NAMESPACE
}
#endif
