#include <stdio.h>
#include <stdlib.h>
#ifndef _WIN32
#include <unistd.h>
#include <Sockets/Semaphore.h>
#include <Sockets/Mutex.h>
#include <Sockets/Thread.h>
#include <Sockets/Utility.h>
#include <Sockets/Lock.h>


class MyThread : public Sockets::Thread
{
public:
	void Run() {
		printf("Thread\n");
	}
};


class SemLock
{
public:
	SemLock(Sockets::Semaphore& sem) : m_sem(sem) {
		m_sem.Wait();
	}
	~SemLock() {
		m_sem.Post();
	}

private:
	Sockets::Semaphore& m_sem;
};


/**
 * Return time difference between two struct timeval's, in seconds
 * \param t0 start time
 * \param t	end time
 */
double Diff(struct timeval t0,struct timeval t)
{
	t.tv_sec -= t0.tv_sec;
	t.tv_usec -= t0.tv_usec;
	if (t.tv_usec < 0)
	{
		t.tv_usec += 1000000;
		t.tv_sec -= 1;
	}
	return t.tv_sec + (double)t.tv_usec / 1000000;
}


static	int val = 0;

void lock(Sockets::Mutex& m, int i)
{
	Sockets::Lock l(m);
	val += i;
}


void lock(Sockets::Semaphore& s, int i)
{
	SemLock l(s);
	val += i;
}
#endif // WIN32


int main()
{
#ifndef _WIN32
	Sockets::Mutex mutex;
	Sockets::Semaphore sema(1);
	struct timeval start;
	struct timeval tt;
	double d;

	Sockets::Utility::GetTime(&start);
	for (int i = 0; i < 100000; i++)
		lock(mutex, i);
	Sockets::Utility::GetTime(&tt);
	d = Diff(start, tt);
	printf("%.4f sec\n", d);

	Sockets::Utility::GetTime(&start);
	for (int i = 0; i < 100000; i++)
		lock(sema, i);
	Sockets::Utility::GetTime(&tt);
	d = Diff(start, tt);
	printf("%.4f sec\n", d);
#endif
}


