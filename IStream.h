#pragma once
#ifndef ROBOT_MAIN_ISTREAM_H_
#define ROBOT_MAIN_ISTREAM_H_


#include <string>
using namespace std;


#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif

class IStream
{
public:
	virtual ~IStream() {}

	//从源读取buf_sz字节，实际读取的字节数作为返回值
	virtual size_t IStreamRead(char *buf, size_t buf_sz) = 0;

	virtual void IStreamWrite(const char *buf, size_t sz) = 0;

};

#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif

#endif
