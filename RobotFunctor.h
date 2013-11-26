#pragma once
#ifndef ROBOT_MAIN_FUNCTOR_H_
#define ROBOT_MAIN_FUNCTOR_H_

/*
 * Power by RenZhenwen
 * Date:2012-12-07
 * Email:admin@unix8.net
 * */
#include <map>
#include <iostream>
#include <time.h>
#include <string>
#include <list>


using namespace std;
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
typedef std::map<int, std::string> ArStrMap;
class RobotFunctor
{
public:
  virtual ~RobotFunctor(void) {}
  virtual void invoke(void) = 0;
  virtual const char *getName(void) { return myName.c_str();  }
  virtual void setName(const char *name) { myName = name; }
protected:
  std::string myName;
};


template<class P1>
class RobotFunctor1 : public RobotFunctor //带有一个参数的基类
{
public:
  virtual ~RobotFunctor1(void) {}
  virtual void invoke(void) = 0;				//invoke为函数的执行成员函数，实现为纯虚
  virtual void invoke(P1 p1) = 0;
};

template<class P1, class P2>
class RobotFunctor2 : public RobotFunctor1<P1>//带有二个参数的基类
{
public:
  virtual ~RobotFunctor2() {}
  virtual void invoke(void) = 0;
  virtual void invoke(P1 p1) = 0;
  virtual void invoke(P1 p1, P2 p2) = 0;
};

template<class P1, class P2, class P3>
class RobotFunctor3 : public RobotFunctor2<P1, P2>//带有3个参数的基类
{
public:
  virtual ~RobotFunctor3() {}
  virtual void invoke(void) = 0;
  virtual void invoke(P1 p1) = 0;
  virtual void invoke(P1 p1, P2 p2) = 0;
  virtual void invoke(P1 p1, P2 p2, P3 p3) = 0;
};

template<class P1, class P2, class P3, class P4>
class RobotFunctor4 : public RobotFunctor3<P1, P2, P3>//带有四个参数的基类
{
public:
  virtual ~RobotFunctor4() {}
  virtual void invoke(void) = 0;
  virtual void invoke(P1 p1) = 0;
  virtual void invoke(P1 p1, P2 p2) = 0;
  virtual void invoke(P1 p1, P2 p2, P3 p3) = 0;
  virtual void invoke(P1 p1, P2 p2, P3 p3, P4 p4) = 0;

};

template<class Ret>
class ArRetFunctor : public RobotFunctor//有一个返回值，但是没有参数的基类
{
public:
  virtual ~ArRetFunctor() {}
  virtual void invoke(void) {invokeR();}
  virtual Ret invokeR(void) = 0;
};


template<class Ret, class P1>
class ArRetFunctor1 : public ArRetFunctor<Ret>//有一个返回值，1个参数的基类
{
public:
  virtual ~ArRetFunctor1() {}
  virtual Ret invokeR(void) = 0;
  virtual Ret invokeR(P1 p1) = 0;
};

template<class Ret, class P1, class P2>
class ArRetFunctor2 : public ArRetFunctor1<Ret, P1>//有一个返回值，2个参数的基类
{
public:
  virtual ~ArRetFunctor2() {}
  virtual Ret invokeR(void) = 0;
  virtual Ret invokeR(P1 p1) = 0;
  virtual Ret invokeR(P1 p1, P2 p2) = 0;
};

template<class Ret, class P1, class P2, class P3>
class ArRetFunctor3 : public ArRetFunctor2<Ret, P1, P2>//有一个返回值，3个参数的基类
{
public:
  virtual ~ArRetFunctor3() {}
  virtual Ret invokeR(void) = 0;
  virtual Ret invokeR(P1 p1) = 0;
  virtual Ret invokeR(P1 p1, P2 p2) = 0;
  virtual Ret invokeR(P1 p1, P2 p2, P3 p3) = 0;
};


template<class Ret, class P1, class P2, class P3, class P4>
class ArRetFunctor4 : public ArRetFunctor3<Ret, P1, P2, P3>//有一个返回值，4个参数的基类
{
public:
  virtual ~ArRetFunctor4() {}
  virtual Ret invokeR(void) = 0;
  virtual Ret invokeR(P1 p1) = 0;
  virtual Ret invokeR(P1 p1, P2 p2) = 0;
  virtual Ret invokeR(P1 p1, P2 p2, P3 p3) = 0;
  virtual Ret invokeR(P1 p1, P2 p2, P3 p3, P4 p4) = 0;
};





/*以下是为C风格的全局函数设计的类参数封装*/

#ifndef SWIG

class ArGlobalFunctor : public RobotFunctor//全局函数没有参数
{
public:
  ArGlobalFunctor() {}
  ArGlobalFunctor(void (*func)(void)) : myFunc(func) {}
  virtual ~ArGlobalFunctor() {}
  virtual void invoke(void) {(*myFunc)();}
protected:
  void (*myFunc)(void);
};


template<class P1>
class ArGlobalFunctor1 : public RobotFunctor1<P1>//全局函数有1个参数
{
public:
  ArGlobalFunctor1() {}
  ArGlobalFunctor1(void (*func)(P1)) :myFunc(func), myP1() {}
  ArGlobalFunctor1(void (*func)(P1), P1 p1) : myFunc(func), myP1(p1) {}
  virtual ~ArGlobalFunctor1() {}
  virtual void invoke(void) {(*myFunc)(myP1);}
  virtual void invoke(P1 p1) {(*myFunc)(p1);}
  virtual void setP1(P1 p1) {myP1=p1;}
protected:
  void (*myFunc)(P1);
  P1 myP1;
};

template<class P1, class P2>
class ArGlobalFunctor2 : public RobotFunctor2<P1, P2>//全局函数有2个参数
{
public:
  ArGlobalFunctor2() {}
  ArGlobalFunctor2(void (*func)(P1, P2)) :myFunc(func), myP1(), myP2() {}
  ArGlobalFunctor2(void (*func)(P1, P2), P1 p1) :myFunc(func), myP1(p1), myP2() {}
  ArGlobalFunctor2(void (*func)(P1, P2), P1 p1, P2 p2) :myFunc(func), myP1(p1), myP2(p2) {}
  virtual ~ArGlobalFunctor2() {}
  virtual void invoke(void) {(*myFunc)(myP1, myP2);}
  virtual void invoke(P1 p1) {(*myFunc)(p1, myP2);}
  virtual void invoke(P1 p1, P2 p2) {(*myFunc)(p1, p2);}
  virtual void setP1(P1 p1) {myP1=p1;}
  virtual void setP2(P2 p2) {myP2=p2;}
protected:
  void (*myFunc)(P1, P2);
  P1 myP1;
  P2 myP2;
};


template<class P1, class P2, class P3>
class ArGlobalFunctor3 : public RobotFunctor3<P1, P2, P3>//全局函数有3个参数
{
public:
  ArGlobalFunctor3() {}
  ArGlobalFunctor3(void (*func)(P1, P2, P3)) :
    myFunc(func), myP1(), myP2(), myP3() {}
  ArGlobalFunctor3(void (*func)(P1, P2, P3), P1 p1) :
    myFunc(func), myP1(p1), myP2(), myP3() {}
  ArGlobalFunctor3(void (*func)(P1, P2, P3), P1 p1, P2 p2) :
    myFunc(func), myP1(p1), myP2(p2), myP3() {}
  ArGlobalFunctor3(void (*func)(P1, P2, P3), P1 p1, P2 p2, P3 p3) :
    myFunc(func), myP1(p1), myP2(p2), myP3(p3) {}
  virtual ~ArGlobalFunctor3() {}
  virtual void invoke(void) {(*myFunc)(myP1, myP2, myP3);}
  virtual void invoke(P1 p1) {(*myFunc)(p1, myP2, myP3);}
  virtual void invoke(P1 p1, P2 p2) {(*myFunc)(p1, p2, myP3);}
  virtual void invoke(P1 p1, P2 p2, P3 p3) {(*myFunc)(p1, p2, p3);}
  virtual void setP1(P1 p1) {myP1=p1;}
  virtual void setP2(P2 p2) {myP2=p2;}
  virtual void setP3(P3 p3) {myP3=p3;}
protected:
  void (*myFunc)(P1, P2, P3);
  P1 myP1;
  P2 myP2;
  P3 myP3;
};




template<class P1, class P2, class P3, class P4>
class ArGlobalFunctor4 : public RobotFunctor4<P1, P2, P3, P4>//全局函数有4个参数
{
public:
  ArGlobalFunctor4() {}
  ArGlobalFunctor4(void (*func)(P1, P2, P3, P4)) :
    myFunc(func), myP1(), myP2(), myP3(), myP4() {}
  ArGlobalFunctor4(void (*func)(P1, P2, P3, P4), P1 p1) :
    myFunc(func), myP1(p1), myP2(), myP3(), myP4() {}
  ArGlobalFunctor4(void (*func)(P1, P2, P3, P4), P1 p1, P2 p2) :
    myFunc(func), myP1(p1), myP2(p2), myP3(), myP4() {}
  ArGlobalFunctor4(void (*func)(P1, P2, P3, P4), P1 p1, P2 p2, P3 p3) :
    myFunc(func), myP1(p1), myP2(p2), myP3(p3), myP4() {}
 ArGlobalFunctor4(void (*func)(P1, P2, P3, P4), P1 p1, P2 p2, P3 p3, P4 p4) :
    myFunc(func), myP1(p1), myP2(p2), myP3(p3), myP4(p4) {}
  virtual ~ArGlobalFunctor4() {}
  virtual void invoke(void) {(*myFunc)(myP1, myP2, myP3, myP4);}
  virtual void invoke(P1 p1) {(*myFunc)(p1, myP2, myP3, myP4);}
  virtual void invoke(P1 p1, P2 p2) {(*myFunc)(p1, p2, myP3, myP4);}
  virtual void invoke(P1 p1, P2 p2, P3 p3) {(*myFunc)(p1, p2, p3, myP4);}
  virtual void invoke(P1 p1, P2 p2, P3 p3, P4 p4) {(*myFunc)(p1, p2, p3, p4);}
  virtual void setP1(P1 p1) {myP1=p1;}
  virtual void setP2(P2 p2) {myP2=p2;}
  virtual void setP3(P3 p3) {myP3=p3;}
  virtual void setP4(P4 p4) {myP4=p4;}
protected:
  void (*myFunc)(P1, P2, P3, P4);
  P1 myP1;
  P2 myP2;
  P3 myP3;
  P4 myP4;
};




/*以下是全局函数带有一个返回值的参数封装*/

template<class Ret>
class ArGlobalRetFunctor : public ArRetFunctor<Ret>//0参数，1返回值
{
public:
  ArGlobalRetFunctor() {}
  ArGlobalRetFunctor(Ret (*func)(void)) : myFunc(func) {}
  virtual ~ArGlobalRetFunctor() {}
  virtual Ret invokeR(void) {return (*myFunc)();}
protected:
  Ret (*myFunc)(void);
};

template<class Ret, class P1>
class ArGlobalRetFunctor1 : public ArRetFunctor1<Ret, P1>//1参数，1返回值
{
public:
  ArGlobalRetFunctor1() {}
  ArGlobalRetFunctor1(Ret (*func)(P1)) :
    myFunc(func), myP1() {}
  ArGlobalRetFunctor1(Ret (*func)(P1), P1 p1) :
    myFunc(func), myP1(p1) {}
  virtual ~ArGlobalRetFunctor1() {}
  virtual Ret invokeR(void) {return (*myFunc)(myP1);}
  virtual Ret invokeR(P1 p1) {return (*myFunc)(p1);}
  virtual void setP1(P1 p1) {myP1=p1;}
protected:
  Ret (*myFunc)(P1);
  P1 myP1;
};


template<class Ret, class P1, class P2>
class ArGlobalRetFunctor2 : public ArRetFunctor2<Ret, P1, P2>//2参数，1返回值
{
public:
  ArGlobalRetFunctor2() {}
  ArGlobalRetFunctor2(Ret (*func)(P1, P2)) :
    myFunc(func), myP1(), myP2() {}
  ArGlobalRetFunctor2(Ret (*func)(P1, P2), P1 p1) :
    myFunc(func), myP1(p1), myP2() {}
  ArGlobalRetFunctor2(Ret (*func)(P1, P2), P1 p1, P2 p2) :
    myFunc(func), myP1(p1), myP2(p2) {}
  virtual ~ArGlobalRetFunctor2() {}
  virtual Ret invokeR(void) {return (*myFunc)(myP1, myP2);}
  virtual Ret invokeR(P1 p1) {return (*myFunc)(p1, myP2);}
  virtual Ret invokeR(P1 p1, P2 p2) {return (*myFunc)(p1, p2);}
  virtual void setP1(P1 p1) {myP1=p1;}
  virtual void setP2(P2 p2) {myP2=p2;}
protected:
  Ret (*myFunc)(P1, P2);
  P1 myP1;
  P2 myP2;
};


template<class Ret, class P1, class P2, class P3>
class ArGlobalRetFunctor3 : public ArRetFunctor3<Ret, P1, P2, P3>//3参数，1返回值
{
public:
  ArGlobalRetFunctor3() {}
  ArGlobalRetFunctor3(Ret (*func)(P1, P2, P3)) :
    myFunc(func), myP1(), myP2(), myP3() {}
  ArGlobalRetFunctor3(Ret (*func)(P1, P2, P3), P1 p1) :
    myFunc(func), myP1(p1), myP2(), myP3() {}
  ArGlobalRetFunctor3(Ret (*func)(P1, P2, P3), P1 p1, P2 p2) :
    myFunc(func), myP1(p1), myP2(p2), myP3() {}
  ArGlobalRetFunctor3(Ret (*func)(P1, P2, P3), P1 p1, P2 p2, P3 p3) :
    myFunc(func), myP1(p1), myP2(p2), myP3(p3) {}
  virtual ~ArGlobalRetFunctor3() {}
  virtual Ret invokeR(void) {return (*myFunc)(myP1, myP2, myP3);}
  virtual Ret invokeR(P1 p1) {return (*myFunc)(p1, myP2, myP3);}
  virtual Ret invokeR(P1 p1, P2 p2) {return (*myFunc)(p1, p2, myP3);}
  virtual Ret invokeR(P1 p1, P2 p2, P3 p3) {return (*myFunc)(p1, p2, p3);}
  virtual void setP1(P1 p1) {myP1=p1;}
  virtual void setP2(P2 p2) {myP2=p2;}
  virtual void setP3(P3 p3) {myP3=p3;}
protected:
  Ret (*myFunc)(P1, P2, P3);
  P1 myP1;
  P2 myP2;
  P3 myP3;
};


template<class Ret, class P1, class P2, class P3, class P4>
class ArGlobalRetFunctor4 : public ArRetFunctor4<Ret, P1, P2, P3, P4>//4参数，1返回值
{
public:
  ArGlobalRetFunctor4() {}
  ArGlobalRetFunctor4(Ret (*func)(P1, P2, P3)) :
    myFunc(func), myP1(), myP2(), myP3() {}
  ArGlobalRetFunctor4(Ret (*func)(P1, P2, P3), P1 p1) :
    myFunc(func), myP1(p1), myP2(), myP3() {}
  ArGlobalRetFunctor4(Ret (*func)(P1, P2, P3), P1 p1, P2 p2) :
    myFunc(func), myP1(p1), myP2(p2), myP3() {}
  ArGlobalRetFunctor4(Ret (*func)(P1, P2, P3, P4), P1 p1, P2 p2, P3 p3) :
    myFunc(func), myP1(p1), myP2(p2), myP3(p3), myP4() {}
  ArGlobalRetFunctor4(Ret (*func)(P1, P2, P3, P4), P1 p1, P2 p2, P3 p3, P4 p4) :
    myFunc(func), myP1(p1), myP2(p2), myP3(p3), myP4(p4) {}

  virtual ~ArGlobalRetFunctor4() {}
  virtual Ret invokeR(void) {return (*myFunc)(myP1, myP2, myP3, myP4);}
  virtual Ret invokeR(P1 p1) {return (*myFunc)(p1, myP2, myP3, myP4);}
  virtual Ret invokeR(P1 p1, P2 p2) {return (*myFunc)(p1, p2, myP3, myP4);}
  virtual Ret invokeR(P1 p1, P2 p2, P3 p3) {return (*myFunc)(p1, p2, p3, myP4);}
  virtual Ret invokeR(P1 p1, P2 p2, P3 p3, P4 p4) {return (*myFunc)(p1, p2, p3, p4);}

  virtual void setP1(P1 p1) {myP1=p1;}
  virtual void setP2(P2 p2) {myP2=p2;}
  virtual void setP3(P3 p3) {myP3=p3;}
  virtual void setP4(P4 p4) {myP4=p4;}
protected:
  Ret (*myFunc)(P1, P2, P3, P4);
  P1 myP1;
  P2 myP2;
  P3 myP3;
  P4 myP4;
};

#endif // SWIG

/*以下为类的成员函数提供封装，非常重要*/

template<class T>
class RobotFunctorC : public RobotFunctor  /*0参数，0返回值*/
{
public:
  RobotFunctorC() {}
  RobotFunctorC(T &obj, void (T::*func)(void)) : myObj(&obj), myFunc(func) {}
  RobotFunctorC(T *obj, void (T::*func)(void)) : myObj(obj), myFunc(func) {}
  virtual ~RobotFunctorC() {}
  virtual void invoke(void) {(myObj->*myFunc)();}
  virtual void setThis(T *obj) {myObj=obj;}
  virtual void setThis(T &obj) {myObj=&obj;}
protected:
  T *myObj;
  void (T::*myFunc)(void);
};

template<class T, class P1>
class RobotFunctor1C : public RobotFunctor1<P1>/*1参数，0返回值*/
{
public:
  RobotFunctor1C() {}
  RobotFunctor1C(T &obj, void (T::*func)(P1)) :
    myObj(&obj), myFunc(func), myP1() {}
  RobotFunctor1C(T &obj, void (T::*func)(P1), P1 p1) :
    myObj(&obj), myFunc(func), myP1(p1) {}
  RobotFunctor1C(T *obj, void (T::*func)(P1)) :
    myObj(obj), myFunc(func), myP1() {}
  RobotFunctor1C(T *obj, void (T::*func)(P1), P1 p1) :
    myObj(obj), myFunc(func), myP1(p1) {}
  virtual ~RobotFunctor1C() {}
  virtual void invoke(void) {(myObj->*myFunc)(myP1);}
  virtual void invoke(P1 p1) {(myObj->*myFunc)(p1);}
  virtual void setThis(T *obj) {myObj=obj;}
  virtual void setThis(T &obj) {myObj=&obj;}
  virtual void setP1(P1 p1) {myP1=p1;}
protected:
  T *myObj;
  void (T::*myFunc)(P1);
  P1 myP1;
};

template<class T, class P1, class P2>
class RobotFunctor2C : public RobotFunctor2<P1, P2>/*2参数，0返回值*/
{
public:
  RobotFunctor2C() {}
  RobotFunctor2C(T &obj, void (T::*func)(P1, P2)) :
    myObj(&obj), myFunc(func), myP1(), myP2() {}
  RobotFunctor2C(T &obj, void (T::*func)(P1, P2), P1 p1) :
    myObj(&obj), myFunc(func), myP1(p1), myP2() {}
  RobotFunctor2C(T &obj, void (T::*func)(P1, P2), P1 p1, P2 p2) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(p2) {}
  RobotFunctor2C(T *obj, void (T::*func)(P1, P2)) :
    myObj(obj), myFunc(func), myP1(), myP2() {}
  RobotFunctor2C(T *obj, void (T::*func)(P1, P2), P1 p1) :
    myObj(obj), myFunc(func), myP1(p1), myP2() {}
  RobotFunctor2C(T *obj, void (T::*func)(P1, P2), P1 p1, P2 p2) :
    myObj(obj), myFunc(func), myP1(p1), myP2(p2) {}
  virtual ~RobotFunctor2C() {}
  virtual void invoke(void) {(myObj->*myFunc)(myP1, myP2);}
  virtual void invoke(P1 p1) {(myObj->*myFunc)(p1, myP2);}
  virtual void invoke(P1 p1, P2 p2) {(myObj->*myFunc)(p1, p2);}
  virtual void setThis(T *obj) {myObj=obj;}
  virtual void setThis(T &obj) {myObj=&obj;}
  virtual void setP1(P1 p1) {myP1=p1;}
  virtual void setP2(P2 p2) {myP2=p2;}
protected:
  T *myObj;
  void (T::*myFunc)(P1, P2);
  P1 myP1;
  P2 myP2;
};

template<class T, class P1, class P2, class P3>
class RobotFunctor3C : public RobotFunctor3<P1, P2, P3>/*3参数，0返回值*/
{
public:
  RobotFunctor3C() {}
  RobotFunctor3C(T &obj, void (T::*func)(P1, P2, P3)) :
    myObj(&obj), myFunc(func), myP1(), myP2(), myP3() {}
  RobotFunctor3C(T &obj, void (T::*func)(P1, P2, P3), P1 p1) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(), myP3() {}
  RobotFunctor3C(T &obj, void (T::*func)(P1, P2, P3), P1 p1, P2 p2) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(p2), myP3() {}
  RobotFunctor3C(T &obj, void (T::*func)(P1, P2, P3), P1 p1, P2 p2, P3 p3) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(p2), myP3(p3) {}
  RobotFunctor3C(T *obj, void (T::*func)(P1, P2, P3)) :
    myObj(obj), myFunc(func), myP1(), myP2(), myP3() {}

  RobotFunctor3C(T *obj, void (T::*func)(P1, P2, P3), P1 p1) :
    myObj(obj), myFunc(func), myP1(p1), myP2(), myP3() {}

  RobotFunctor3C(T *obj, void (T::*func)(P1, P2, P3), P1 p1, P2 p2) :
    myObj(obj), myFunc(func), myP1(p1), myP2(p2), myP3() {} 


  RobotFunctor3C(T *obj, void (T::*func)(P1, P2, P3), P1 p1, P2 p2, P3 p3) :
    myObj(obj), myFunc(func), myP1(p1), myP2(p2), myP3(p3) {}


  virtual ~RobotFunctor3C() {}


  virtual void invoke(void) {(myObj->*myFunc)(myP1, myP2, myP3);}

  virtual void invoke(P1 p1) {(myObj->*myFunc)(p1, myP2, myP3);}


  virtual void invoke(P1 p1, P2 p2) {(myObj->*myFunc)(p1, p2, myP3);}


  virtual void invoke(P1 p1, P2 p2, P3 p3) {(myObj->*myFunc)(p1, p2, p3);}

  virtual void setThis(T *obj) {myObj=obj;}

  virtual void setThis(T &obj) {myObj=&obj;}

  virtual void setP1(P1 p1) {myP1=p1;}


  virtual void setP2(P2 p2) {myP2=p2;}

  virtual void setP3(P3 p3) {myP3=p3;}
protected:
  T *myObj;
  void (T::*myFunc)(P1, P2, P3);
  P1 myP1;
  P2 myP2;
  P3 myP3;
};



template<class T, class P1, class P2, class P3, class P4>
class RobotFunctor4C : public RobotFunctor4<P1, P2, P3, P4>/*4参数，0返回值*/
{
public:
  RobotFunctor4C() {}
  RobotFunctor4C(T &obj, void (T::*func)(P1, P2, P3, P4)) :
    myObj(&obj), myFunc(func), myP1(), myP2(), myP3(), myP4() {}
  RobotFunctor4C(T &obj, void (T::*func)(P1, P2, P3, P4), P1 p1) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(), myP3(), myP4() {}


  RobotFunctor4C(T &obj, void (T::*func)(P1, P2, P3, P4), P1 p1, P2 p2) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(p2), myP3(), myP4() {}

  RobotFunctor4C(T &obj, void (T::*func)(P1, P2, P3, P4), P1 p1, P2 p2, P3 p3) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(p2), myP3(p3), myP4() {}


  RobotFunctor4C(T &obj, void (T::*func)(P1, P2, P3, P4), P1 p1, P2 p2, P3 p3, P4 p4) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(p2), myP3(p3), myP4(p4) {}

  RobotFunctor4C(T *obj, void (T::*func)(P1, P2, P3, P4)) :
    myObj(obj), myFunc(func), myP1(), myP2(), myP3(), myP4() {}


  RobotFunctor4C(T *obj, void (T::*func)(P1, P2, P3, P4), P1 p1) :
    myObj(obj), myFunc(func), myP1(p1), myP2(), myP3(), myP4() {}


  RobotFunctor4C(T *obj, void (T::*func)(P1, P2, P3, P4), P1 p1, P2 p2) :
    myObj(obj), myFunc(func), myP1(p1), myP2(p2), myP3(), myP4() {}

 
  RobotFunctor4C(T *obj, void (T::*func)(P1, P2, P3, P4), P1 p1, P2 p2, P3 p3) :
    myObj(obj), myFunc(func), myP1(p1), myP2(p2), myP3(p3), myP4() {}

  RobotFunctor4C(T *obj, void (T::*func)(P1, P2, P3, P4), P1 p1, P2 p2, P3 p3, P4 p4) :
    myObj(obj), myFunc(func), myP1(p1), myP2(p2), myP3(p3), myP4(p4) {}

  virtual ~RobotFunctor4C() {}

  virtual void invoke(void) {(myObj->*myFunc)(myP1, myP2, myP3, myP4);}

  virtual void invoke(P1 p1) {(myObj->*myFunc)(p1, myP2, myP3, myP4);}

  virtual void invoke(P1 p1, P2 p2) {(myObj->*myFunc)(p1, p2, myP3, myP4);}

  virtual void invoke(P1 p1, P2 p2, P3 p3) {(myObj->*myFunc)(p1, p2, p3, myP4);}

  virtual void invoke(P1 p1, P2 p2, P3 p3, P4 p4) {(myObj->*myFunc)(p1, p2, p3, p4);}

  virtual void setThis(T *obj) {myObj=obj;}

  virtual void setThis(T &obj) {myObj=&obj;}

  virtual void setP1(P1 p1) {myP1=p1;}

  virtual void setP2(P2 p2) {myP2=p2;}

  virtual void setP3(P3 p3) {myP3=p3;}

  virtual void setP4(P4 p4) {myP4=p4;}


protected:

  T *myObj;
  void (T::*myFunc)(P1, P2, P3, P4);
  P1 myP1;
  P2 myP2;
  P3 myP3;
  P4 myP4;
};



template<class Ret, class T>
class ArRetFunctorC : public ArRetFunctor<Ret>/*0参数，1返回值*/
{
public:
  ArRetFunctorC() {}

  ArRetFunctorC(T &obj, Ret (T::*func)(void)) : myObj(&obj), myFunc(func) {}

  ArRetFunctorC(T *obj, Ret (T::*func)(void)) : myObj(obj), myFunc(func) {}
  virtual ~ArRetFunctorC() {}
  virtual Ret invokeR(void) {return (myObj->*myFunc)();}

  virtual void setThis(T *obj) {myObj=obj;}
  virtual void setThis(T &obj) {myObj=&obj;}

protected:

  T *myObj;
  Ret (T::*myFunc)(void);
};


template<class Ret, class T, class P1>
class ArRetFunctor1C : public ArRetFunctor1<Ret, P1>/*1参数，1返回值*/
{
public:
  ArRetFunctor1C() {}
  ArRetFunctor1C(T &obj, Ret (T::*func)(P1)) :
    myObj(&obj), myFunc(func), myP1() {}
  ArRetFunctor1C(T &obj, Ret (T::*func)(P1), P1 p1) :
    myObj(&obj), myFunc(func), myP1(p1) {}

  ArRetFunctor1C(T *obj, Ret (T::*func)(P1)) :
    myObj(obj), myFunc(func), myP1() {}
  ArRetFunctor1C(T *obj, Ret (T::*func)(P1), P1 p1) :
    myObj(obj), myFunc(func), myP1(p1) {}
  virtual ~ArRetFunctor1C() {}
  virtual Ret invokeR(void) {return (myObj->*myFunc)(myP1);}
  virtual Ret invokeR(P1 p1) {return (myObj->*myFunc)(p1);}
  virtual void setThis(T *obj) {myObj=obj;}
  virtual void setThis(T &obj) {myObj=&obj;}
  virtual void setP1(P1 p1) {myP1=p1;}

protected:

  T *myObj;
  Ret (T::*myFunc)(P1);
  P1 myP1;
};

template<class Ret, class T, class P1, class P2>
class ArRetFunctor2C : public ArRetFunctor2<Ret, P1, P2>/*2参数，1返回值*/
{
public:
  ArRetFunctor2C() {}
  ArRetFunctor2C(T &obj, Ret (T::*func)(P1, P2)) :
    myObj(&obj), myFunc(func), myP1(), myP2() {}

  ArRetFunctor2C(T &obj, Ret (T::*func)(P1, P2), P1 p1) :
    myObj(&obj), myFunc(func), myP1(p1), myP2() {}

  ArRetFunctor2C(T &obj, Ret (T::*func)(P1, P2), P1 p1, P2 p2) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(p2) {}
  ArRetFunctor2C(T *obj, Ret (T::*func)(P1, P2)) :
    myObj(obj), myFunc(func), myP1(), myP2() {}
  ArRetFunctor2C(T *obj, Ret (T::*func)(P1, P2), P1 p1) :
    myObj(obj), myFunc(func), myP1(p1), myP2() {}

  ArRetFunctor2C(T *obj, Ret (T::*func)(P1, P2), P1 p1, P2 p2) :
    myObj(obj), myFunc(func), myP1(p1), myP2(p2) {}

  virtual ~ArRetFunctor2C() {}
  virtual Ret invokeR(void) {return (myObj->*myFunc)(myP1, myP2);}

  virtual Ret invokeR(P1 p1) {return (myObj->*myFunc)(p1, myP2);}
  virtual Ret invokeR(P1 p1, P2 p2) {return (myObj->*myFunc)(p1, p2);}
  virtual void setThis(T *obj) {myObj=obj;}
  virtual void setThis(T &obj) {myObj=&obj;}
  virtual void setP1(P1 p1) {myP1=p1;}
  virtual void setP2(P2 p2) {myP2=p2;}
protected:

  T *myObj;
  Ret (T::*myFunc)(P1, P2);
  P1 myP1;
  P2 myP2;
};


template<class Ret, class T, class P1, class P2, class P3>
class ArRetFunctor3C : public ArRetFunctor3<Ret, P1, P2, P3>/*3参数，1返回值*/
{
public:
  ArRetFunctor3C() {}
  ArRetFunctor3C(T &obj, Ret (T::*func)(P1, P2, P3)) :
    myObj(&obj), myFunc(func), myP1(), myP2(), myP3() {}
  ArRetFunctor3C(T &obj, Ret (T::*func)(P1, P2, P3), P1 p1) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(), myP3() {}
  ArRetFunctor3C(T &obj, Ret (T::*func)(P1, P2, P3), P1 p1, P2 p2) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(p2), myP3() {}

  ArRetFunctor3C(T &obj, Ret (T::*func)(P1, P2, P3), P1 p1, P2 p2, P3 p3) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(p2), myP3(p3) {}
  ArRetFunctor3C(T *obj, Ret (T::*func)(P1, P2, P3)) :
    myObj(obj), myFunc(func), myP1(), myP2(), myP3() {}
  ArRetFunctor3C(T *obj, Ret (T::*func)(P1, P2, P3), P1 p1) :
    myObj(obj), myFunc(func), myP1(p1), myP2(), myP3() {}

  ArRetFunctor3C(T *obj, Ret (T::*func)(P1, P2, P3), P1 p1, P2 p2) :
    myObj(obj), myFunc(func), myP1(p1), myP2(p2), myP3() {}

  ArRetFunctor3C(T *obj, Ret (T::*func)(P1, P2, P3), P1 p1, P2 p2, P3 p3) :
    myObj(obj), myFunc(func), myP1(p1), myP2(p2), myP3(p3) {}

  virtual ~ArRetFunctor3C() {}
  virtual Ret invokeR(void) {return (myObj->*myFunc)(myP1, myP2, myP3);}
  virtual Ret invokeR(P1 p1) {return (myObj->*myFunc)(p1, myP2, myP3);}
  virtual Ret invokeR(P1 p1, P2 p2) {return (myObj->*myFunc)(p1, p2, myP3);}
  virtual Ret invokeR(P1 p1, P2 p2, P3 p3) 
    {return (myObj->*myFunc)(p1, p2, p3);}
  virtual void setThis(T *obj) {myObj=obj;}
  virtual void setThis(T &obj) {myObj=&obj;}
  virtual void setP1(P1 p1) {myP1=p1;}
  virtual void setP2(P2 p2) {myP2=p2;}
  virtual void setP3(P3 p3) {myP3=p3;}
protected:
  T *myObj;
  Ret (T::*myFunc)(P1, P2, P3);
  P1 myP1;
  P2 myP2;
  P3 myP3;
};



template<class Ret, class T, class P1, class P2, class P3, class P4>
class ArRetFunctor4C : public ArRetFunctor4<Ret, P1, P2, P3, P4>/*4参数，1返回值*/
{
public:
  ArRetFunctor4C() {}
  ArRetFunctor4C(T &obj, Ret (T::*func)(P1, P2, P3, P4)) :
    myObj(&obj), myFunc(func), myP1(), myP2(), myP3(), myP4() {}
  ArRetFunctor4C(T &obj, Ret (T::*func)(P1, P2, P3, P4), P1 p1) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(), myP3(), myP4() {}
  ArRetFunctor4C(T &obj, Ret (T::*func)(P1, P2, P3, P4), P1 p1, P2 p2) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(p2), myP3(), myP4() {}
  ArRetFunctor4C(T &obj, Ret (T::*func)(P1, P2, P3, P4), P1 p1, P2 p2, P3 p3) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(p2), myP3(p3), myP4() {}
  ArRetFunctor4C(T &obj, Ret (T::*func)(P1, P2, P3, P4), P1 p1, P2 p2, P3 p3, P4 p4) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(p2), myP3(p3), myP4(p4) {}
  ArRetFunctor4C(T *obj, Ret (T::*func)(P1, P2, P3, P4)) :
    myObj(obj), myFunc(func), myP1(), myP2(), myP3(), myP4() {}
  ArRetFunctor4C(T *obj, Ret (T::*func)(P1, P2, P3, P4), P1 p1) :
    myObj(obj), myFunc(func), myP1(p1), myP2(), myP3(), myP4() {}
  ArRetFunctor4C(T *obj, Ret (T::*func)(P1, P2, P3, P4), P1 p1, P2 p2) :
    myObj(obj), myFunc(func), myP1(p1), myP2(p2), myP3(), myP4() {}
  ArRetFunctor4C(T *obj, Ret (T::*func)(P1, P2, P3, P4), P1 p1, P2 p2, P3 p3) :
    myObj(obj), myFunc(func), myP1(p1), myP2(p2), myP3(p3), myP4() {}
  ArRetFunctor4C(T *obj, Ret (T::*func)(P1, P2, P3, P4), P1 p1, P2 p2, P3 p3, P4 p4) :
    myObj(obj), myFunc(func), myP1(p1), myP2(p2), myP3(p3), myP4(p4) {}


  virtual ~ArRetFunctor4C() {}


  virtual Ret invokeR(void) {return (myObj->*myFunc)(myP1, myP2, myP3, myP4);}
  virtual Ret invokeR(P1 p1) {return (myObj->*myFunc)(p1, myP2, myP3, myP4);}
  virtual Ret invokeR(P1 p1, P2 p2) {return (myObj->*myFunc)(p1, p2, myP3, myP4);}
  virtual Ret invokeR(P1 p1, P2 p2, P3 p3) 
    {return (myObj->*myFunc)(p1, p2, p3, myP4);}

  virtual Ret invokeR(P1 p1, P2 p2, P3 p3, P4 p4) 
    {return (myObj->*myFunc)(p1, p2, p3, p4);}

  virtual void setThis(T *obj) {myObj=obj;}

  virtual void setThis(T &obj) {myObj=&obj;}

  virtual void setP1(P1 p1) {myP1=p1;}

  virtual void setP2(P2 p2) {myP2=p2;}

  virtual void setP3(P3 p3) {myP3=p3;}

  virtual void setP4(P4 p4) {myP4=p4;}

protected:

  T *myObj;
  Ret (T::*myFunc)(P1, P2, P3, P4);
  P1 myP1;
  P2 myP2;
  P3 myP3;
  P4 myP4;
};


/*以下为类的const成员函数*/
#ifndef SWIG
template<class T>
class ArConstFunctorC : public RobotFunctor/*0参数，0返回值*/
{
public:
  ArConstFunctorC() {}
  ArConstFunctorC(T &obj, void (T::*func)(void) const) : myObj(&obj), myFunc(func) {}
  ArConstFunctorC(T *obj, void (T::*func)(void) const) : myObj(obj), myFunc(func) {}
  virtual ~ArConstFunctorC() {}
  virtual void invoke(void) {(myObj->*myFunc)();}
  virtual void setThis(T *obj) {myObj=obj;}
  virtual void setThis(T &obj) {myObj=&obj;}
protected:
  T *myObj;
  void (T::*myFunc)(void) const;
};

template<class T, class P1>
class ArConstFunctor1C : public RobotFunctor1<P1>/*1参数，0返回值*/
{
public:
  ArConstFunctor1C() {}
  ArConstFunctor1C(T &obj, void (T::*func)(P1) const) :
    myObj(&obj), myFunc(func), myP1() {}

  ArConstFunctor1C(T &obj, void (T::*func)(P1) const, P1 p1) :
    myObj(&obj), myFunc(func), myP1(p1) {}

  ArConstFunctor1C(T *obj, void (T::*func)(P1) const) :
    myObj(obj), myFunc(func), myP1() {}
  ArConstFunctor1C(T *obj, void (T::*func)(P1) const, P1 p1) :
    myObj(obj), myFunc(func), myP1(p1) {}
  virtual ~ArConstFunctor1C() {}
  virtual void invoke(void) {(myObj->*myFunc)(myP1);}
  virtual void invoke(P1 p1) {(myObj->*myFunc)(p1);}
  virtual void setThis(T *obj) {myObj=obj;}
  virtual void setThis(T &obj) {myObj=&obj;}
  virtual void setP1(P1 p1) {myP1=p1;}
protected:
  T *myObj;
  void (T::*myFunc)(P1) const;
  P1 myP1;
};


template<class T, class P1, class P2>
class ArConstFunctor2C : public RobotFunctor2<P1, P2>/*2参数，0返回值*/
{
public:
  ArConstFunctor2C() {}
  ArConstFunctor2C(T &obj, void (T::*func)(P1, P2) const) :
    myObj(&obj), myFunc(func), myP1(), myP2() {}
  ArConstFunctor2C(T &obj, void (T::*func)(P1, P2) const, P1 p1) :
    myObj(&obj), myFunc(func), myP1(p1), myP2() {}
  ArConstFunctor2C(T &obj, void (T::*func)(P1, P2) const, P1 p1, P2 p2) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(p2) {}
  ArConstFunctor2C(T *obj, void (T::*func)(P1, P2) const) :
    myObj(obj), myFunc(func), myP1(), myP2() {}
  ArConstFunctor2C(T *obj, void (T::*func)(P1, P2) const, P1 p1) :
    myObj(obj), myFunc(func), myP1(p1), myP2() {}
  ArConstFunctor2C(T *obj, void (T::*func)(P1, P2) const, P1 p1, P2 p2) :
    myObj(obj), myFunc(func), myP1(p1), myP2(p2) {}


  virtual ~ArConstFunctor2C() {}

  virtual void invoke(void) {(myObj->*myFunc)(myP1, myP2);}
  virtual void invoke(P1 p1) {(myObj->*myFunc)(p1, myP2);}
  virtual void invoke(P1 p1, P2 p2) {(myObj->*myFunc)(p1, p2);}

  virtual void setThis(T *obj) {myObj=obj;}
  virtual void setThis(T &obj) {myObj=&obj;}
  virtual void setP1(P1 p1) {myP1=p1;}
  virtual void setP2(P2 p2) {myP2=p2;}
protected:
  T *myObj;
  void (T::*myFunc)(P1, P2) const;
  P1 myP1;
  P2 myP2;
};

template<class T, class P1, class P2, class P3>
class ArConstFunctor3C : public RobotFunctor3<P1, P2, P3>/*3参数，0返回值*/
{
public:
  ArConstFunctor3C() {}
  ArConstFunctor3C(T &obj, void (T::*func)(P1, P2, P3) const) :
    myObj(&obj), myFunc(func), myP1(), myP2(), myP3() {}

  ArConstFunctor3C(T &obj, void (T::*func)(P1, P2, P3) const, P1 p1) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(), myP3() {}
  ArConstFunctor3C(T &obj, void (T::*func)(P1, P2, P3) const, P1 p1, P2 p2) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(p2), myP3() {}
  ArConstFunctor3C(T &obj, void (T::*func)(P1, P2, P3) const, P1 p1, P2 p2, P3 p3) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(p2), myP3(p3) {}
  ArConstFunctor3C(T *obj, void (T::*func)(P1, P2, P3) const) :
    myObj(obj), myFunc(func), myP1(), myP2(), myP3() {}

  ArConstFunctor3C(T *obj, void (T::*func)(P1, P2, P3) const, P1 p1) :
    myObj(obj), myFunc(func), myP1(p1), myP2(), myP3() {}
  ArConstFunctor3C(T *obj, void (T::*func)(P1, P2, P3) const, P1 p1, P2 p2) :
    myObj(obj), myFunc(func), myP1(p1), myP2(p2), myP3() {} 
  ArConstFunctor3C(T *obj, void (T::*func)(P1, P2, P3) const, P1 p1, P2 p2, P3 p3) :
    myObj(obj), myFunc(func), myP1(p1), myP2(p2), myP3(p3) {}


  virtual ~ArConstFunctor3C() {}
  virtual void invoke(void) {(myObj->*myFunc)(myP1, myP2, myP3);}
  virtual void invoke(P1 p1) {(myObj->*myFunc)(p1, myP2, myP3);}
  virtual void invoke(P1 p1, P2 p2) {(myObj->*myFunc)(p1, p2, myP3);}
  virtual void invoke(P1 p1, P2 p2, P3 p3) {(myObj->*myFunc)(p1, p2, p3);}
  virtual void setThis(T *obj) {myObj=obj;}
  virtual void setThis(T &obj) {myObj=&obj;}
  virtual void setP1(P1 p1) {myP1=p1;}
  virtual void setP2(P2 p2) {myP2=p2;}
  virtual void setP3(P3 p3) {myP3=p3;}
protected:

  T *myObj;
  void (T::*myFunc)(P1, P2, P3) const;
  P1 myP1;
  P2 myP2;
  P3 myP3;
};

template<class T, class P1, class P2, class P3, class P4>
class ArConstFunctor4C : public RobotFunctor4<P1, P2, P3, P4>
{
public:
  ArConstFunctor4C() {}
  ArConstFunctor4C(T &obj, void (T::*func)(P1, P2, P3, P4) const) :
    myObj(&obj), myFunc(func), myP1(), myP2(), myP3(), myP4() {}
  ArConstFunctor4C(T &obj, void (T::*func)(P1, P2, P3, P4), P1 p1) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(), myP3(), myP4() {}
  ArConstFunctor4C(T &obj, void (T::*func)(P1, P2, P3, P4), P1 p1, P2 p2) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(p2), myP3(), myP4() {}
  ArConstFunctor4C(T &obj, void (T::*func)(P1, P2, P3, P4), P1 p1, P2 p2, P3 p3) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(p2), myP3(p3), myP4() {}
  ArConstFunctor4C(T &obj, void (T::*func)(P1, P2, P3, P4), P1 p1, P2 p2, P3 p3, P4 p4) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(p2), myP3(p3), myP4(p4) {}
  ArConstFunctor4C(T *obj, void (T::*func)(P1, P2, P3, P4) const) :
    myObj(obj), myFunc(func), myP1(), myP2(), myP3(), myP4() {}
  ArConstFunctor4C(T *obj, void (T::*func)(P1, P2, P3, P4), P1 p1) :
    myObj(obj), myFunc(func), myP1(p1), myP2(), myP3(), myP4() {}
  ArConstFunctor4C(T *obj, void (T::*func)(P1, P2, P3, P4), P1 p1, P2 p2) :
    myObj(obj), myFunc(func), myP1(p1), myP2(p2), myP3(), myP4() {}
  ArConstFunctor4C(T *obj, void (T::*func)(P1, P2, P3, P4), P1 p1, P2 p2, P3 p3) :
    myObj(obj), myFunc(func), myP1(p1), myP2(p2), myP3(p3), myP4() {}
  ArConstFunctor4C(T *obj, void (T::*func)(P1, P2, P3, P4), P1 p1, P2 p2, P3 p3, P4 p4) :
    myObj(obj), myFunc(func), myP1(p1), myP2(p2), myP3(p3), myP4(p4) {}

  virtual ~ArConstFunctor4C() {}

  virtual void invoke(void) {(myObj->*myFunc)(myP1, myP2, myP3, myP4);}
  virtual void invoke(P1 p1) {(myObj->*myFunc)(p1, myP2, myP3, myP4);}
  virtual void invoke(P1 p1, P2 p2) {(myObj->*myFunc)(p1, p2, myP3, myP4);}
  virtual void invoke(P1 p1, P2 p2, P3 p3) {(myObj->*myFunc)(p1, p2, p3, myP4);}
  virtual void invoke(P1 p1, P2 p2, P3 p3, P4 p4) {(myObj->*myFunc)(p1, p2, p3, p4);}
  virtual void setThis(T *obj) {myObj=obj;}
  virtual void setThis(T &obj) {myObj=&obj;}
  virtual void setP1(P1 p1) {myP1=p1;}
  virtual void setP2(P2 p2) {myP2=p2;}
  virtual void setP3(P3 p3) {myP3=p3;}
  virtual void setP4(P4 p4) {myP4=p4;}
protected:
  T *myObj;
  void (T::*myFunc)(P1, P2, P3, P4) const;
  P1 myP1;
  P2 myP2;
  P3 myP3;
  P4 myP4;
};






/*const成员函数且有返回值*/

template<class Ret, class T>
class ArConstRetFunctorC : public ArRetFunctor<Ret>
{
public:
  ArConstRetFunctorC() {}
  ArConstRetFunctorC(T &obj, Ret (T::*func)(void) const) : myObj(&obj), myFunc(func) {}
  ArConstRetFunctorC(T *obj, Ret (T::*func)(void) const) : myObj(obj), myFunc(func) {}
  virtual ~ArConstRetFunctorC() {}
  virtual Ret invokeR(void) {return (myObj->*myFunc)();}
  virtual void setThis(T *obj) {myObj=obj;}
  virtual void setThis(T &obj) {myObj=&obj;}
protected:
  T *myObj;
  Ret (T::*myFunc)(void) const;
};

template<class Ret, class T, class P1>
class ArConstRetFunctor1C : public ArRetFunctor1<Ret, P1>
{
public:
  ArConstRetFunctor1C() {}

  ArConstRetFunctor1C(T &obj, Ret (T::*func)(P1) const) :
    myObj(&obj), myFunc(func), myP1() {}
  ArConstRetFunctor1C(T &obj, Ret (T::*func)(P1) const, P1 p1) :
    myObj(&obj), myFunc(func), myP1(p1) {}
  ArConstRetFunctor1C(T *obj, Ret (T::*func)(P1) const) :
    myObj(obj), myFunc(func), myP1() {}

  ArConstRetFunctor1C(T *obj, Ret (T::*func)(P1) const, P1 p1) :
    myObj(obj), myFunc(func), myP1(p1) {}


  virtual ~ArConstRetFunctor1C() {}
  virtual Ret invokeR(void) {return (myObj->*myFunc)(myP1);}
  virtual Ret invokeR(P1 p1) {return (myObj->*myFunc)(p1);}
  virtual void setThis(T *obj) {myObj=obj;}
  virtual void setThis(T &obj) {myObj=&obj;}
  virtual void setP1(P1 p1) {myP1=p1;}
protected:
  T *myObj;
  Ret (T::*myFunc)(P1) const;
  P1 myP1;
};
template<class Ret, class T, class P1, class P2>
class ArConstRetFunctor2C : public ArRetFunctor2<Ret, P1, P2>
{
public:
  ArConstRetFunctor2C() {}
  ArConstRetFunctor2C(T &obj, Ret (T::*func)(P1, P2) const) :
    myObj(&obj), myFunc(func), myP1(), myP2() {}
  ArConstRetFunctor2C(T &obj, Ret (T::*func)(P1, P2) const, P1 p1) :
    myObj(&obj), myFunc(func), myP1(p1), myP2() {}
  ArConstRetFunctor2C(T &obj, Ret (T::*func)(P1, P2) const, P1 p1, P2 p2) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(p2) {}
  ArConstRetFunctor2C(T *obj, Ret (T::*func)(P1, P2) const) :
    myObj(obj), myFunc(func), myP1(), myP2() {}
  ArConstRetFunctor2C(T *obj, Ret (T::*func)(P1, P2) const, P1 p1) :
    myObj(obj), myFunc(func), myP1(p1), myP2() {}
  ArConstRetFunctor2C(T *obj, Ret (T::*func)(P1, P2) const, P1 p1, P2 p2) :
    myObj(obj), myFunc(func), myP1(p1), myP2(p2) {}

  virtual ~ArConstRetFunctor2C() {}
  virtual Ret invokeR(void) {return (myObj->*myFunc)(myP1, myP2);}
  virtual Ret invokeR(P1 p1) {return (myObj->*myFunc)(p1, myP2);}
  virtual Ret invokeR(P1 p1, P2 p2) {return (myObj->*myFunc)(p1, p2);}
  virtual void setThis(T *obj) {myObj=obj;}
  virtual void setThis(T &obj) {myObj=&obj;}
  virtual void setP1(P1 p1) {myP1=p1;}
  virtual void setP2(P2 p2) {myP2=p2;}
protected:
  T *myObj;
  Ret (T::*myFunc)(P1, P2) const;
  P1 myP1;
  P2 myP2;
};

template<class Ret, class T, class P1, class P2, class P3>
class ArConstRetFunctor3C : public ArRetFunctor3<Ret, P1, P2, P3>
{
public:
  ArConstRetFunctor3C() {}
  ArConstRetFunctor3C(T &obj, Ret (T::*func)(P1, P2, P3) const) :
    myObj(&obj), myFunc(func), myP1(), myP2(), myP3() {}
  ArConstRetFunctor3C(T &obj, Ret (T::*func)(P1, P2, P3) const, P1 p1) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(), myP3() {}
  ArConstRetFunctor3C(T &obj, Ret (T::*func)(P1, P2, P3) const, P1 p1, P2 p2) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(p2), myP3() {}
  ArConstRetFunctor3C(T &obj, Ret (T::*func)(P1, P2, P3) const, P1 p1, P2 p2, P3 p3) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(p2), myP3(p3) {}
  ArConstRetFunctor3C(T *obj, Ret (T::*func)(P1, P2, P3) const) :
    myObj(obj), myFunc(func), myP1(), myP2(), myP3() {}
  ArConstRetFunctor3C(T *obj, Ret (T::*func)(P1, P2, P3) const, P1 p1) :
    myObj(obj), myFunc(func), myP1(p1), myP2(), myP3() {}
  ArConstRetFunctor3C(T *obj, Ret (T::*func)(P1, P2, P3) const, P1 p1, P2 p2) :
    myObj(obj), myFunc(func), myP1(p1), myP2(p2), myP3() {}
  ArConstRetFunctor3C(T *obj, Ret (T::*func)(P1, P2, P3) const, P1 p1, P2 p2, P3 p3) :
    myObj(obj), myFunc(func), myP1(p1), myP2(p2), myP3(p3) {}

  virtual ~ArConstRetFunctor3C() {}
  virtual Ret invokeR(void) {return (myObj->*myFunc)(myP1, myP2, myP3);}
  virtual Ret invokeR(P1 p1) {return (myObj->*myFunc)(p1, myP2, myP3);}
  virtual Ret invokeR(P1 p1, P2 p2) {return (myObj->*myFunc)(p1, p2, myP3);}
  virtual Ret invokeR(P1 p1, P2 p2, P3 p3) 
    {return (myObj->*myFunc)(p1, p2, p3);}
  virtual void setThis(T *obj) {myObj=obj;}
  virtual void setThis(T &obj) {myObj=&obj;}
  virtual void setP1(P1 p1) {myP1=p1;}
  virtual void setP2(P2 p2) {myP2=p2;}
  virtual void setP3(P3 p3) {myP3=p3;}
protected:
  T *myObj;
  Ret (T::*myFunc)(P1, P2, P3) const;
  P1 myP1;
  P2 myP2;
  P3 myP3;
};

template<class Ret, class T, class P1, class P2, class P3, class P4>
class ArConstRetFunctor4C : public ArRetFunctor4<Ret, P1, P2, P3, P4>
{
public:
  ArConstRetFunctor4C() {}
  ArConstRetFunctor4C(T &obj, Ret (T::*func)(P1, P2, P3, P4) const) :
    myObj(&obj), myFunc(func), myP1(), myP2(), myP3(), myP4() {}
  ArConstRetFunctor4C(T &obj, Ret (T::*func)(P1, P2, P3, P4) const, P1 p1) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(), myP3(), myP4() {}
  ArConstRetFunctor4C(T &obj, Ret (T::*func)(P1, P2, P3, P4) const, P1 p1, P2 p2) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(p2), myP3(), myP4() {}
  ArConstRetFunctor4C(T &obj, Ret (T::*func)(P1, P2, P3, P4) const, P1 p1, P2 p2, P3 p3) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(p2), myP3(p3), myP4() {}
  ArConstRetFunctor4C(T &obj, Ret (T::*func)(P1, P2, P3, P4) const, P1 p1, P2 p2, P3 p3, P4 p4) :
    myObj(&obj), myFunc(func), myP1(p1), myP2(p2), myP3(p3), myP4(p4) {}
  ArConstRetFunctor4C(T *obj, Ret (T::*func)(P1, P2, P3, P4) const) :
    myObj(obj), myFunc(func), myP1(), myP2(), myP3(), myP4() {}
  ArConstRetFunctor4C(T *obj, Ret (T::*func)(P1, P2, P3, P4) const, P1 p1) :
    myObj(obj), myFunc(func), myP1(p1), myP2(), myP3(), myP4() {}
  ArConstRetFunctor4C(T *obj, Ret (T::*func)(P1, P2, P3, P4) const, P1 p1, P2 p2) :
    myObj(obj), myFunc(func), myP1(p1), myP2(p2), myP3(), myP4() {}
  ArConstRetFunctor4C(T *obj, Ret (T::*func)(P1, P2, P3, P4) const, P1 p1, P2 p2, P3 p3) :
    myObj(obj), myFunc(func), myP1(p1), myP2(p2), myP3(p3), myP4() {}
  ArConstRetFunctor4C(T *obj, Ret (T::*func)(P1, P2, P3, P4) const, P1 p1, P2 p2, P3 p3, P4 p4) :
    myObj(obj), myFunc(func), myP1(p1), myP2(p2), myP3(p3), myP4(p4) {}

  virtual ~ArConstRetFunctor4C() {}

  virtual Ret invokeR(void) {return (myObj->*myFunc)(myP1, myP2, myP3, myP4);}
  virtual Ret invokeR(P1 p1) {return (myObj->*myFunc)(p1, myP2, myP3, myP4);}
  virtual Ret invokeR(P1 p1, P2 p2) {return (myObj->*myFunc)(p1, p2, myP3, myP4);}
  virtual Ret invokeR(P1 p1, P2 p2, P3 p3) 
    {return (myObj->*myFunc)(p1, p2, p3, myP4);}
  virtual Ret invokeR(P1 p1, P2 p2, P3 p3, P4 p4) 
    {return (myObj->*myFunc)(p1, p2, p3, p4);}
  virtual void setThis(T *obj) {myObj=obj;}
  virtual void setThis(T &obj) {myObj=&obj;}
  virtual void setP1(P1 p1) {myP1=p1;}
  virtual void setP2(P2 p2) {myP2=p2;}
  virtual void setP3(P3 p3) {myP3=p3;}
  virtual void setP4(P4 p4) {myP4=p4;}
protected:
  T *myObj;
  Ret (T::*myFunc)(P1, P2, P3, P4) const;
  P1 myP1;
  P2 myP2;
  P3 myP3;
  P4 myP4;
};

#endif
#ifdef ROBOT_NAMESPACE
}
#endif
#endif // ROBOT_MAIN_FUNCTOR_H_
