

CircularBuffer::CircularBuffer(size_t size) :buf(new char[2 * size])
,m_max(size)
,m_q(0)
,m_b(0)
,m_t(0)
,m_count(0)
{
}


CircularBuffer::~CircularBuffer()
{
	delete[] buf;
}


bool CircularBuffer::Write(const char *s,size_t l)
{
	if (m_q + l > m_max)
	{
		return false;
	}
	m_count += (unsigned long)l;
	if (m_t + l > m_max) // block crosses circular border
	{
		size_t l1 = m_max - m_t; // size left until circular border crossing
		// always copy full block to buffer(buf) + top pointer(m_t)
		// because we have doubled the buffer size for performance reasons
		memcpy(buf + m_t, s, l);
		memcpy(buf, s + l1, l - l1);
		m_t = l - l1;
		m_q += l;
	}
	else
	{
		memcpy(buf + m_t, s, l);
		memcpy(buf + m_max + m_t, s, l);
		m_t += l;
		if (m_t >= m_max)
			m_t -= m_max;
		m_q += l;
	}
	return true;
}


bool TcpSocket::CircularBuffer::Read(char *s,size_t l)
{
	if (l > m_q)
	{
		return false; // not enough chars
	}
	if (m_b + l > m_max) // block crosses circular border
	{
		size_t l1 = m_max - m_b;
		if (s)
		{
			memcpy(s, buf + m_b, l1);
			memcpy(s + l1, buf, l - l1);
		}
		m_b = l - l1;
		m_q -= l;
	}
	else
	{
		if (s)
		{
			memcpy(s, buf + m_b, l);
		}
		m_b += l;
		if (m_b >= m_max)
			m_b -= m_max;
		m_q -= l;
	}
	if (!m_q)
	{
		m_b = m_t = 0;
	}
	return true;
}


bool TcpSocket::CircularBuffer::Remove(size_t l)
{
	return Read(NULL, l);
}


size_t TcpSocket::CircularBuffer::GetLength()
{
	return m_q;
}


const char *TcpSocket::CircularBuffer::GetStart()
{
	return buf + m_b;
}


size_t TcpSocket::CircularBuffer::GetL()
{
	return (m_b + m_q > m_max) ? m_max - m_b : m_q;
}


size_t TcpSocket::CircularBuffer::Space()
{
	return m_max - m_q;
}


unsigned long TcpSocket::CircularBuffer::ByteCounter(bool clear)
{
	if (clear)
	{
		unsigned long x = m_count;
		m_count = 0;
		return x;
	}
	return m_count;
}


std::string TcpSocket::CircularBuffer::ReadString(size_t l)
{
	char *sz = new char[l + 1];
	if (!Read(sz, l)) // failed, debug printout in Read() method
	{
		delete[] sz;
		return "";
	}
	sz[l] = 0;
	std::string tmp = sz;
	delete[] sz;
	return tmp;
}

