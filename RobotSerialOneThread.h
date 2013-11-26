#ifndef __STTY_H__ 
#define __STTY_H__ 

//包含头文件
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <termios.h> 
#include <errno.h> 
#include <pthread.h> 
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
// 
// 串口设备信息结构 
typedef struct tty_info_t 
{ 
	int fd; // 串口设备ID 
	pthread_mutex_t mt; // 线程同步互斥对象 
	char name[24]; // 串口设备名称，例："/dev/ttyS0" 
	struct termios ntm; // 新的串口设备选项 
	struct termios otm; // 旧的串口设备选项 
} TTY_INFO; 
// 
// 串口操作函数 
TTY_INFO *readyTTY(int id); 
int setTTYSpeed(TTY_INFO *ptty, int speed); 
int setTTYParity(TTY_INFO *ptty,int databits,int parity,int stopbits); 
int cleanTTY(TTY_INFO *ptty); 
int sendnTTY(TTY_INFO *ptty,char *pbuf,int size); 
int recvnTTY(TTY_INFO *ptty,char *pbuf,int size); 
int lockTTY(TTY_INFO *ptty); 
int unlockTTY(TTY_INFO *ptty); 
#ifdef ROBOT_NAMESPACE
}
#endif
#endif 

