#pragma once
#ifndef ROBOT_MAIN_ACTIONDESIRED_H_
#define ROBOT_MAIN_ACTIONDESIRED_H_

/*
 * Powered by RenZhenwen
 * date:2012-12-07
 *
 * */
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
//#include <utility>
class RobotActionDesired
{
private:
	//float headDelta;
	//左轮速度
	short lSpeed; 
	//右轮速度	
	short rSpeed;
public:
	//typedef pair<float, float> DesiredPair; 
	RobotActionDesired(short tSpeedL = 0, short tSpeedR = 0) : 
		lSpeed(tSpeedL), rSpeed(tSpeedR) {}
	~RobotActionDesired(void){}
	RobotActionDesired(const RobotActionDesired &right_) : lSpeed(right_.lSpeed), rSpeed(right_.rSpeed) {}
	bool operator==(const RobotActionDesired &right) const
	{
		return ((lSpeed == right.lSpeed) && (rSpeed == right.rSpeed));
	}
	RobotActionDesired& operator=(const RobotActionDesired &right)
	{
		lSpeed = right.lSpeed;
		rSpeed = right.rSpeed;
		return *this;
	}
	void setLeftVel(short tSpeedL)
	{
		lSpeed = tSpeedL;
	}
	void setRightVel(short tSpeedR)
	{
		rSpeed = tSpeedR;
	}
	void setVel(short tSpeedL, short tSpeedR)
	{
		lSpeed = tSpeedL;
		rSpeed = tSpeedR;
	}
	short getLeftVel(void) const
	{
		return lSpeed;
	}
	short getRightVel(void) const
	{
		return rSpeed;
	}
};
#ifdef ROBOT_NAMESPACE
}
#endif
#endif  //ROBOT_MAIN_ACTIONDESIRED_H_
