


class CircularBuffer
{
public:
	CircularBuffer(size_t size);
	~CircularBuffer();

	//增加l字节到循环队列
	bool Write(const char *p,size_t l);
	//读取l字节到指定地址
	bool Read(char *dest,size_t l);
	//删除l字节
	bool Remove(size_t l);
	//读取l字节，以字符串形式返回
	std::string ReadString(size_t l);

	//总缓冲区长度
	size_t GetLength();
	//缓冲区头部指针
	const char *GetStart();
	/** return number of bytes from circular buffer beginning to buffer physical end */
	size_t GetL();
	//缓冲区剩余长度
	size_t Space();

	//写到缓冲区的总字节数
	unsigned long ByteCounter(bool clear = false);

private:
	CircularBuffer(const CircularBuffer& s){}
	CircularBuffer& operator=(const CircularBuffer& ) { return *this; }
	char *buf;
	size_t m_max;
	size_t m_q;
	size_t m_b;
	size_t m_t;
	unsigned long m_count;
	};

