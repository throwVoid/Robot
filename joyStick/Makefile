#You can add source code file like the following
SOURCE += my_joystick.c   
SOURCE += RobotSocket.cpp
SOURCE += RobotSocketAddr.cpp
SOURCE += RobotSocketClient.cpp
SOURCE += RobotSocketOptions.cpp
SOURCE += RobotSocketService.cpp
SOURCE += listop.c

##You can add header code file like the following
HEADER += RobotFunctor.h
HEADER += listop.h
HEADER += RobotSocket.h


#You can name the executable file like the following
TARGET = client

#You can add include path like the following
#INCLUDEPATH += -I/usr/local/Aria/include

#You can library path like the following
#LIBPATH += -L/usr/local/Aria/lib

#You can add library like the following
LIBS += -lpthread 
#LIBS += -ldl 
#LIBS += -lAria 
#LIBS += -lArNetworking
#LIBS += -lm

#You cant add compile options like the following
OPTION += -Wall  
#OPTION += -g
#OPTION += -pthread

#You can specify the compiler
CC=g++

#Don't modify the following code unless you know howto exactly
OBJECTS =$(SOURCE:%.c=%.o)

$(TARGET):$(OBJECTS)
	$(CC) $(OPTION) $(INCLUDEPATH) $(LIBPATH) $(LIBS) -o $(TARGET) $(OBJECTS)
clean:
	-rm -f $(TARGET) *.o *~

