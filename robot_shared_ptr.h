#pragma once

#ifndef _shared_ptr_h_
#define _shared_ptr_h_

/*
 * power by RenZhenwen
 * date:2012-12-07
 *
 * */

#include "robot_sp_counted.h"
#include "robot_weak_ptr.h"
#include "robot_enable_shared_from_this.h"
#include "assert.h"
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
#define _assert_define(x) assert(x != 0)
#define shared_ptr_assert(x)	_assert_define(x)

inline void _sp_set_shared_from_this( ... )
{
}
template<typename T> inline void _sp_set_shared_from_this(shared_ptr<T>* sp, enable_shared_from_this<T>* p)
{
	if (p)
	{
		p->_internal_accept_owner( *sp );
	}
}
template<typename T> class shared_ptr;
template<typename T> class weak_ptr;
template<class T>
class shared_ptr
{
public:
	typedef shared_ptr<T> this_type;  		//智能指针类型
	typedef ref_long::count_type count_type;	//引用计数类型
	typedef bool BOOL;
//	template<typename T> class shared_ptr;
//	template<typename T> class weak_ptr;
private:
	T*					m_ptr; //实际的指针
	sp_counted_base*	m_count;		//计数
public:
	shared_ptr(T * p = NULL)			//构造函数
	{
		m_ptr = p;
		m_count = new sp_counted_base(1,1);
		_sp_set_shared_from_this(this, m_ptr);
	}

	shared_ptr(const weak_ptr<T>& right)
	{
		m_ptr = right.get();
		m_count = right._get_sp();
		addref();
		_sp_set_shared_from_this(this, m_ptr);
	}

	shared_ptr(const this_type& o)
	{
		m_ptr = o.m_ptr;
		m_count = o.m_count;
		addref();
		_sp_set_shared_from_this(this, m_ptr);
	}
	
	~shared_ptr()
	{
		release();
	}
	
	this_type& operator=(const this_type& o)
	{
		if(this!=&o)
		{
			if(m_ptr!=o.m_ptr)
			{
				release();
				m_ptr = o.m_ptr;
				m_count = o.m_count;
				_sp_set_shared_from_this(this, m_ptr);
				addref();
			}
		}
		return (*this);
	}

	this_type& operator=(const T* p)
	{
		if (m_ptr != p)
		{
			release();
			m_ptr = const_cast<T*>(p);
			m_count = new sp_counted_base(1,1);
			_sp_set_shared_from_this(this, m_ptr);
		}
		return (*this);
	}

	BOOL operator == (const this_type& o) const
	{
		return m_ptr == o.m_ptr;
	}

	BOOL operator == (const T* p) const
	{
		return (m_ptr == p);
	}

	BOOL operator != (const this_type& o) const
	{
		return m_ptr != o.m_ptr;
	}
#if _MSC_VER < 1300
	BOOL operator != (const T* p) const
	{
		return m_ptr != p;
	}
#endif
/*	
	operator BOOL() const
	{
		return m_ptr != NULL;
	}
*/
	operator T*() const
	{
		return m_ptr;
	}

	T& operator*() const
	{
		return (*m_ptr);
	}
	
	T* operator->() const
	{
		shared_ptr_assert(m_ptr);
		return (m_ptr);
	}

	T* get() const
	{
		return m_ptr;
	}

	count_type use_count() const
	{
		if (m_ptr)
		{
			return m_count->use.get();
		}
		return 0;
	}

	void reset()
	{
		release();
	}

public:
	void addref(void)
	{
		m_count->use.inc();
	}
	
	void release(void)
	{
		if (m_count)
		{
			if(m_count->use.dec() == 0)
			{
				delete_object(m_ptr);
				if( m_count->weak.dec() == 0 )
				{   
					
					delete m_count;
				}
			}
			m_count = NULL;
		}
		m_ptr = NULL;
	}
public:
	sp_counted_base* _get_sp() const
	{
		return m_count;
	}
protected:
	void delete_object(T* p)
	{
		if (p)
		{
			delete p;
		}
	}
};
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
#endif
