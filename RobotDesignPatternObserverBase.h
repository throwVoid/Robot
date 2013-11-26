#pragma once
#ifndef ROBOT_MAIN_DP_H_
#define ROBOT_MAIN_DP_H_
/*
 *Power by Renzhenwen
 *Email:admin@unix8.net
 * */

#include<algorithm>
#include<list>
#include<iostream>
#include<string>
#include<vector>
//using namespace std;

//采用泛型模板编写，省去了观察者基类
//以下的3个traits目的是：根据观察者函数的参数为引用还算指来改变主题的通知函数参数类型
//使用：
//struct or class Observer { /*需要响应的成员函数,myFUnc(/*参数*/)*/};
//struct MySubject : public BasicSubject<Observer> {/*函数调用notifyAll*/};
//
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
template <typename T>
struct arg_type_traits 
{
    typedef const T& result;
};
/*观察者参数为T& ,则主题参数为T&(意味着不能将字面值传递给引用)*/
template <typename T>
struct arg_type_traits<T&> 
{
    typedef T& result;
};

/*观察者参数为const T& ,则主题参数为const T&*/
template <typename T>
struct arg_type_traits<const T&> 
{
    typedef const T& result;
};
template<class ObserverT, class ContainerT = std::list<ObserverT*> >  //默认容器为list
class BasicSubject
{
public:
	inline void addObserver(ObserverT& observer)	//增加观察者
	{
		observers_.push_back(&observer);

	}
	inline void removeObserver(ObserverT& observer) //删除观察者
	{
		observers_.erase(std::remove(observers_.begin(),observers_.end(),&observer), observers_.end());
	}
	template <typename ReturnT>
	inline void notifyAll(ReturnT (ObserverT::*pfn)())  //通知观察者
	{
		for (typename ContainerT::iterator it = observers_.begin(), itEnd = observers_.end(); it != itEnd; ++it)
			((*it)->*pfn)();
	}

	template <typename ReturnT, typename Arg1T>
	inline void notifyAll(ReturnT (ObserverT::*pfn)(Arg1T), 
		typename arg_type_traits<Arg1T>::result arg1)   
	{
		for (typename ContainerT::iterator it = observers_.begin(),itEnd = observers_.end(); it != itEnd; ++it)
			((*it)->*pfn)(arg1);
	}

	template <typename ReturnT, typename Arg1T, typename Arg2T>
	inline void notifyAll(ReturnT (ObserverT::*pfn)(Arg1T, Arg2T), 
		typename arg_type_traits<Arg1T>::result arg1, 
		typename arg_type_traits<Arg2T>::result arg2 )  
	{
		for (typename ContainerT::iterator it = observers_.begin(), itEnd = observers_.end(); it != itEnd; ++it)
			((*it)->*pfn)(arg1, arg2);
	}
	template <typename ReturnT, typename Arg1T, typename Arg2T, typename Arg3T, typename Arg4T, typename Arg5T>
	inline void notifyAll(ReturnT (ObserverT::*pfn)(Arg1T, Arg2T, Arg3T, Arg4T, Arg5T), 
		typename arg_type_traits<Arg1T>::result arg1, 
		typename arg_type_traits<Arg2T>::result arg2,
		typename arg_type_traits<Arg3T>::result arg3,
		typename arg_type_traits<Arg4T>::result arg4,
		typename arg_type_traits<Arg5T>::result arg5) 
	{
		for (typename ContainerT::iterator it = observers_.begin(), itEnd = observers_.end(); it != itEnd; ++it)
			((*it)->*pfn)(arg1, arg2, arg3, arg4, arg5);
	}
protected:
	ContainerT observers_;
};
#ifdef ROBOT_NAMESPACE
}
#endif
#endif //ROBOT_MAIN_DP_H_
