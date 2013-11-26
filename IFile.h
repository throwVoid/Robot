#pragma once
#ifndef ROBOT_MAIN_IFILE_H_
#define  ROBOT_MAIN_IFILE_H_

#include "robot-config.h"
#include <string>
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif


class IFile
{
public:
	virtual ~IFile() {}

	virtual bool fopen(const std::string&, const std::string&) = 0;
	virtual void fclose() const = 0;

	virtual size_t fread(char *, size_t, size_t) const = 0;
	virtual size_t fwrite(const char *, size_t, size_t) = 0;

	virtual char *fgets(char *, int) const = 0;
	virtual void fprintf(const char *format, ...) = 0;

	virtual off_t size() const = 0;
	virtual bool eof() const = 0;

	virtual void resetRead() const = 0;
	virtual void resetWrite() = 0;

	virtual const std::string& Path() const = 0;
};


#ifdef ROBOT_NAMESPACE
}
#endif

#endif 

