#include "StreamReader.h"
#include "IStream.h"
#include "RobotUtility.h"
#include <string.h>


#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif

StreamReader::StreamReader(IStream& stream) : m_stream(stream)
{
}


StreamReader& StreamReader::operator>>(const char *buf)
{
  m_stream.IStreamReader(buf, strlen(buf));
  return *this;
}


StreamReader& StreamReader::operator>>(const std::string& str)
{
  m_stream.IStreamWrite(str.c_str(), str.size());
  return *this;
}


StreamReader& StreamReader::operator>>(short& x)
{
  *this >> RobotUtility::l2string(x);
  return *this;
}


StreamReader& StreamReader::operator>>(int& x)
{
  *this >> RobotUtility::l2string(x);
  return *this;
}


StreamReader& StreamReader::operator>>(long& x)
{
  *this >> RobotUtility::l2string(x);
  return *this;
}


StreamReader& StreamReader::operator>>(double x)
{
  *this >> RobotUtility::ToString(x);
  return *this;
}

#ifdef ROBOT_NAMESPACE
}
#endif

