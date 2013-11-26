#include "RobotSerialOneThread.h"

int main(int argc,char **argv) 
{ 
	TTY_INFO *ptty; 
	int nbyte,idx; 
	char cc[37]; 
	
	ptty = readyTTY(0); 
	if(ptty == NULL) 
	{ 
		printf("readyTTY(0) error\n"); 
		return 1; 
	} 
	// 
	// 
	lockTTY(ptty); 
	if(setTTYSpeed(ptty,115200)>0) 
	{ 
		printf("setTTYSpeed() error\n"); 
		return -1; 
	} 
	if(setTTYParity(ptty,8,'N',1)>0) 
	{ 
		printf("setTTYParity() error\n"); 
		return -1; 
	} 
	// 
	idx = 0; 
	while(1) 
	{ 
	//	cc[0] = 0xFA; 
		//sendnTTY(ptty,&cc[0],1); 
		nbyte = recvnTTY(ptty,cc,10); 
		for(int i =0 ;i < nbyte;++i)
		{
			printf("%d:%02X\n",i,cc[i]); 
		}
	} 
	
	cleanTTY(ptty); 
	
} 
