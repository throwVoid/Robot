#include "RobotFileStream.h"
#include "IFile.h"


#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif

RobotFileStream::RobotFileStream(IFile& file) : m_file(file)
{
}


size_t RobotFileStream::IStreamRead(char *buf, size_t max_sz)
{
  return m_file.fread(buf, 1, max_sz);
}


void RobotFileStream::IStreamWrite(const char *buf, size_t sz)
{
  m_file.fwrite(buf, 1, sz);
}

#ifdef ROBOT_NAMESPACE
}
#endif
