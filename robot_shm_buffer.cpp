#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "robot_shm_buffer.h"
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
atu_shm_buffer::atu_shm_buffer(const char *name, size_t len, bool create)
: m_buf(NULL)
, m_len(len)
, m_name(name)
{
	assert(NULL != name);

	int ret(0);

	if (create)
	{
		m_fd = shm_open(name, O_RDWR | O_CREAT, 0644);

		m_sem = sem_open(name, O_CREAT, 0644, 1);
		if (SEM_FAILED == m_sem)
		{
			fprintf(stderr, "Failed to create semaphore %s!\n", name);
			return;
		}
		ret = ftruncate(m_fd, len);
		if (-1 == ret)
		{
			fprintf(stderr, "Failed to re-size shared memory buffer %s!\n", name);
			return;
		}
	}
	else
	{
		m_fd = shm_open(name, O_RDWR, 0644);
		m_sem = sem_open(name, O_RDWR);
		if (SEM_FAILED == m_sem)
		{
			fprintf(stderr, "Failed to create semaphore %s!\n", name);
			return;
		}
	}
	if (-1 == m_fd)
	{
		fprintf(stderr, "Failed to create shared memory buffer %s!\n", name);
		return;
	}

	ret = fstat(m_fd, &m_stat);
	if (-1 == ret)
	{
		fprintf(stderr, "Failed to get status of shared memory!\n");
		return;
	}
	m_len = m_stat.st_size;

	m_buf = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
	if (MAP_FAILED == m_buf)
	{
		perror("Failed to map shared memory to process!");
		return;
	}

	if (create)
	{
		memset(m_buf, 0, len);
	}
}

atu_shm_buffer::~atu_shm_buffer() {
	if (-1 != m_fd)
	{
		munmap(m_buf, m_len);
	}

}

void atu_shm_buffer::unlink(void)
{
	if (MAP_FAILED != m_buf)
	{
		sem_unlink(m_name.c_str());
		shm_unlink(m_name.c_str());
	}
}

void atu_shm_buffer::read(char *buffer, size_t len)
{
	if (len > m_len)
	{
		fprintf(stderr, "[read] Buffer is truncated! From %d to %d bytes.\n",
				(int)len, (int)m_len);
		len = m_len;
	}
	sem_wait(m_sem);
	memcpy((void *)buffer, m_buf, len);
	sem_post(m_sem);
}

void atu_shm_buffer::write(const void* buffer, size_t len)
{
	if (len > m_len)
	{
		fprintf(stderr, "[write] Buffer is truncated! From %d to %d bytes.\n",
				(int)len, (int)m_len);
		len = m_len;
	}
	sem_wait(m_sem);
	memcpy(m_buf, buffer, len);
	sem_post(m_sem);
}
#ifdef ROBOT_NAMESPACE
}
#endif

