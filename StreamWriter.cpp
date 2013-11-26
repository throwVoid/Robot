#include "StreamWriter.h"
#include "IStream.h"
#include "RobotUtility.h"
#include <string.h>


#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif


StreamWriter::StreamWriter(IStream& stream) : m_stream(stream)
{
}


StreamWriter& StreamWriter::operator<<(const char *buf)
{
  m_stream.IStreamWrite(buf, strlen(buf));
  return *this;
}


StreamWriter& StreamWriter::operator<<(const std::string& str)
{
  m_stream.IStreamWrite(str.c_str(), str.size());
  return *this;
}


StreamWriter& StreamWriter::operator<<(short x)
{
  *this << RobotUtility::l2string(x);
  return *this;
}


StreamWriter& StreamWriter::operator<<(int x)
{
  *this << RobotUtility::l2string(x);
  return *this;
}


StreamWriter& StreamWriter::operator<<(long x)
{
  *this << RobotUtility::l2string(x);
  return *this;
}


StreamWriter& StreamWriter::operator<<(double x)
{
  *this << RobotUtility::ToString(x);
  return *this;
}

#ifdef ROBOT_NAMESPACE
}
#endif

