#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "RobotFile.h"

#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif

RobotFile::RobotFile() : m_fil(NULL), m_b_close(true), m_rptr(0), m_wptr(0)
{
}


RobotFile::RobotFile(FILE *fil) : m_fil(fil), m_b_close(false), m_rptr(0) ,m_wptr(0)
{
}


RobotFile::RobotFile(const std::string& path, const std::string& mode) : m_fil(NULL), m_b_close(true), m_rptr(0), m_wptr(0)
{
	fopen(path, mode);
}


RobotFile::~RobotFile()
{
	if (m_b_close)
	{
		fclose();
	}
}


bool RobotFile::fopen(const std::string& path, const std::string& mode)
{
	m_path = path;
	m_mode = mode;
	m_fil = ::fopen(path.c_str(), mode.c_str());
	return m_fil ? true : false;
}


void RobotFile::fclose() const
{
	if (m_fil)
	{
		::fclose(m_fil);
		m_fil = NULL;
	}
}



size_t RobotFile::fread(char *ptr, size_t size, size_t nmemb) const
{
	size_t r = 0;
	if (m_fil)
	{
		fseek(m_fil, m_rptr, SEEK_SET);
		r = ::fread(ptr, size, nmemb, m_fil);
		m_rptr = ftell(m_fil);
	}
	return r;
}


size_t RobotFile::fwrite(const char *ptr, size_t size, size_t nmemb)
{
	size_t r = 0;
	if (m_fil)
	{
		fseek(m_fil, m_wptr, SEEK_SET);
		r = ::fwrite(ptr, size, nmemb, m_fil);
		m_wptr = ftell(m_fil);
	}
	return r;
}



char *RobotFile::fgets(char *s, int size) const
{
	char *r = NULL;
	if (m_fil)
	{
		fseek(m_fil, m_rptr, SEEK_SET);
		r = ::fgets(s, size, m_fil);
		m_rptr = ftell(m_fil);
	}
	return r;
}


void RobotFile::fprintf(const char *format, ...)
{
	if (!m_fil)
		return;
	va_list ap;
	va_start(ap, format);
	fseek(m_fil, m_wptr, SEEK_SET);
	vfprintf(m_fil, format, ap);
	m_wptr = ftell(m_fil);
	va_end(ap);
}


off_t RobotFile::size() const
{
	struct stat st;
	if (stat(m_path.c_str(), &st) == -1)
	{
		return 0;
	}
	return st.st_size;
}


bool RobotFile::eof() const
{
	if (m_fil)
	{
		if (feof(m_fil))
			return true;
	}
	return false;
}


void RobotFile::resetRead() const
{
	m_rptr = 0;
}


void RobotFile::resetWrite()
{
	m_wptr = 0;
}


const std::string& RobotFile::Path() const
{
	return m_path;
}



#ifdef ROBOT_NAMESPACE
}
#endif


