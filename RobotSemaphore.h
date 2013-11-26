#pragma once
#ifndef ROBOT_MAIN_ROBOTSEMAPHORE_H_
#define ROBOT_MAIN_ROBOTSEMAPHORE_H_

#include <pthread.h>
#include <semaphore.h>

#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif


typedef unsigned int value_t;

class RobotSemaphore
{
public:
	RobotSemaphore(value_t start_val = 0);
	~RobotSemaphore();

	int Post();

	int Wait();

	int TryWait();

	int GetValue(int&);

private:
	RobotSemaphore(const RobotSemaphore& ) {}
	RobotSemaphore& operator=(const RobotSemaphore& ) { return *this; }
	sem_t m_sem;
};


#ifdef ROBOT_NAMESPACE
}
#endif


#endif 

