/*
 *Power by RenZhenwen
 *Date:2012-12-07
 *Email:admin@unix8.net
 * */
#include "RobotMediator.h"
#include "RobotSerial.h"
#include "RobotDeBug.h"

#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif

RobotSerial::RobotSerial(Mediator* media, const char *serialName) : mediator(media)
{
	front_rs232 = 0;
	rear_rs232 = 0;
	bempty_rs232 = 1;
	bfull_rs232 = 0;
	pthread_pause = false;
	if(serialName)
		strcpy(DeviceName, serialName);
	pthread_mutex_init(&mutex_rs232_empty, NULL);
	pthread_mutex_init(&mutex_rs232_full, NULL);
	pthread_cond_init(&conlock_rs232_empty, NULL);
	pthread_cond_init(&conlock_rs232_full, NULL);
	pthread_mutex_init(&mutex_pause, NULL);
	pthread_cond_init(&cond_pause, NULL);
}

RobotSerial::~RobotSerial(void) throw()
{
  if(fd_rs232 >= 0)
	  close(fd_rs232);
	pthread_mutex_destroy(&mutex_rs232_empty);
	pthread_mutex_destroy(&mutex_rs232_full);
	pthread_cond_destroy(&conlock_rs232_empty);
	pthread_cond_destroy(&conlock_rs232_full);
	pthread_mutex_destroy(&mutex_pause);
	pthread_cond_destroy(&cond_pause);
}
int  RobotSerial::openSerial(void)
{
	int i;
	DeviceSpeed = DS;
	ByteBits = BB;
	for(i = 0; i < MAX_TRY; i++)
	{
	   fd_rs232 = open( DeviceName, O_RDWR|O_NONBLOCK |O_NOCTTY);
		if (fd_rs232 < 0)
		{
			PRINT("->串口打开失败，2秒后再试<-");
			sleep(2);
			continue ;
		}
		break;
	}
	if ((i == MAX_TRY) && fd_rs232 < 0)
	{
		PRINT("->串口打开失败<-");
		return (STATUS_OPEN_FAILED);
	}
	if (fcntl(fd_rs232, F_SETFL, O_NONBLOCK) < 0)
	{
		PRINT("串口设置阻塞失败");
		return (STATUS_SYSTEM_FAILED);
	}
	memset(&TtyAttr, 0, sizeof(struct termios));
	TtyAttr.c_iflag = IGNPAR;
	TtyAttr.c_cflag = DeviceSpeed | HUPCL | ByteBits | CREAD | CLOCAL;
	TtyAttr.c_cc[VMIN] = 1;
	if (tcsetattr(fd_rs232, TCSANOW, &TtyAttr) < 0)
	{
		PRINT("串口设置属性失败");
		return (STATUS_SYSTEM_FAILED);
	}
	PRINT("-->打开串口成功<--");
	return 0;
}
void RobotSerial::pthread_pause_location()
{
	while(pthread_pause)
	{
		pthread_mutex_lock(&mutex_pause);
		PRINT("线程已经锁住");
		pthread_cond_wait(&cond_pause, &mutex_pause);
		pthread_mutex_unlock(&mutex_pause);
		PRINT("线程已经唤醒");
	}
	
}
void RobotSerial::pthread_suspend(void)
{
	if(!pthread_pause)
	{
		pthread_mutex_lock(&mutex_pause);
		pthread_pause = true;
		PRINT("发送线程挂起");
		pthread_mutex_unlock(&mutex_pause);
	}
}
void RobotSerial::pthread_resume(void)
{
	if(pthread_pause)
	{
		pthread_mutex_lock(&mutex_pause);
		front_rs232 = 0;
		rear_rs232 = 0;
		bempty_rs232 = 1;
		bfull_rs232 = 0;
		pthread_pause = false;
		tcflush(fd_rs232, TCIOFLUSH);
		pthread_cond_broadcast(&cond_pause);
		sleep(1);
		pthread_cond_broadcast(&cond_pause);
		PRINT("发送线程唤醒");
		pthread_mutex_unlock(&mutex_pause);
	}
}
void RobotSerial::_read_rs232(void) 
{
	pthread_detach(pthread_self());
	unsigned char buff[MAX_BUFF_SIZE+ 1];
	unsigned char tmp;
	volatile int cnt, bstart;
	bstart = cnt = 0;
	for (;;)
	{
		pthread_pause_location();
		//有数据可读
		if (front_rs232 != rear_rs232)
		{
			//已经找到一帧的头部
			if (bstart)
			{
				//从循环队列中读数据
				buff[cnt++] = buf_rs232[front_rs232];
				//更新头部索引
				front_rs232 = (front_rs232 + 1) % MAX_BUFF_SIZE;
				//
				if(bfull_rs232)
				{
					bfull_rs232 = 0;
					pthread_cond_signal(&conlock_rs232_full);
				}
				if(cnt > frame1Len)  
				{
					bstart = 0;
					cnt = 0;
					continue;
				} 
				//检测到尾部
				if (buff[cnt - 1] == RS232_END)
				{
					//PRINT("--[长度为][%d]--->机器人收到一个完整的数据包控制帧<--------\n", cnt);
					//transMsg(buff, cnt);
					//printf("\n");
          if((cnt == frame1Len) /*|| (cnt == frame2Len)*/)
						mediator->processData(buff, cnt);
					bstart = 0;
					cnt = 0;
				}//end for if(buff[cnt-1]==RS232_END )
				else if(buff[cnt - 1] == RS232_START)
				{
					bstart = 1;
					buff[0] = RS232_START;
					continue;
				}

			}//end for if (bstart)
			else
			{
				//首先把头部读出来判断
				tmp = buf_rs232[front_rs232];
				//更新头部索引
				front_rs232 = (front_rs232 + 1) % MAX_BUFF_SIZE;
				//读走了一个数据，如果先前为满，则应该通知生产者继续生产	
				if(bfull_rs232)
				{
					//已经不满
					bfull_rs232 = 0;
					//通知
					//printf("通知缓冲区已经不满\n");
					pthread_cond_signal(&conlock_rs232_full);
				}
				
				if (tmp == RS232_START)
				{
					bstart = 1;
					buff[cnt++] = tmp;
				}
				//else 丢弃还没检查到开始标志前的所有数据
			}
		}//end for	if (front_rs232 != rear_rs232)
		else
		{
			//为空
			bempty_rs232 = 1;
			//等待
			//while(bempty_rs232 ==1)
			//{
				bfull_rs232 = 0;
			//	printf("缓冲区为空，等待中\n");
				_block_cond_wait(&conlock_rs232_empty, &mutex_rs232_empty);
			//	printf("缓冲区为空，等待结束\n");
			//}
		}
	}
}


void RobotSerial::transMsg(unsigned char *msg, int len)
{
	int i;
	for (i = 0; i < len; ++i)
		printf("%2x ", msg[i]);
}


void RobotSerial::_write_rs232(void)
{
	pthread_detach(pthread_self());
	unsigned char buff[MAX_BUFF_SIZE+ 1];
	int n;
	volatile int rear;
	int i;
	for (;;)
	{
		pthread_pause_location();
		//等待描述符可用
		_wait_fd_read_eable(fd_rs232);
		//读串口
		n = read(fd_rs232, buff,MAX_BUFF_SIZE);
		if(n > 0)
		{	
			for (i = 0; i < n; ++i)
			{
				rear = rear_rs232;
				//先试探下一个位置
				rear = (rear + 1) % MAX_BUFF_SIZE;
				//下一个位置不为头索引，则缓冲区还未满，可以填入数据
				if (rear != front_rs232)
				{
					//数据填入
					buf_rs232[rear_rs232] = buff[i];
					//更新尾部索引
					rear_rs232 = rear;
					//如果先前缓冲区为空，则信号通知读取循环队列的线程读取数据
					if(bempty_rs232)
					{
						//此时串口缓冲区不为空
						bempty_rs232 = 0;
						//通知
					//	printf("通知循环队列缓冲区有数据\n");
						pthread_cond_signal(&conlock_rs232_empty);
					}
				}//end for if   处理缓冲区为满的情况
				else
				{
					//缓冲区满，不能写入，故回退，等待下一次再写入
					i--;
					//更新满状态
					bfull_rs232 = 1;
					//休眠等待读取线程读取数据，直到有空闲区
					//while(bfull_rs232 == 1)
				//	{
						bempty_rs232 = 0;
					//	printf("缓冲区满\n");
						_block_cond_timedwait(&conlock_rs232_full, &mutex_rs232_full);
						pthread_cond_signal(&conlock_rs232_empty);
				//	}
				} // end for else
			}//end for for (i = 0; i < n; ++i)
		}//end for if(n > 0)
		else
		{
			//	return (STATUS_SYSTEM_FAILED);
		}
	}
}

void *RobotSerial::_static_read_rs232(void *arg)
{
	RobotSerial *f=static_cast<RobotSerial *>(arg);
	f->_read_rs232();
	return NULL;
}
void *RobotSerial::_static_write_rs232(void *arg)
{
	RobotSerial *f=static_cast<RobotSerial *>(arg);
	f->_write_rs232();
	return NULL;
}
int RobotSerial::run(void)
{
	int err;
	err = pthread_create(&tid_read_rs232, NULL,_static_read_rs232, this);
	if (err != 0)
	{
		return (STATUS_CREATE_FAILED);
	}
	err = pthread_create(&tid_write_rs232, NULL, _static_write_rs232, this);
	if (err != 0)
	{
		return (STATUS_CREATE_FAILED);
	}
//	pthread_join(tid_write_rs232, NULL);
//	pthread_join(tid_test_sendcmd, NULL);
	return 0;
}

void RobotSerial::sendSerialIsReady()
{
	unsigned char ready[5] = {0xfb, 0x03, 0x02,0x00, 0xfe};
	sendMsg(ready, 5);
	mediator->setRobotIsReady();
}
void RobotSerial::waitReadyACK()
{
		/*
		_wait_fd_read_eable(fd_rs232);
		int n = read(fd_rs232, buff, TMP_LENGTH);
		if((n == 5 ) && (buff[2] == 0x02))
			break;
			*/
}
#ifdef ROBOT_NAMESPACE
}
#endif

