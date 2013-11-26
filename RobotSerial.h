#pragma once
#ifndef ROBOT_MAIN_MULTSERIAL_H_
#define ROBOT_MAIN_MULTSERIAL_H_

/*
 * Power by RenZhenwen
 * Date:2012 - 12 -07
 * Email:admin@unix8.net
 * */
#include <pthread.h>
#include <errno.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
 #include <sys/time.h>
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
class Mediator;
class RobotSerial
{
private:
	Mediator *mediator;
	pthread_t tid_write_rs232;																	//线程ID
	pthread_t tid_read_rs232;
	//波特率 帧头，帧尾
	#define DS	(B115200)		
	#define BB  (CS8)
	#define RS232_START		(0xfb)
	#define RS232_END		(0xfe)
	//等待串口可读
	inline int _wait_fd_read_eable(int fd);		
	//等待串口可写
	inline int _wait_fd_write_eable(int fd);		
	//阻塞并等待信号
	inline void _block_cond_wait(pthread_cond_t *pcond, pthread_mutex_t *pmutex);
	inline void _block_cond_timedwait(pthread_cond_t *pcond, pthread_mutex_t *pmutex);
	void pthread_pause_location();
	//发送信号并通知
	void _send_cond_signal(pthread_cond_t *pcond);							
	//提取到完整帧后的解析函数
	void transMsg(unsigned char *msg, int len);									
	//以下3个static函数为线程的处理函数入口
	static void *_static_read_rs232(void *arg);									
	static void *_static_write_rs232(void *arg);
	static void *_static_test_sendcmd(void *arg);
public:
		typedef enum {																					//串口状态
		STATUS_CREATE_FAILED = 1, 
		STATUS_SELECT_FAILED,
		STATUS_OPEN_FAILED, 
		STATUS_SYSTEM_FAILED
	} SerialStatus;
	explicit RobotSerial(Mediator *, const char* serialName = NULL);
	~RobotSerial(void) throw();	
	void setSerial(const char* name) {strcpy(DeviceName, name);}
	void setFrame1Len(int l){frame1Len = l;}
	void setFrame2Len(int l){frame2Len = l;}

	//将数据写到串口
	inline void sendMsg(const unsigned  char arr[], int len);	
	//运行串口线程
	int run(void);
	//初始化并打开串口
	int openSerial(void);			
	//读循环队列线程
	void _read_rs232(void);
	//写循环队列线程
	void _write_rs232(void);

	void sendSerialIsReady();
	void waitReadyACK();
	void pthread_suspend(void);
	void pthread_resume(void);
private:
	int frame1Len;
	int frame2Len;
private:
//	static const int	 	TMP_LENGTH = 2048;
	static const int    MAX_TRY = 3;
	static const int    MAX_BUFF_SIZE = 2048;
	//循环队列
	unsigned char buf_rs232[MAX_BUFF_SIZE + 1];   
	/*64字节的临时存储*/
	char DeviceName[20] ;
	int fd_rs232;
	volatile int bempty_rs232;
	volatile int bfull_rs232;
	volatile int front_rs232, rear_rs232;	
	int DeviceSpeed; 				/* B38400 */
	int ByteBits;    				/* CS8     */
	pthread_mutex_t mutex_rs232_empty;
	pthread_mutex_t mutex_rs232_full;
	pthread_cond_t conlock_rs232_empty;
	pthread_cond_t conlock_rs232_full;

	pthread_mutex_t mutex_pause;
	pthread_cond_t cond_pause;
	volatile bool pthread_pause;

	struct termios TtyAttr;
	struct timespec ts;
	struct timeval tp;
};
inline void RobotSerial::_block_cond_wait(pthread_cond_t *pcond, pthread_mutex_t *pmutex)
{
	pthread_mutex_lock(pmutex);
	pthread_cond_wait(pcond, pmutex);
	pthread_mutex_unlock(pmutex);
}
inline void RobotSerial::_block_cond_timedwait(pthread_cond_t *pcond, pthread_mutex_t *pmutex)
{
	pthread_mutex_lock(pmutex);
	gettimeofday(&tp, NULL);
	ts.tv_sec  = tp.tv_sec;
  ts.tv_nsec = tp.tv_usec * 1000;
  ts.tv_sec += 1;
	pthread_cond_timedwait(pcond, pmutex, &ts);
	pthread_mutex_unlock(pmutex);
}
inline int RobotSerial::_wait_fd_write_eable(int fd) 
{
	fd_set wsets;
  FD_ZERO(&wsets);
 	FD_SET(fd, &wsets);
	if (select(fd + 1, NULL, &wsets, NULL, NULL) < 0)
	{
		return (STATUS_SELECT_FAILED);
	}
	return 0;
}
inline int RobotSerial::_wait_fd_read_eable(int fd)
{
  fd_set rsets;
  FD_ZERO(&rsets);
 	FD_SET(fd, &rsets);
	if (select(fd + 1, &rsets, NULL, NULL, NULL) < 0)
	{
		return (STATUS_SELECT_FAILED);
	}
	return 0;
}
inline void RobotSerial::sendMsg(const unsigned char arr[], int len)/*unsigned char,int*/
{
	_wait_fd_write_eable(fd_rs232);
	write(fd_rs232, arr, len);
}
#ifdef ROBOT_NAMESPACE
}
#endif
#endif
