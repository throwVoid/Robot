#pragma once
#ifndef ROBOT_MAIN_DEBUG_H_
#define ROBOT_MAIN_DEBUG_H_

/*
 *Power by zhenwenRen
 *Email: admin@unix8.net
 * */

#include <assert.h>
#include <stdio.h>

#define DEBUG

#ifdef DEBUG
	#include <iostream>
	#include <iterator>
	#include <algorithm>
	using namespace std;
#endif



#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
#ifdef DEBUG
	#define PRINT(fmt, ...)  do{fprintf(stdout, fmt, ##__VA_ARGS__); fprintf(stdout,"\n");}while(0)
	#define PRINTS_MORE(s) do {\
			printf("[%s] [%s] [%d]\n",s,__FILE__,__LINE__);\
		}while(0)
	#define PRINT_ERR(fmt, ...)  do{fprintf(stderr, fmt, ##__VA_ARGS__); fprintf(stderr,"\n");}while(0)

	template<class Iterator>
	void PRINT_RANGE(Iterator start, Iterator finish)
	{
		ostream_iterator<typename iterator_traits<Iterator>::value_type> output(cout, " ");
		copy(start, finish, output);
		std::cout<<endl;
	}
#else
	#define PRINT(fmt, ...)  do{}while(0)
	#define PRINTS_MORE(s) do{}while(0)
	#define PRINT_ERR(fmt, ...) do{}while(0)
	template<class Iterator>
	void PRINT_RANGE(Iterator start, Iterator finish){}
#endif
#ifdef ROBOT_NAMESPACE
}
#endif

#endif//ROBOT_MAIN_DEBUG_H_
