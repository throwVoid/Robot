#pragma once

#ifndef _SP_COUNTED_H_
#define _SP_COUNTED_H_
/*
 * power by RenZhenwen
 * date:2012-12-07
 *
 * */

#include "robot_volatile_long.h"
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
struct  sp_counted_base
{
	ref_long	use;
	ref_long	weak;

	sp_counted_base(ref_long::count_type _use, ref_long::count_type _weak)
		: use(_use), weak(_weak) {}
};
#ifdef ROBOT_NAMESPACE
}
#endif
#endif
