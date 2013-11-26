#pragma once
#ifndef ROBOT_MAIN_MATH_H_
#define ROBOT_MAIN_MATH_H_
/*
 * power by RenZhenwen
 * date:2012-12-07
 * Email: admin@unix8.net
 *
 * */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>
#include <limits.h>
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
#define PI 3.1415
#define invert360  0.002777778
#define invert180  0.005555556
class RobotMath
{
public:
	//两个角度相加
	static float addAngle(float ang1, float ang2) 
	{ 
		return fixAngle(ang1 + ang2); 
	}
	//两个角度相减
	static float subAngle(float ang1, float ang2) 
	{ 
		return fixAngle(ang1 - ang2); 
	}
	//把角度转换到-180度到180
	static float fixAngle(float angle) 
	{
		if (angle >= 360)
			angle = static_cast<float>(angle - 360.0 * (float)((int)angle * invert360));
		if (angle < -360)
			angle = static_cast<float>(angle + 360.0 * (float)((int)angle * (-invert360)));
		if (angle <= -180)
			angle = static_cast<float>(+ 180.0 + (angle + 180.0));
		if (angle > 180)
			angle = static_cast<float>(- 180.0 + (angle - 180.0));
      return angle;
	} 
	//角度转弧度
	static float degToRad(float deg) { return static_cast<float>(deg * PI * invert180); }	
	//弧度转角度
	static float radToDeg(float rad) { return static_cast<float>(rad * 57.296 /*180.0 / PI*/); }	
	//cos（角度）
	static float cos(float angle) { return ::cos(RobotMath::degToRad(angle)); }
	//sin(角度）
	static float sin(float angle) { return ::sin(RobotMath::degToRad(angle)); }
	//tan（角度）
	static float tan(float angle) { return ::tan(RobotMath::degToRad(angle)); }
	//arctan（x,y）
	static float atan2(float y, float x) { return RobotMath::radToDeg(::atan2(y, x)); }
	//测试一个角度是否在一个范围内
	static bool angleBetween(float angle, float startAngle, float endAngle)
	{
		angle = fixAngle(angle);
		startAngle = fixAngle(startAngle);
		endAngle = fixAngle(endAngle);
		if ((startAngle < endAngle && angle > startAngle && angle < endAngle) ||
		(startAngle > endAngle && (angle > startAngle || angle < endAngle)))
			return true;
		else
			return false;
	}
	//双精度数的绝对值
	static float fabs(float val) 
	{
		if (val < 0.0)
			return -val;
		else
			return val;
	}
	//把双精度转换最近的整数
	static int roundInt(float val) 
	{ 
		val = static_cast<float>(val+ .49);
		if (val > INT_MAX)
			return (int) INT_MAX;
		else if (val < INT_MIN)
			return (int) INT_MIN;
		else
		return((int) floor(val)); 
	}
  //短整数范围内向上转换
	static short roundShort(float val) 
	{	 
		val = static_cast<float>(val + .49);
		if (val > 32767)
			return (short) 32767;
		else if (val < -32768)
			return (short) -32768;
		else
		return((short) floor(val)); 
	}
    
	//绕原点旋转一个坐标
	static void pointRotate(float *x, float *y, float th)
	{
		float cs, sn, xt, yt;
		cs = cos(th);
		sn = sin(th);
		xt = *x;  
		yt = *y;
		*x = cs * xt + sn * yt;
		*y = cs * yt - sn * xt;
    }
  //0--2^31之间的随机正整数
	static long random(void)
	{
		return(lrand48());
	}
	//计算两点距离
	static float distanceBetween(float x1, float y1, float x2, float y2)
    	{ return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));  }
	//计算两点距离的平方
	static float squaredDistanceBetween(float x1, float y1, float x2, float y2)
    	{ return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);  }
	//以2为底的对数
	static float log2(float x)
	{
		return static_cast<float>(log10(x) / 0.3010303); 
	}
  //两个8位转换成16位
  static unsigned short charToShort(unsigned char high, unsigned char low ) 
  { return (unsigned short)(((high & 0xff) << 8) | (low & 0xff)); }
  static short charToSignedShort(char high, char low ) 
  { return (short)(((high & 0xff) << 8) | (low & 0xff)); }
  //两个16位转换成32位
  static unsigned int shortToInt(unsigned short high, unsigned short low) 
  { return  (unsigned int)(((high & 0xffff) << 16) | (low & 0xffff)); }
  static int shortToSignedInt(short high, short low) 
  { return  (int)(((high & 0xffff) << 16) | (low & 0xffff)); }
};


class Point
{
public:
	Point(float x = 0, float y = 0, float th = 0)
	{ 
		myX = x;
		myY = y;
		myTh = th;
	}
	Point(const Point &pose) : myX(pose.myX), myY(pose.myY), myTh(pose.myTh) {}
	~Point(void) {}
	Point& operator=(const Point &p)
	{
		setX(p.getX());
		setY(p.getY());
		setTh(p.getTh());
		return *this;
	}
	//设置一个坐标点
	void setPose(float x, float y, float th = 0) 
	{ 
		setX(x);
		setY(y);
		setTh(th); 
	}
	//整个点赋值
	void setPose(Point position)
	{
		setX(position.getX());
		setY(position.getY());
		setTh(position.getTh());
	}
	void setX(float x) { myX = x; }
	void setY(float y) { myY = y; }
	void setTh(float th) { myTh = RobotMath::fixAngle(th); }
	//设置机器人头部角度，以弧度表示
	void setThRad(float th) { myTh = RobotMath::fixAngle(RobotMath::radToDeg(th)); }
	float getX(void) const { return myX; }
	float getY(void) const { return myY; }
	float getTh(void) const { return myTh; }
	float getThRad(void) const { return RobotMath::degToRad(myTh); }
	//以参数返回，目的就是一次获取机器人的位置信息，而不是每次返回一个属性
	void getPose(float *x, float *y, float *th = NULL) const
	{ 
		if (x != NULL) 
			*x = myX;
		if (y != NULL) 
			*y = myY; 
		if (th != NULL) 
			*th = myTh; 
	}
	//当前点到目标点的距离
	float findDistanceTo(const Point& position) const
	{
		return RobotMath::distanceBetween(getX(), getY(), 
				     position.getX(), 
				     position.getY());
	}
	//当前点到目标点的平方距离
	float squaredFindDistanceTo(const Point& position) const
	{
		 return RobotMath::squaredDistanceBetween(getX(), getY(), 
					    position.getX(), 
					    position.getY());
	}
	//计算点到目标点的角度,两点连线与X轴夹角
	float findAngleTo(const Point& position) const
	{
		return RobotMath::radToDeg(atan2(position.getY() - getY(), position.getX() - getX()));
	}
	//点加
	Point operator+(const Point& other)
	{
		return Point( myX + other.getX(), myY + other.getY(), RobotMath::fixAngle(myTh + other.getTh()) );
	}
	//点减
	Point operator-(const Point& other) 
	{
		return Point( myX - other.getX(), myY - other.getY(), RobotMath::fixAngle(myTh - other.getTh()) );
	}

private:
	float myX;
	float myY;
	float myTh;
};
#ifdef ROBOT_NAMESPACE
}
#endif
#endif//ROBOT_MAIN_MATH_H_
