#pragma once
#ifndef ROBOT_MAIN_FILESTREAM_H_
#define ROBOT_MAIN_FILESTREAM_H_


#include "IStream.h"


#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif


class IFile;

class RobotFileStream : public IStream
{
public:
	RobotFileStream(IFile& file);

	size_t IStreamRead(char *buf, size_t max_sz);

	void IStreamWrite(const char *buf, size_t sz);

private:
	IFile& m_file;
};

#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
#endif
