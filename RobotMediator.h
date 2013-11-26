#ifndef ROBOT_MAIN_MEDIATOR_H_
#define ROBOT_MAIN_MEDIATOR_H_

class Robot;
class RobotSerial;

#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif

class Mediator
{
public:
	void registRobot(Robot*);
	void registRobotSerial(RobotSerial*);
	void processData(const unsigned  char* arr, int len);
	void send(const unsigned char* arr, int len);
	void setRobotIsReady(bool b = true);

	void serialSuspend();
	void serialResume();
private:
	Robot *robot;
	RobotSerial *robotSerial;
};

#ifdef ROBOT_NAMESPACE
}
#endif
#endif
