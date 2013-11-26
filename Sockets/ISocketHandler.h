#ifndef _SOCKETS_ISocketHandler_H
#define _SOCKETS_ISocketHandler_H
#include "sockets-config.h"

#include <list>
#include <map>

#include "socket_include.h"
#include "Socket.h"
#include "StdLog.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif

class SocketAddress;
class IMutex;


/*Socket管理器，事件发生器*/
class ISocketHandler
{
	friend class Socket;

public:
	//连接池，不用的连接留在池中，在下一次需要连接时直接使用
	//管理器内部使用
#ifdef ENABLE_POOL
	class PoolSocket : public Socket
	{
	public:
		PoolSocket(ISocketHandler& h,Socket *src) : Socket(h) 
		{
			CopyConnection( src );
			SetIsClient();
		}

		//在连接池中的Socket都是休眠等待以后使用，所以不应该收到数据
		void OnRead() 
		{
			Handler().LogError(this, "OnRead", 0, "休眠的SOCKET收到数据", LOG_LEVEL_FATAL);
			SetCloseAndDelete();
		}
		//不能设置socket选项
		void OnOptions(int,int,int,SOCKET) {}

	};
#endif

public:
	virtual ~ISocketHandler() {}

	//返回创建的另外一个管理器
	virtual ISocketHandler *Create(StdLog * = NULL) = 0;

	//返回创建的另外一个带锁管理器
	virtual ISocketHandler *Create(IMutex&, ISocketHandler&, StdLog * = NULL) = 0;

	//该管理器的父管理器有效性判断。子管理器由Create返回
	virtual bool ParentHandlerIsValid() = 0;

	//返回父亲管理器
	virtual ISocketHandler& ParentHandler() = 0;


	/** Get thread handler with least connections */
	virtual ISocketHandler& GetRandomHandler() = 0;

	/** Return parent handler if valid, otherwise return normal handler */
	virtual ISocketHandler& GetEffectiveHandler() = 0;

	//使能线程
	virtual void SetNumberOfThreads(size_t n) = 0;

	//线程使能状态返回
	virtual bool IsThreaded() = 0;

	/** Enable select release */
	virtual void EnableRelease() = 0;

	/** Make select release */
	virtual void Release() = 0;

	//在多线程时需要使用线程安全锁
	virtual IMutex& GetMutex() const = 0;

	//注册Log
	virtual void RegStdLog(StdLog *log) = 0;

	//log日志
	virtual void LogError(Socket *p,const std::string& user_text,int err,const std::string& sys_err,loglevel_t t = LOG_LEVEL_WARNING) = 0;

	//给管理器map增加Socket，Socket通常不需要手动删除
	virtual void Add(Socket *) = 0;

protected:
	//从socket map中删除该Socket，被Socket使用来删除自身
	virtual void Remove(Socket *) = 0;

	//实际的select执行
	virtual int ISocketHandler_Select(struct timeval *) = 0;

public:
	//增加，修改，删除描述符集合
	virtual void ISocketHandler_Add(Socket *,bool bRead,bool bWrite) = 0;
	virtual void ISocketHandler_Mod(Socket *,bool bRead,bool bWrite) = 0;
	virtual void ISocketHandler_Del(Socket *) = 0;

	//在给定时间内等待事件发生
	virtual int Select(long sec,long usec) = 0;
	//一直阻塞到事件发生
	virtual int Select() = 0;
	virtual int Select(struct timeval *tsel) = 0;

	//检查一个Soket是否被该Socket Handler管理
	virtual bool Valid(Socket *) = 0;
	//同上，这里用每个Socket的uid来标识
	virtual bool Valid(socketuid_t) = 0;

	//返回该Socket Handler管理的Sockets个数
	virtual size_t GetCount() = 0;

	//返回该 Socket Handler允许管理的最大sockets个数
	virtual size_t MaxCount() = 0;

	//检测p指向的Socket是否准备好accept来的连接请求
	virtual bool OkToAccept(Socket *p) = 0;

	//返回该管理器管理的所有Socket,注意在多线程的时候需要lock  h.GetMutex()
	virtual const std::map<SOCKET, Socket *>& AllSockets() = 0;

	/** Override to accept longer lines than TCP_LINE_SIZE */
	virtual size_t MaxTcpLineSize() = 0;

	virtual void SetCallOnConnect(bool = true) = 0;
	virtual void SetDetach(bool = true) = 0;
	virtual void SetTimeout(bool = true) = 0;
	virtual void SetRetry(bool = true) = 0;
	virtual void SetClose(bool = true) = 0;


	// -------------------------------------------------------------------------
	//连接池 
	// -------------------------------------------------------------------------
#ifdef ENABLE_POOL
	//在连接池中根据协议类型，套接字类型来查找可用的连接
	virtual ISocketHandler::PoolSocket *FindConnection(int type,const std::string& protocol,SocketAddress&) = 0;
	//使能连接池
	virtual void EnablePool(bool = true) = 0;
	//返回连接池使能状态
	virtual bool PoolEnabled() = 0;
#endif // ENABLE_POOL


	// -------------------------------------------------------------------------
	// 异步DNS解析服务器管理
	//用于防止同步的gethostbyname()解析出现阻塞
	// -------------------------------------------------------------------------
#ifdef ENABLE_RESOLVER
	//使能异步DNS，参数为异步DNS服务器端口
	virtual void EnableResolver(port_t = 16667) = 0;
	//返回true表示异步DNS可用
	virtual bool ResolverEnabled() = 0;
	//排队一个异步DNS请求
	///host为主机名hostname，即需要解析的主机名
	///port为在Socket::OnResolved回调时返回
	virtual int Resolve(Socket *,const std::string& host,port_t port) = 0;
	//逆向DNS查询，根据地址查hostname
	virtual int Resolve(Socket *,ipaddr_t a) = 0;
	//获取异步DNS服务器监听端口
	virtual port_t GetResolverPort() = 0;
	//解析线程已经准备后查询DNS
	virtual bool ResolverReady() = 0;
	//如果Socekt正在等待一个解析事件则返回true
	virtual bool Resolving(Socket *) = 0;
#endif // ENABLE_RESOLVER



#ifdef ENABLE_DETACH
	//设置该socket handler工作在SocketThread上
	virtual void SetSlave(bool x = true) = 0;
	//返回该handler是否运行在Sokethread上
	virtual bool IsSlave() = 0;
#endif // ENABLE_DETACH

};


#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _SOCKETS_ISocketHandler_H

