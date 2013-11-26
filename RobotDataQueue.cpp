#include "RobotDataQueue.h"
/*
 * power by RenZhenwen
 * date:2012-12-07
 * */
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
RobotTaskPriorityQueue *RobotTaskPriorityQueue::_taskQueueAddr = NULL;
RobotSerialDataQueue* RobotSerialDataQueue::_serialDataQueueAddr = NULL;
RobotNetDataQueue* RobotNetDataQueue::_netDataQueueAddr = NULL;
#if 0
RobotSerialDataArray* RobotSerialDataArray::_robotSerialDataArray = NULL;
struct semLight RobotSerialDataArray::shared;
unsigned char* RobotSerialDataArray::serialDataArray = NULL;
RobotMutex RobotSerialDataArray::serialDataMutex;
#endif
#ifdef ROBOT_NAMESPACE
}
#endif

