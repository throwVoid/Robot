#pragma once
#ifndef ROBOT_MAIN_COORTRANS_H_
#define ROBOT_MAIN_COORTRANS_H_
/*
 * power by RenZhenwen
 * date:2012-12-07
 *
 * */
#include<math.h>

#include"RobotMath.h"
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
using namespace std;


class CoorTrans
{
public:
	static void angleModify(Point &p)  //测试正确
	{
		p.setThRad(atan2(p.getY(),p.getX()));
	}
	static float angleModify(float x, float y)   //测试正确，能求atan并且转换到-180---180度
	{
		return RobotMath::radToDeg(atan2(y,x));  //弧度转换成角度
	}

	/*
	 *	robotX:机器人在基坐标下的X坐标
	 *	robotY:机器人在基坐标下的Y坐标
	 *	goalX:目标在基坐标下的X坐标
	 *	goalY:目标在基坐标下的Y坐标
	 *	angleRobot:机器人前进方向（既机器人坐标系的X轴）与基坐标的X轴的夹角
	 *	返回值为目标点在机器人坐标系下的坐标，目标点向量与机器人坐标系X轴的夹角
	 */
	//基坐标系点转换成机器人坐标系坐标
	static Point MapCoor2RobotCoor(float robotX, float robotY, float angleRobot, float goalX,float goalY )//测试正确
	{
		Point goalPointInRobot;
		angleRobot =  RobotMath::degToRad(angleRobot);//把角度转换成弧度

		goalPointInRobot.setX((goalX - robotX) * cos(angleRobot) + (goalY - robotY) * sin(angleRobot)); //x坐标
		goalPointInRobot.setY((robotX - goalX) * sin(angleRobot) + (goalY - robotY) * cos(angleRobot)); //y坐标
		goalPointInRobot.setTh(CoorTrans::angleModify(goalPointInRobot.getX(),goalPointInRobot.getY()));
		return  (goalPointInRobot);//make_pair(goalPointInRobot, goalAngleInRobot);

	}

	/*
	 *	robotInMap:机器人在基坐标下位置
	 *	goalInMap:目标点在基坐标下的位置
	 *	返回值:目标在机器人坐标下的位置
	 */
	//基坐标系下的点转换成机器人坐标系坐标
	static Point MapCoor2RobotCoor(const Point& robotInMap, const Point& goalInMap) //测试正确
	{;
		Point goalPointInRobot = MapCoor2RobotCoor(robotInMap.getX(),robotInMap.getY(),robotInMap.getTh(),goalInMap.getX(),goalInMap.getY());
		return  (goalPointInRobot);//make_pair(goalPointInRobot, goalAngleInRobot);
	}

	/*
	 *	robotInMap:机器人在基坐标下的位置和方向
	 *	goalInRobot:目标点在机器人坐标下的坐标
	 *  返回值：返回目标点在基坐标下的坐标和角度
	 */
	static Point RobotCoor2MapCoor(const Point& robotInMap,const Point& goalInRobot)  //目标点在机器人坐标系下转换到基坐标系  正确
	{
		Point goalInMap;
		goalInMap.setX(goalInRobot.getX() * cos(robotInMap.getThRad()) - goalInRobot.getY()*sin(robotInMap.getThRad()) + robotInMap.getX());
		goalInMap.setY(goalInRobot.getX() * sin(robotInMap.getThRad()) + goalInRobot.getY()*cos(robotInMap.getThRad()) + robotInMap.getY());
		goalInMap.setTh(CoorTrans::angleModify(goalInMap.getX(),goalInMap.getY()));
		return goalInMap;
	}
	/*同上，只是参数未封装*/
	static Point RobotCoor2MapCoor(float robotX,float robotY, float robotAngle,float goalInRobotX,float goalInRobotY) 
	{
		return (RobotCoor2MapCoor(Point(robotX,robotY,robotAngle),Point(goalInRobotX,goalInRobotY)));
	}

	
	/*
	 *	x:机器人在基坐标下的X坐标
	 *	y:机器人在基坐标下的Y坐标
	 *	rotAngle:机器人旋转的角度
	 *  返回值为：机器人的新坐标和与基坐标的夹角
	 */
	static Point  PointRot(float x, float y, float rotAngle)   //*************************函数经过测试，正确
	{
		Point newPoint;
		rotAngle =  RobotMath::degToRad(rotAngle);
		newPoint.setX(cos(rotAngle) * x - sin(rotAngle) * y);
		newPoint.setY(sin(rotAngle) * x + cos(rotAngle) * y);
		newPoint.setTh(CoorTrans::angleModify(newPoint.getX(),newPoint.getY()));
		return (newPoint);//make_pair(newPoint,rotAngle);
	}

	static void setDeltaHeading(Point& robotInMap, float rotAngle)//机器人头部转动一个角度
	{
		robotInMap.setTh(RobotMath::addAngle(rotAngle ,robotInMap.getTh()));
	}
};
#ifdef ROBOT_NAMESPACE
}
#endif
#endif //ROBOT_MAIN_COORTRANS_H_
