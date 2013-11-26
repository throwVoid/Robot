#include"RobotBasePacket.h"
#include <stdio.h>
#include<string.h>
/*
 * Power by RenZhenwen
 * Date:2012-12-07
 * Email:admin@unix8.net
 * */

//外部传进来的缓冲指针或NULL
//缓冲区最大长度，也是包的最大长度
//头部长度，一般为一个字节
//尾部长度，一般为一个字节
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
RobotBasePacket::RobotBasePacket(unsigned char * buf,						
								 unsigned short bufferSize, 			
                                 unsigned short headerLength,			
                                 unsigned short footerLength) 	  
{
	//如果没有设置外部地址，则该类自动申请内存 
	if (buf == NULL && bufferSize > 0)   
	{
		//使用类内部自己new的空间
		myOwnMyBuf = true;						
		#ifdef EXCEPTION_SAFE
		try
		{
		#endif
			myBuf = new unsigned char[bufferSize];
		#ifdef EXCEPTION_SAFE
		}
		catch(std::bad_alloc &ba)
		{
			if(myBuf != NULL)
			{
				delete myBuf;
				myBuf = NULL;
			}
		}
		#endif
	} 
	else 
	{
		//使用类外部内存空间
		myOwnMyBuf = false;   
		//将外部空间的地址赋值给内的成员
		myBuf = buf;					
	}
	//头部长度
	myHeaderLength = headerLength;	
	//尾部长度
	myFooterLength = footerLength;	
	//当前读指针索引所在位置
	myReadLength = myHeaderLength;	
	//缓冲区最大长度
	myMaxLength = bufferSize;			
	//当前仅有头部字节，还未加入尾部，在计算校验和以后再加入	
	myLength = myHeaderLength;			
	//可用性测试
	myIsValid = true;								
}

RobotBasePacket::~RobotBasePacket(void) throw()
{
	// 如果是类内部申请的内存空间，则交给类的析构函数来释放
	if (myOwnMyBuf && myBuf != NULL)			
		delete[] myBuf, myBuf = NULL;
}


//重新设置缓冲区（指向外部的地址空间）
void RobotBasePacket::setBuf(unsigned char *buf, unsigned short bufferSize)
{
	if (myOwnMyBuf)
	{
		delete[] myBuf;
		myBuf = NULL;
		myOwnMyBuf = false;
	} 
	myBuf = buf;
	/*2013-02-28添加*/
	myReadLength = myHeaderLength;
	myLength = myHeaderLength;
	/*end*/
	myMaxLength = bufferSize;
}

//重新设置缓冲区最大长度
//没有将原来缓冲区的数据拷贝到新缓冲区（注意）
void RobotBasePacket::setMaxLength(unsigned short bufferSize)
{
	//为了已有数据的安全，新的长度不能小于旧长度
	if (myMaxLength >= bufferSize) 
		return;
	if (myOwnMyBuf) 
	{
		delete[] myBuf;
		myBuf = NULL;
		myOwnMyBuf = false;
	} 
	#ifdef EXCEPTION_SAFE
	try
	{
	#endif
		myBuf = new unsigned char[bufferSize];
	#ifdef EXCEPTION_SAFE
	}
	catch(std::bad_alloc &ba)
	{
			if(myBuf != NULL)
			{
				delete myBuf;
				myBuf = NULL;
			}
	}
	#endif
	myMaxLength = bufferSize;
	myOwnMyBuf = true;
}

//设置帧的现有总长度
bool RobotBasePacket::setLength(unsigned short length)
{
	if (myOwnMyBuf && length > myMaxLength)
			return false;

	myLength = length;
	return true;
}
//设置读索引的位置
void RobotBasePacket::setReadLength(unsigned short readLength)				
{
	myReadLength = readLength;
}

 bool RobotBasePacket::setHeaderLength(unsigned short length)
{
	if (myOwnMyBuf && length > myMaxLength)
		return false;
	myHeaderLength = length;
	return true;
}

 //复位读索引
 void RobotBasePacket::resetRead(void)			
{
	myReadLength = myHeaderLength;
	resetValid();
}

 //清空数据包，此时数据帧只含头部
void RobotBasePacket::empty(void)			
{
	myLength = myHeaderLength;
	/*2013-02-28添加*/
	resetRead();
	/*end*/
	//resetValid();
}

bool RobotBasePacket::isNextGood(int bytes)
{
	if (bytes <= 0)
		return false;
	//保证在有效范围内读数据
	if (myReadLength + bytes <= myLength )
	{
		return true;
		/*printf("验证通过，可以写入");*/
	}
	myIsValid = false;
	return false;
}

//测试是否还有写的空间
bool RobotBasePacket::hasWriteCapacity(int bytes)
{
	if (bytes < 0) 
	{
		return false;
	}
	if ((myLength + bytes) <= myMaxLength) 
	{
		return true;
	}
	myIsValid = false;
	return false;
}


//测试帧是否可用，可以调用empty()和resetRead()来复位重置有效状态
bool RobotBasePacket::isValid(void)
{
	return myIsValid;
}

//强制有效，数据可能被截断，一般不要用
void RobotBasePacket::resetValid(void)  
{
	myIsValid = true;
}
#if 0
//该函数已经移至内联函数
/*const*/ unsigned char *RobotBasePacket::getBuf(void)/*返回缓冲头部指针*/
{
	return myBuf;
}
#endif


void RobotBasePacket::byteToBuf(char val)
{
	if (!hasWriteCapacity(1)) 
	{
		return;
	}
	memcpy(myBuf + myLength, &val, 1);
	myLength += 1;
}
//unsigned short存入缓冲区
void RobotBasePacket::byte2ToBuf(short val)
{
	if (!hasWriteCapacity(2)) 
	{
		return;
	}
	unsigned char c;
	c = (val >> 8) & 0xff;
//	memcpy(myBuf+myLength+1, &c, 1);
	memcpy(myBuf + myLength, &c, 1);
	c = val & 0xff;
//	memcpy(myBuf+myLength, &c, 1);
	memcpy(myBuf + myLength + 1, &c, 1);
	myLength += 2;
}

void RobotBasePacket::byte4ToBuf(int val)
{
	if (!hasWriteCapacity(4)) 
	{
		return;
	}

	unsigned char c;
	c = (val >> 24) & 0xff;
//	memcpy(myBuf+myLength+3, &c, 1);
	memcpy(myBuf + myLength, &c, 1);
	c = (val >> 16) & 0xff;
//	memcpy(myBuf+myLength+2, &c, 1);
	memcpy(myBuf + myLength + 1, &c, 1);
	c = (val >> 8) & 0xff;
//	memcpy(myBuf+myLength+1, &c, 1);
	memcpy(myBuf + myLength + 2, &c, 1);
	c = val & 0xff;
//	memcpy(myBuf+myLength, &c, 1);
	memcpy(myBuf + myLength + 3, &c, 1);
	myLength += 4;
}

void RobotBasePacket::uByteToBuf(unsigned char val)
{
	if (!hasWriteCapacity(1)) 
	{
		return;
	}
	memcpy(myBuf + myLength, &val, 1);
	myLength += 1;
}

void RobotBasePacket::uByte2ToBuf(unsigned short val)
{
	if (!hasWriteCapacity(2)) 
	{
		return;
	}
	unsigned char c;
	c = (val >> 8) & 0xff;
//	memcpy(myBuf+myLength+1, &c, 1);
	memcpy(myBuf + myLength, &c, 1);
	c = val & 0xff;
//	memcpy(myBuf+myLength, &c, 1);
	memcpy(myBuf + myLength + 1, &c, 1);
	myLength += 2;
}

 void RobotBasePacket::uByte4ToBuf(unsigned int val)
{
	if (!hasWriteCapacity(4)) 
	{
		return;
	}
	memcpy(myBuf + myLength, &val, 4);
	myLength += 4;
}



void RobotBasePacket::dataToBuf(const char *data, int length) //char追加
{
	if (data == NULL)
	{
		return;
	}
	if (!hasWriteCapacity(length)) 
	{
		return;
	}
	memcpy(myBuf + myLength, data, length);
	myLength += length;
}


void RobotBasePacket::dataToBuf(const unsigned char *data, int length)//unsigned char追加
{
	if (data == NULL) 
	{
		return;
	}
	if (!hasWriteCapacity(length)) 
	{
		return;
	}
	memcpy(myBuf + myLength, data, length);
	myLength += length;
	//printf("[%s] [%d] [%d] \n",__FILE__,__LINE__,myLength);
}

//转换成char
char RobotBasePacket::bufToByte(void)
{
	char ret = 0;
	if (isNextGood(1))
	{
		memcpy(&ret, myBuf + myReadLength, 1);
		myReadLength += 1;
	}
	return(ret);
}

short RobotBasePacket::bufToByte2(void)//转换成short
{
	short ret = 0;
	unsigned char c1, c2;
	if (isNextGood(2))
	{
		/*memcpy(&c1, myBuf+myReadLength, 1);
		memcpy(&c2, myBuf+myReadLength+1, 1);
		ret = (c1 & 0xff) | (c2 << 8);
		myReadLength+=2;*/
		memcpy(&c1, myBuf + myReadLength, 1);
		memcpy(&c2, myBuf + myReadLength + 1, 1);
		ret = (c2 & 0xff) | (c1 << 8);
		myReadLength += 2;
	}
	return ret;
}

int RobotBasePacket::bufToByte4(void) //转换成int
{
	int ret = 0;
	unsigned char c1, c2, c3, c4;
	if (isNextGood(4))
	{
		memcpy(&c1, myBuf + myReadLength, 1);
		memcpy(&c2, myBuf + myReadLength+1, 1);
		memcpy(&c3, myBuf + myReadLength+2, 1);
		memcpy(&c4, myBuf + myReadLength+3, 1);
		//ret = (c1 & 0xff) | (c2 << 8) | (c3 << 16) | (c4 << 24);
		ret = (c4 & 0xff) | (c3 << 8) | (c2 << 16) | (c1 << 24);
		myReadLength += 4;
	}
	return ret;
}


//从当前read索引起1个字节转换成char
unsigned char RobotBasePacket::bufToUByte(void)
{
	char ret = 0;
	if (isNextGood(1))
	{
		memcpy(&ret, myBuf + myReadLength, 1);
		myReadLength += 1;
	}
	return(ret);
}
 //从当前read索引起2个字节转换成unsigned short
unsigned short RobotBasePacket::bufToUByte2(void) 
{
	unsigned short ret = 0;
	unsigned char c1, c2;
	if (isNextGood(2))
	{
		memcpy(&c1, myBuf + myReadLength, 1);
		memcpy(&c2, myBuf + myReadLength + 1, 1);
		//ret = (c1 & 0xff) | (c2 << 8);
		ret = (c2 & 0xff) | (c1 << 8);
		myReadLength += 2;
	}
	return ret;
}
//从当前read索引起4个字节转换成无符号整形unsigned int，用作将传感器数据或者其他数据转换成整数
unsigned int RobotBasePacket::bufToUByte4(void) 
{
	int ret = 0;
	unsigned char c1, c2, c3, c4;
	if (isNextGood(4))
	{
		memcpy(&c1, myBuf + myReadLength, 1);
		memcpy(&c2, myBuf + myReadLength + 1, 1);
		memcpy(&c3, myBuf + myReadLength + 2, 1);
		memcpy(&c4, myBuf + myReadLength + 3, 1);
		//ret = (c1 & 0xff) | (c2 << 8) | (c3 << 16) | (c4 << 24);
		ret = (c4 & 0xff) | (c3 << 8) | (c2 << 16) | (c1 << 24);
		myReadLength += 4;
	}
	return ret;
}

//从read索引读取length字节到data
void RobotBasePacket::bufToData(char *data, int length)
{
	if (data == NULL) 
	{
		return;
	}
	if (isNextGood(length))
	{
		memcpy(data, myBuf + myReadLength, length);
		myReadLength += length;
	}
}


//对包的数据段追加，提高效率
void RobotBasePacket::bufToData(unsigned char *data, int length)
{
	if (data == NULL) 
	{
		return;
	}
	if (isNextGood(length))
	{
		//printf("这里进行了包可写测试，测试正确\n");
		memcpy(data, myBuf + myReadLength, length);
		myReadLength += length;
	}
	//printf("这里进行了包可写测试，测试异常\n");
}

//包的复制
void RobotBasePacket::dupPacket(RobotBasePacket *packet)
{
	myLength = packet->getLength();
	myReadLength = packet->getReadLength();
	memcpy(myBuf, packet->getBuf(), myLength);
}

/*打印数据帧*/
void RobotBasePacket::printHex(void)
{
	int i;
	printf("数据长度为 %d\n",myLength);
	for (i = 0; i < myLength; i++)
		printf("[%i] 0x%x\n", i,myBuf[i]);
	printf("\n");
}
#ifdef ROBOT_NAMESPACE
}
#endif
