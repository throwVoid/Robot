#pragma once
#ifndef ROBOT_MAIN_STREAMWRITER_H_
#define ROBOT_MAIN_STREAMWRITER_H_


#include <string>


#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif


class IStream;

class StreamWriter
{
public:
	StreamWriter(IStream& stream);
	virtual ~StreamWriter() {}

	StreamWriter& operator<<(const char *);
	StreamWriter& operator<<(const std::string&);
	StreamWriter& operator<<(short);
	StreamWriter& operator<<(int);
	StreamWriter& operator<<(long);
	StreamWriter& operator<<(double);

private:
	IStream& m_stream;

};


#ifdef ROBOT_NAMESPACE
}
#endif


#endif 
