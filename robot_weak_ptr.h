#pragma once

#ifndef _weak_ptr_h_
#define _weak_ptr_h_

/*
 * power by RenZhenwen
 * date:2012-12-07
 *
 * */
#include "robot_shared_ptr.h"
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
template<typename T> class shared_ptr;
template<typename T> class weak_ptr;


template<typename T>
class weak_ptr
{
protected:
	T* m_ptr;
	sp_counted_base* m_count;
public:
	typedef bool BOOL;
	weak_ptr()
	{
		m_ptr = NULL;
		m_count = 0;
	}

	weak_ptr(const shared_ptr<T>& right)
	{
		m_ptr = right.get();
		m_count = shared_ptr<T>::_get_counted(right);
		m_count->weak.inc();
	}
	weak_ptr(const weak_ptr<T>& right)
	{
		m_ptr = right.m_ptr;
		m_count = right.m_count;
		m_count->weak.inc();
	}
	~weak_ptr()
	{
		reset();
	}

	weak_ptr<T>& operator=(const weak_ptr<T>& right)
	{
		reset();
		m_ptr = right.m_ptr;
		m_count = right.m_count;
		m_count->weak.inc();
		return *this;
	}

	weak_ptr<T>& operator=(const shared_ptr<T>& right)
	{
		reset();
		m_ptr = right.get();
		m_count = right._get_sp();
		if(m_count) {
			m_count->weak.inc();
		}
		return *this;
	}

	void reset()
	{
		if (m_count && m_count->weak.dec() == 0)
		{
			delete m_count;
			m_count = NULL;
		}
	}

	T* get() const
	{
		if (!expired())
		{
			return m_ptr;
		}
		return NULL;
	}

	BOOL expired() const
	{
		return (m_count == NULL || m_count->use.get() == 0);
	}

	shared_ptr<T> lock() const
	{
		if ( !expired() )
		{
			return shared_ptr<T>(*this);
		}
		return shared_ptr<T>(NULL);
	}

	sp_counted_base* _get_sp() const
	{
		return m_count;
	}
};
#ifdef ROBOT_NAMESPACE
}
#endif

#endif
