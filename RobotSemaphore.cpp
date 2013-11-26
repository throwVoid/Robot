#include "RobotSemaphore.h"


#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif

RobotSemaphore::RobotSemaphore(value_t start_val)
{
	sem_init(&m_sem, 0, start_val);
}


RobotSemaphore::~RobotSemaphore()
{
	sem_destroy(&m_sem);
}


int RobotSemaphore::Post()
{
	return sem_post(&m_sem);
}


int RobotSemaphore::Wait()
{
	return sem_wait(&m_sem);
}


int RobotSemaphore::TryWait()
{
	return sem_trywait(&m_sem);
}


int RobotSemaphore::GetValue(int& i)
{
	return sem_getvalue(&m_sem, &i);
}

#ifdef ROBOT_NAMESPACE
}
#endif
