#include "RobotModeLock.h"
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
RobotModeLock::RobotModeLock():cond(), mode(MODE_JOYSTICK) {} /*默认为手柄，防止启动后机器人乱跑*/
RobotModeLock::~RobotModeLock() {}
#ifdef ROBOT_NAMESPACE
}
#endif

