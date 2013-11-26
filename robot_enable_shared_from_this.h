#pragma once

#ifndef _ENABLE_SHARED_FROM_THIS_H_
#define _ENABLE_SHARED_FROM_THIS_H_

/*
 * power by RenZhenwen
 * date:2012-12-07
 *
 * */
#include "robot_shared_ptr.h"
#include "robot_weak_ptr.h"
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif

template<typename T>
class enable_shared_from_this
{
public:
	shared_ptr<T> shared_from_this()
	{
		return shared_ptr<T>( weak_this_ );
	}
public:
	void _internal_accept_owner(const shared_ptr<T>& p)
	{
		weak_this_ = p;
	}
private:
	weak_ptr<T> weak_this_;
};
#ifdef ROBOT_NAMESPACE
}
#endif
#endif
