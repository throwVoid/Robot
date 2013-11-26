#include "RobotMediator.h"
#include "Robot.h"
#include "RobotSerial.h"
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
void Mediator::registRobot(Robot* tRobot)
{
	robot = tRobot;
}
void Mediator::registRobotSerial(RobotSerial* serial)
{
	robotSerial = serial;
}
void Mediator::processData(const unsigned char* arr, int len)
{
	robot->processData(arr, len);
}
void Mediator::send(const unsigned char* arr, int len)
{
	robotSerial->sendMsg(arr, len);
}

void Mediator::setRobotIsReady(bool b)
{
	robot->setRobotDriverIsReady(b);
}
void Mediator::serialSuspend()
{
	robotSerial->pthread_suspend();
}
void Mediator::serialResume()
{
	robotSerial->pthread_resume();
}
#ifdef ROBOT_NAMESPACE
}
#endif
