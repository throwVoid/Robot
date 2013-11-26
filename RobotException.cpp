#include "RobotException.h"
#include "RobotUtility.h"

#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif


RobotException::RobotException(const std::string& description) : m_description(description)
, m_stack(RobotUtility::Stack())
{
}


const std::string RobotException::ToString() const
{
	return m_description;
}


const std::string RobotException::Stack() const
{
	return m_stack;
}

#ifdef ROBOT_NAMESPACE
} 
#endif
