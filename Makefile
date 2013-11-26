SOURCE += RobotMutex.cpp
SOURCE += RobotCondition.cpp
SOURCE += RobotBasePacket.cpp
SOURCE += RobotPacket.cpp
SOURCE += RobotTask.cpp
SOURCE += RobotWheelTask.cpp
SOURCE += RobotLightTask.cpp
SOURCE += RobotThread.cpp
SOURCE += RobotDataQueue.cpp
SOURCE += RobotSerial.cpp
SOURCE += Robot.cpp
SOURCE += main.cpp   
SOURCE += RobotPathPlan.cpp
SOURCE += RobotJoyStick.cpp
SOURCE += RobotSocket.cpp
SOURCE += RobotSocketAddr.cpp
SOURCE += RobotSocketClient.cpp
SOURCE += RobotSocketOptions.cpp
SOURCE += RobotSocketService.cpp
SOURCE += RobotModeLock.cpp
SOURCE += RobotNetEpoll.cpp
SOURCE += RobotNetEpollOnce.cpp
SOURCE += RobotNetEpollAll.cpp
SOURCE += RobotSleep.cpp
SOURCE += RobotPacketExternal.cpp
SOURCE += RobotInclude.cpp
SOURCE += RobotMediator.cpp
SOURCE += ./Config/RobotConfig.cpp
SOURCE += RobotException.cpp
SOURCE += RobotUtility.cpp
SOURCE += StreamWriter.cpp
SOURCE += RobotFileStream.cpp
SOURCE += RobotFile.cpp
SOURCE += RobotSemaphore.cpp



HEADER += RobotTask.h
HEADER += RobotFunctor.h
#HEADER += AutoPtr.h
HEADER += RobotDataQueue.h
HEADER += RobotTaskQueue.h
HEADER += RobotSerial.h
HEADER += RobotBasePacket.h
HEADER += RobotMutex.h
HEADER += RobotPacket.h
HEADER += RobotCondition.h
HEADER += RobotThread.h
HEADER += Robot.h
HEADER += RobotDeBug.h
HEADER += RobotMath.h
HEADER += RobotCoorTrans.h
#HEADER += robot_enable_shared_from_this.h
#HEADER += robot_shared_ptr.h
#HEADER += robot_sp_counted.h
#HEADER += robot_volatile_long.h
#HEADER += robot_weak_ptr.h
HEADER += RobotPathPlan.h
HEADER += RobotDef.h
HEADER += RobotPacketExternal.h
HEADER += RobotUtility.h
HEADER += RobotException.h
HEADER += RobotSemaphore.h


TARGET = robot

#INCLUDEPATH += -I
#LIBPATH += -L

LIBS += -lpthread 
LIBS += -lm
#LIBS += -lmudflap

OPTION += -std=gnu++0x
OPTION += -Wall  
#OPTION += -Wextra 
#OPTION += -pedantic
OPTION += -rdynamic
OPTION += -g
OPTION += -DBUG
#OPTION += -fmudflap
CC=g++

OBJECTS =$(SOURCE:%.c=%.o)

$(TARGET):$(OBJECTS)
	$(CC) $(OPTION) $(INCLUDEPATH) $(LIBPATH) $(LIBS) -o $(TARGET) $(OBJECTS)
clean:
	-rm -f $(TARGET) *.o *~
