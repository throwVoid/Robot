#ifndef _SOCKETS_SocketHandler_H
#define _SOCKETS_SocketHandler_H

#include "sockets-config.h"
#include <map>
#include <list>

#include "socket_include.h"
#include "ISocketHandler.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


class Socket;
#ifdef ENABLE_RESOLVER
class ResolvServer;
#endif
class IMutex;
class SocketHandlerThread;
class UdpSocket;

class SocketHandler : public ISocketHandler
{
protected:
	//map，用于索引文件描述符与Socket结构指针
	typedef std::map<SOCKET,Socket *> socket_m;

public:
	//ctor，log为日志记录类的指针
	SocketHandler(StdLog *log = NULL);

	//线程安全构造函数
	SocketHandler(IMutex& mutex,StdLog *log = NULL);

	//指明父亲socket handler的构造函数
	SocketHandler(IMutex&, ISocketHandler& parent, StdLog * = NULL);

	~SocketHandler();

	//作为父handler,创建一个子handler
	virtual ISocketHandler *Create(StdLog * = NULL);
	virtual ISocketHandler *Create(IMutex&, ISocketHandler&, StdLog * = NULL);

	//父亲socket handler是否可用
	virtual bool ParentHandlerIsValid();

	//返回父亲handler
	virtual ISocketHandler& ParentHandler();

	virtual ISocketHandler& GetRandomHandler();

	virtual ISocketHandler& GetEffectiveHandler();

	virtual void SetNumberOfThreads(size_t n);
	virtual bool IsThreaded();

	virtual void EnableRelease();
	virtual void Release();

	//用于线程操作时获取互斥锁
	IMutex& GetMutex() const;

	void RegStdLog(StdLog *log);

	void LogError(Socket *p,const std::string& user_text,int err,const std::string& sys_err,loglevel_t t = LOG_LEVEL_WARNING);

	//增加Socket到handler管理的map中
	void Add(Socket *);

	//增加，修改，删除可读可写描述集合
	void ISocketHandler_Add(Socket *,bool bRead,bool bWrite);
	void ISocketHandler_Mod(Socket *,bool bRead,bool bWrite);
	void ISocketHandler_Del(Socket *);

	//等待指定事件内的事件
	int Select(long sec,long usec);

	//一直阻塞到有事件发生
	int Select();

	//以时间结构作为参数的select
	int Select(struct timeval *tsel);

	//检测某套接字Socket是否被该handler管理
	bool Valid(Socket *);
	bool Valid(socketuid_t);

	size_t GetCount();
	size_t MaxCount() { return FD_SETSIZE; }

	bool OkToAccept(Socket *p);

	const std::map<SOCKET, Socket *>& AllSockets() { return m_sockets; }

	size_t MaxTcpLineSize() { return TCP_LINE_SIZE; }

	void SetCallOnConnect(bool = true);
	void SetDetach(bool = true);
	void SetTimeout(bool = true);
	void SetRetry(bool = true);
	void SetClose(bool = true);

private:
	//文件流
	static FILE *m_event_file;
	//事件计数
	static unsigned long m_event_counter;

public:
//连接池
#ifdef ENABLE_POOL
	//查找可用的已经open的连接
	ISocketHandler::PoolSocket *FindConnection(int type,const std::string& protocol,SocketAddress&);
	void EnablePool(bool x = true);
	bool PoolEnabled();
#endif // ENABLE_POOL


	// DNS 解析服务器
#ifdef ENABLE_RESOLVER
		//使能异步DNS
	void EnableResolver(port_t port = 16667);
	bool ResolverEnabled();
	//排队一个dns请求
	int Resolve(Socket *,const std::string& host,port_t port);
	//逆向DNS
	int Resolve(Socket *,ipaddr_t a);
	//获取异步DNS服务器端口
	port_t GetResolverPort();
	//解析线程已经准备查询dns
	bool ResolverReady();
	//Socket正在等待一个解析event则返回true
	bool Resolving(Socket *);
#endif // ENABLE_RESOLVER

#ifdef ENABLE_DETACH
	//设置该socket handler工作在SocketThread上
	void SetSlave(bool x = true);
	//返回该handler是否运行在Sokethread上
	bool IsSlave();
#endif

protected:
	socket_m m_sockets; ///< 存放活跃的socket 的map
	std::list<Socket *> m_add; ///< 被增加到socket map的Socket
	std::list<Socket *> m_delete; ///< 当Add时失败后被删除的Socket

protected:
	int ISocketHandler_Select(struct timeval *);
	//Socket使用的接口，从socket map中删除Socket
	void Remove(Socket *);
	void DeleteSocket(Socket *);
	void AddIncoming();
	void CheckErasedSockets();
	void CheckCallOnConnect();
	void CheckDetach();
	void CheckTimeout(time_t);
	void CheckRetry();
	void CheckClose();
	//
	StdLog *m_stdlog; ///< log指针
	IMutex& m_mutex; ///< 线程安全锁
	bool m_b_use_mutex; ///< 锁已经被正确初始化
	ISocketHandler& m_parent;///< 父亲socket hanlder
	bool m_b_parent_is_valid;///<父亲handler是否可用

private:
	//重构描述符集合
	void RebuildFdset();
	//设置socket描述符到集合
	void Set(Socket *,bool,bool);
	//
	std::list<SocketHandlerThread *> m_threads;
	UdpSocket *m_release;
	//
	SOCKET m_maxsock; ///< select时使用的最大描述符参数
	fd_set m_rfds; ///< 读集合
	fd_set m_wfds; ///< 写集合
	fd_set m_efds; ///< 出错集合
	time_t m_tlast; ///< 超时控制

	// 状态链表
	std::list<socketuid_t> m_fds_erase; ///< 从m_sockets中erase的Socket

	bool m_b_check_callonconnect;
	bool m_b_check_detach;
	bool m_b_check_timeout;
	bool m_b_check_retry;
	bool m_b_check_close;

#ifdef ENABLE_RESOLVER
	int m_resolv_id; ///<解析ID计数
	ResolvServer *m_resolver; ///< 解析服务器线程指针
	port_t m_resolver_port; ///< 解析服务器监听端口
	std::map<socketuid_t, bool> m_resolve_q; ///< 待解析队列
#endif
#ifdef ENABLE_POOL
	bool m_b_enable_pool; ///<连接池使能标识 
#endif
#ifdef ENABLE_DETACH
	bool m_slave; ///< 指示该handler运行在SocketThread
#endif
};


#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _SOCKETS_SocketHandler_H

