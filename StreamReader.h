#pragma once
#ifndef ROBOT_MAIN_STREAMREADER_H_
#define ROBOT_MAIN_STREAMREADER_H_


#include <string>


#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif

class IStream;

class StreamReader
{
private:
	IStream& m_stream;
public:
	StreamReader(IStream& stream);
	virtual ~StreamReader() {}

	StreamReader& operator>>(char&);
	StreamReader& operator>>(std::string&);
	StreamReader& operator>>(short&);
	StreamReader& operator>>(int&);
	StreamReader& operator>>(long&);
	StreamReader& operator>>(double&);

};
#ifdef ROBOT_NAMESPACE
}
#endif


#endif 
