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
 * obsRange - �ϰ���Ӱ�����
 * safeRange -��������Ϊ��ȫ�ľ���
 * coseDist - ������Ϊ�ﵽĿ���ľ���
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
	
    obsRange = 2;							//�ϰ���Ӱ�����
    safeRange = obsRange /4;				//��ȫ����
*/
    invertObsRange = 1.0 / obsRange;		//�ϰ���Ӱ������
    myFollowObsState = STATE_NO_TODO;  //����Ҫ�ر�
    myState = STATE_NO_GOAL;
    myVoidObsMethState = STATE_NONE;
    newGoal = false;
}


void RobotPathPlan::detectObsBySonar()//�������ɼ���ϰ��������������ϰ����б�
{
    for(int i = 0; i < SONAR; ++i)
    {				
        obsPose[i].dis = static_cast<float>(myRobot->getSonarRange(i)) / 1000;//��ȡ���ɴ������µ��ϰ�����Ϣ

	//	printf("��%d������������������= %d,ת����= %.4f\n",i+1,myRobot->getSonarRange(i), obsPose[i].dis);
        if(obsPose[i].dis <= obsRange)
        {
			obsPose[i].x = obsPose[i].dis * cos(obsPose[i].angle);
			obsPose[i].y = obsPose[i].dis * sin(obsPose[i].angle);
		}
		else
		{
			obsPose[i].dis = UNVALIED;  //����ڰ�ȫ��Χ��������Ϊ10��
		}
    }
	for(int i = 0; i < SONAR; i++)
	{
		if(obsPose[i].dis < 0.5)
			printf("������[%d],����Ϊ[%.2f],С��0.5��\n",i+1,obsPose[i].dis);
	}
}

bool RobotPathPlan::isMinPoint()	//�ж��Ƿ����ֲ���С��
{

	if((goalAngleInRobot >= 80) || (goalAngleInRobot < -80))//������ͷ����Ŀ��н�
	{
		DEBUG_INFO;
		return true;
	}
	if(isInSafeRange(obsPose[2].dis) && isInSafeRange(obsPose[3].dis) && isInSafeRange(obsPose[4].dis))
		return true;

	   //�ϰ�̫������Ϊ�ֲ���С��
    	if(safeRange / 2 > myRobot->getClosestSonarRange(-90.0, 90.0))
    	{ 
		DEBUG_INFO;
        	return true;
    	}
	float angleDelta;
	angleDelta = myStartAngle - myRobot->getTh();
    	if(abs((int)angleDelta) > 90)  //�����������ߣ���Ϊ�ֲ���С��
    	{
		DEBUG_INFO;
        	return true;
    	}
	return false;
}




bool RobotPathPlan::isRoad()    //���ģʽ
{
	if(isInSafeRange(obsPose[3].dis))  //ǰ�����ϰ�����׼����ת
   	 {
		if(isAroundGoal())  //����Ŀ��
		{
			setDesired(0,0);
			myState = STATE_ACHIEVED_GOAL;
			myVoidObsMethState = STATE_NONE;
			return true;
		}
		if(isInObsRange(obsPose[1].dis) || isInObsRange(obsPose[2].dis))//���������ʱ��������б
		{
			setDesired(30,100);
			return true;
		}
		else if(isInObsRange(obsPose[4].dis) || isInObsRange(obsPose[5].dis))
		{
			setDesired(-30,100);
			return true;
		}

		if(myVoidObsMethState == STATE_APF ) //�����˹��Ƴ���ʱ��������С��
		{
			DEBUG_INFO;
			myVoidObsMethState = STATE_PREPARE_FOLLOWOBS;//����ֲ���С�㣬׼���ر�
			setDesired(0,0);
		}
		else //�����ر����ٴη��־ֲ���С��
		{
			DEBUG_INFO;
			myVoidObsMethState = STATE_FOLLOWOBS;   //�����ر�
			setDesired(0,0);
		}
		return true;	
    }
	
    if((isInObsRange(obsPose[0].dis) && isInObsRange(obsPose[6].dis))  ||
      (isInObsRange(obsPose[1].dis) && isInObsRange(obsPose[5].dis))  ||
	(isInObsRange(obsPose[11].dis) && isInObsRange(obsPose[7].dis))  
      )
    {
		if(isAroundGoal())  //����Ŀ��
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
				setDesired(0,0);//�Ǿֲ���С����׼���ر�
				myVoidObsMethState = STATE_PREPARE_FOLLOWOBS;DEBUG_INFO;
			}
			else
			{
				setDesired(0,50);
				myVoidObsMethState = STATE_FOLLOWOBS;
			}
			return true;
		}
		if(myRobot->getClosestSonarRange(-90,0)/1000 < safeRange) //������б�����ײǽ
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

void RobotPathPlan::transMapGoal2RobotGoal()//����ͼ�е�Ŀ���任������������ϵ
{
	robotX = myRobot->getX() / 1000;//�������ڵ�ͼ�е����꣬��λ��m
	robotY = myRobot->getY() / 1000;
	angleRobot = myRobot->getTh();	//������ǰ��������X��ļнǣ���λ��
	goalX = myGoal.getX() / 1000;	//Ŀ����ڵ�ͼ�е����꣬��λ��m
	goalY = myGoal.getY() / 1000;
	printf("�����˵�ǰλ��[x=%.2f,y=%.2f,%.2f]\n",robotX,robotY,angleRobot);	
	Point p =CoorTrans::MapCoor2RobotCoor(robotX,robotY,angleRobot,goalX,goalY);
	goalXInRobot = p.getX();
	goalYInRobot = p.getY();
    	goalAngleInRobot = p.getTh();  //�Ƕ�
	goalRadInRobot = p.getThRad();	
}

//�����˹��Ƴ�������Ƕ�
void RobotPathPlan::calcAngleByAPF()
{
	float powerGoal, disGoal;
	float disObs,invertDisObs;
	float inta;
	float sumX,sumY;
	float powerObs,angleObs;
	if(isRoad())  
	{
		printf("�������ģʽ\n"); return;
	}
	//printf("Ŀ����ڻ������е�λ��[x=%.2f,y=%.2f,%.2f]\n",goalXInRobot,goalYInRobot,goalAngleInRobot);	
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
		disObs = RobotMath::distanceBetween(0,0,obsPose[i].x,obsPose[i].y ); //����ÿ���ϰ���㵽Ŀ���ĳ���
		invertDisObs =  1.0 / disObs;
		inta = disGoal * disGoal/ disObs;
		powerObs = 10 * (invertDisObs - invertObsRange) * (invertDisObs - invertObsRange) * (1 / (disObs * disObs * disObs)) * powerGoal * powerGoal;  //����
		angleObs = RobotMath::degToRad(RobotMath::fixAngle(RobotMath::radToDeg(obsPose[i].angle) -180));
	      	//���Ժܾòŷ���������󣬸ĳ��������������㣬��Ҫԭ���ǿ�ʼ�Ĵ�����Ҫ ����->�Ƕ�-���ȣ�������Ҫ����
		sumX += powerObs * cos(angleObs);
		sumY += powerObs * sin(angleObs);
		//printf("����[%.2f],�Ƕ�[%.2f],sumX(%.2f),sumY(%.2f)\n",powerObs,RobotMath::fixAngle(RobotMath::radToDeg(angleObs)),sumX,sumY);
		powerObs = 10 * (invertDisObs - invertObsRange) * (invertDisObs - invertObsRange) * powerGoal; 
		sumX += powerObs * cos(goalRadInRobot);
		sumY += powerObs * sin(goalRadInRobot);
	}
//	printf("�Ƕ���%.2f\n",CoorTrans::angleModify(sumX,sumY));
	setDesired(CoorTrans::angleModify(sumX,sumY));
	myVoidObsMethState = STATE_APF;
}



//�ر߷�����
bool RobotPathPlan::voidObsByFollowOs()
{

	//if(isRoad())//�ر�ʱ�������ģʽ
	//	return true;
	if(isAroundGoal())  //����Ŀ��
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

	if(myRobot->getClosestSonarRange(0,90) / 1000 <= safeRange) //���رߵ�ʱ����������ϰ���̫��
	{
		DEBUG_INFO;
		myVoidObsMethState = STATE_FOLLOWOBS;//��һ�μ����ر�
		setDesired(-20,200);//��΢����ת
		return true;
	}
	if(myRobot->checkRangeDevicesCurrentPolar(-90,0) / 1000 <  safeRange) //���رߵ�ʱ����������ϰ���̫��
	{
		DEBUG_INFO;
		myVoidObsMethState = STATE_FOLLOWOBS;//��һ�μ����ر�
		setDesired(20,200);//��΢����ת
		return true;
	}
	if(goalAngleInRobot >= 0) //Ŀ��������
	{
			
		//printf("%.2f��ȫ�������\n",myRobot->checkRangeDevicesCurrentPolar(0,90));
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
			myVoidObsMethState = STATE_FOLLOWOBS;//��һ�μ����ر�
		}
		else  //����Ѿ�û���ϰ�����
		{
			DEBUG_INFO ;
			setDesired(0,0);
			myVoidObsMethState = STATE_APF; //��һ�β����˹��Ƴ�
		}
	}
	else if(goalAngleInRobot  < 0) //�ϰ���������
	{
		if(isInObsRange(obsPose[5].dis) ||
				isInObsRange(obsPose[6].dis) ||
				isInObsRange(obsPose[7].dis))
		{
			
			if(isInObsRange(obsPose[5].dis))
			{
				DEBUG_INFO ;
				setDesired(-20,200); //��������ϰ��ﰲȫ����ֱ��ǰ��
			}
			else
				setDesired(0,200);
			myVoidObsMethState = STATE_FOLLOWOBS;//��һ�μ����ر�
		}
		else  //�ұ��Ѿ�û���ϰ�����
		{
			DEBUG_INFO ;
			setDesired(0,0);
			myVoidObsMethState = STATE_APF; //��һ�β����˹��Ƴ�
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



bool RobotPathPlan::isAroundGoal(void)//�ж��Ƿ��Ѿ�����Ŀ��
{
	if(myRobot->getPose().findDistanceTo(myGoal) < closeDist) //1��֮��
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

	if(speed_ == -1.0)  //Ĭ���ɽǶ����ٶ�
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
int RobotPathPlan::judgeDir()//��ʼ�رߣ��жϷ���
{
	if(goalAngleInRobot >= 0) 
	{
		if(isInObsRange(obsPose[1].dis) || isInObsRange(obsPose[2].dis))
			return 0;
		else
			return 1; //��ת
	}
	else if(goalAngleInRobot < 0)//��ת
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
        detectObsBySonar();                     //̽�ⳬ����
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
	
	if(newGoal)  //��һ���µĵ�
    	{
        	myStartAngle = myRobot->getPose().findAngleTo(myGoal); //�����������Ŀ������ �������X��н�
        	newGoal = false;
	}
	if(myState ==STATE_NO_GOAL)
	{
		setDesired(0,0);
        	return &myDesired;
	}
	if(isAroundGoal())  //������Ŀ����Χ
	{
		printf("Reach the goal\n");
	//	exit(0);
		myState = STATE_ACHIEVED_GOAL;
		myVoidObsMethState = STATE_NONE;
        	setDesired(0,0);
        	return &myDesired;
	}
	printf("--------------------�ָ���-------------------------------\n");
	detectObsBySonar();			//̽�ⳬ����
	transMapGoal2RobotGoal();		//��ͼת��

	//calcAngleByAPF();//����Ƕ�
	//return &myDesired;//���ؿ���


	if(myVoidObsMethState == STATE_PREPARE_FOLLOWOBS) //�Ѿ�׼�����ر�
	{
		DEBUG_INFO;
		dir = judgeDir();  //׼���رߵķ���
		if(1 == dir )
			setDesired(40,100);
		else if(dir == -1)
			setDesired(-40,100);  //�ʼѰ��Ŀ���Ĵ���
		else
			setDesired(0,50);

		myVoidObsMethState = STATE_FOLLOWOBS;//��ʼ�ر�
		return &myDesired;
	}
	if(myVoidObsMethState == STATE_FOLLOWOBS) //��һ��Ϊ�ر�
	{
		DEBUG_INFO;
		voidObsByFollowOs();  //����������Ѿ����ú��ٶȺ���һ�εĶ���
		/*����ת��APF����STATE_FOLLOWOBS*/
		return &myDesired;
	}
	else if(myVoidObsMethState == STATE_APF) //��һ��״̬ΪAPF
	{
		/*����ת��STATE_APF2STATE_FOLLOWOBS��STATE_APF*/
		if(!isMinPoint())//�Ǿֲ���С��
		{
			DEBUG_INFO;
			calcAngleByAPF();//����Ƕ�
		}
		else
		{
			DEBUG_INFO;
			setDesired(0,0);//�Ǿֲ���С����׼���ر�
			myVoidObsMethState = STATE_PREPARE_FOLLOWOBS;

		}
		return &myDesired;
	}
	else if(myVoidObsMethState == STATE_NONE)		//��һ����û����
	{   
		if(!isMinPoint())//���Ǿֲ���С��
		{
			DEBUG_INFO;
			calcAngleByAPF();//����Ƕ�

		}
		else
		{ 
			DEBUG_INFO;
			setDesired(0,0);
			myVoidObsMethState = STATE_PREPARE_FOLLOWOBS;
		}
		return &myDesired;
	}
	printf("�����ڵ����");
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
