#pragma once
#ifndef ROBOT_MAIN_FILE_H_
#define  ROBOT_MAIN_FILE_H_

#include "robot-config.h"
#include "IFile.h"
#include <stdio.h>

#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif


class RobotFile : public IFile
{
public:
	RobotFile();
	RobotFile(FILE *);
	RobotFile(const std::string& path, const std::string& mode);
	~RobotFile();

	bool fopen(const std::string& path, const std::string& mode);
	void fclose() const;

	size_t fread(char *, size_t, size_t) const;
	size_t fwrite(const char *, size_t, size_t);

	char *fgets(char *, int) const;
	void fprintf(const char *format, ...);

	off_t size() const;
	bool eof() const;

	void resetRead() const;
	void resetWrite();

	const std::string& Path() const;

private:
	RobotFile(const RobotFile& ) {}
	RobotFile& operator=(const RobotFile& ) { return *this; }

	std::string m_path;
	std::string m_mode;
	mutable FILE *m_fil;
	bool m_b_close;
	mutable long m_rptr;
	long m_wptr;
};




#ifdef ROBOT_NAMESPACE
}
#endif

#endif

