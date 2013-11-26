#pragma once
#ifndef ROBOT_MAIN_EXCEPTION_H_
#define ROBOT_MAIN_EXCEPTION_H_


#include <string>


#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif


class RobotException
{
public:
	RobotException(const std::string& description);
	virtual ~RobotException() {}

	virtual const std::string ToString() const;
	virtual const std::string Stack() const;

	RobotException(const RobotException& ) {}

	RobotException& operator=(const RobotException& ) { return *this; }

private:
	std::string m_description;
	std::string m_stack;

};



#ifdef ROBOT_NAMESPACE
} 
#endif

#endif

