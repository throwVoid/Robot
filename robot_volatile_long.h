#pragma once

#ifndef _INC_VOLATILE_LONG_H_
#define _INC_VOLATILE_LONG_H_

/*
 * power by RenZhenwen
 * date:2012-12-07
 *
 * */

//# include <windows.h>
//#include </usr/src/kernels/2.6.25-14.fc9.i686/include/asm/atomic_32.h>
//#include "/usr/src/kernels/2.6.25-14.fc9.i686/include/asm/atomic.h"
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif

class ref_long
{
public:
	typedef volatile long count_type;
/*
	static __inline__ void atomic_inc(long  *v)
	{
   		__asm__ __volatile__(
        	"lock incl %0"
       	:"+m" (*v));
	}*/
	explicit ref_long(count_type ref) {
		m_ref = ref;
	}
	long inc() {
	//	return InterlockedIncrement(&m_ref);
//		return atomic_inc(&m_ref);
		return ++m_ref;
	}
	
	long dec() {
//		return InterlockedDecrement(&m_ref);
	//	return atomic_dec(&m_ref);
		return --m_ref;
	}
	long operator++() {
		return 	inc();
	}
	long operator--() {
		return dec();
	}
	long operator++(int) {
		return inc()-1;
	}
	long operator--(int) {
		return dec()+1;
	}
public:
	count_type get() const {
		return m_ref;
	}
private:
	count_type m_ref;
};
#ifdef ROBOT_NAMESPACE
}
#endif
#endif
