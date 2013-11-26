#ifndef ATU_SHM_BUFFER_H_
#define ATU_SHM_BUFFER_H_
#include <string>
#include <fcntl.h>
#include <semaphore.h>


#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
const u_int c_default_buffer_len = 256;

class atu_shm_buffer {
public:
	atu_shm_buffer(const char *name, size_t len = c_default_buffer_len, bool create = false);
	virtual ~atu_shm_buffer();
	inline const char * get_name(void) { return m_name.c_str(); }
	inline size_t get_len(void) { return m_len; }

	void read(char *buf, size_t len);
	void write(const void *content, size_t len);
	void unlink(void);
private:
	void *m_buf;
	size_t m_len;
	int m_fd;
	std::string m_name;

	sem_t *m_sem;
	struct stat m_stat;
};
#ifdef ROBOT_NAMESPACE
}
#endif
#endif /* ATU_SHM_BUFFER_H_ */
