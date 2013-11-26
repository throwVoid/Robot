#pragma once
#ifndef _ROBOT_DEBUG_
#define _ROBOT_DEBUG_
#include <exception>
#include <stdexcept>
#include <stdio.h>
#include <new>
#include <assert.h>
/*
 * power by RenZhenwen
 * */
#define DEBUG
#ifdef DEBUG
	#define PRINT(s)\
		{\
			printf("[%s] [%s] [%d]\n",s,__FILE__,__LINE__);\
		}
#else
	#define PRINT(s)\
		{\
		}
#endif

#ifdef DEBUG
	#define PRINT_MORE(fmt, args...)  printf(fmt, ## args)
#else
	#define PRINT_MORE(fmt, args...) 
#endif


#endif
