#ifndef _SOCKETS_Socket_H
#define _SOCKETS_Socket_H
#include "sockets-config.h"

#include <string>
#include <vector>
#include <list>
#include "socket_include.h"
#ifdef HAVE_OPENSSL
#include <openssl/ssl.h>
#endif

#include <time.h>
#include "SocketAddress.h"
#include "Thread.h"


#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


class ISocketHandler;
class SocketAddress;
class IFile;
class SocketThread;


//套接字基类
class Socket
{
public:
	Socket(ISocketHandler&);

	virtual ~Socket();

	//当需要一个非标准的构造函数来创建一个对象是使用。派生类一般重实现为使用默认构造函数
	virtual Socket *Create() { return NULL; }

	//返回拥有管理该Socket的handler,如果该Socket已经被Detach，则返回的是slave SocketHandler
	ISocketHandler& Handler() const;

	//即使被Detach，也返回最初的socket Handler
	ISocketHandler& MasterHandler() const;

	/** Called by ListenSocket after accept but before socket is added to handler.
	 * CTcpSocket uses this to create its ICrypt member variable.
	 * The ICrypt member variable is created by a virtual method, therefore
	 * it can't be called directly from the CTcpSocket constructor.
	 * Also used to determine if incoming HTTP connection is normal (port 80)
	 * or ssl (port 443).
	 */
	/* 该函数用于监听套接字
	 * 在accept之后，增加到handler之前使用
	 *	
	 * */
	virtual void Init();

	/**创建一个套接字文件描述符
		\ af  AF_INET / AF_INET6 /
		\ type SOCK_STREAM / SOCK_DGRAM /
		\ protocol "tcp" / "udp" /
	*/
	SOCKET CreateSocket(int af,int type,const std::string& protocol = "");

	//将一个描述符attch到Socket
	void Attach(SOCKET s);

	//返回文件描述符
	SOCKET GetSocket();

	//关闭连接（内部使用） -SetCloseAndDelete 
	virtual int Close();

	//当一个套接字有效(valid)而且该套接字并没有打算关闭
	virtual bool Ready();

	//返回一个监听套接字实例的指针。因为LiatenSocket<>作为父Socket
	Socket *GetParent();

	//ListenSocket使用。给新创建的Socket设置父亲Socket
	void SetParent(Socket *);

	//返回监听套机字ListenSocket<>的监听端口
	virtual port_t GetPort();

	//设置Socket为非阻塞
	bool SetNonblocking(bool);

	//对一个描述符设置为非阻塞
	bool SetNonblocking(bool, SOCKET);

	//该实例的总剩余事件
	time_t Uptime();

	//设置最后一次connect()的地址和端口
	void SetClientRemoteAddress(SocketAddress&);

	//获取最后一次connect（）的地址结构SocketAddress
	std::auto_ptr<SocketAddress> GetClientRemoteAddress();

	//TCP和UDP：发送一个byte数组
	virtual void SendBuf(const char *,size_t,int = 0);

	//TCP和UDP：发送字符串
	virtual void Send(const std::string&,int = 0);

	//获取发送的字节数
	virtual uint64_t GetBytesSent(bool clear = false);

	//获取收到的字节数
	virtual uint64_t GetBytesReceived(bool clear = false);

	// LIST_TIMEOUT

	/** Enable timeout control. 0=disable timeout check. */

	void SetTimeout(time_t secs);

	bool CheckTimeout();

	/** Check timeout. \return true if time limit reached */
	bool Timeout(time_t tnow);

	/** Used by ListenSocket. ipv4 and ipv6 */
	//ListenSocket<>使用
	void SetRemoteAddress(SocketAddress&);


	//------------------------------------------------------------------
	//事件产生，回调
	//------------------------------------------------------------------

	
	//当有可读事件产生时
	virtual void OnRead();
	//当描述符可以写时
	virtual void OnWrite();
	//当描述符异常时
	virtual void OnException();
	//当该Socket被ISocketHandler删除时回调
	virtual void OnDelete();
	//当一个connection完成时(对于客户端)
	virtual void OnConnect();
	//当一个外来连接请求accept完成时（对于服务器）
	virtual void OnAccept();
	//在行协议模式时，当一个完整的行被读到时
	virtual void OnLine(const std::string& );
	//当connect超时时（5s）（客户端）
	virtual void OnConnectFailed();

	/*当一个客户端Socket创建时，会使用Onptinos设置选项
		\family AF_INET, AF_INET6
		\type SOCK_STREAM, SOCK_DGRAM
		\protocol (tcp, udp, sctp)
		\s 套接字描述符*/
	virtual void OnOptions(int family,int type,int protocol,SOCKET s) = 0;


	//connect重试时回调，返回false为中断连接
	virtual bool OnConnectRetry();
#ifdef ENABLE_RECONNECT
	//一个重新connect被使用
	virtual void OnReconnect();
#endif
	//当一个连接被断开时（recv返回0）
	virtual void OnDisconnect();

	//TCPSocket
	/*当一个连接断开被检测到时（recv返回0）
		\info bit 0 read(0)/write(1)
					bit 1 normal(read or write returned 0)/error(r/w returned -1)
		\code read/write error code */
	virtual void OnDisconnect(short info, int code);
	//超时回调
	virtual void OnTimeout();
	//连接超时回调
	virtual void OnConnectTimeout();




	//当你想socketHandler在使用完该Socket时并删除该Socket
	void SetDeleteByHandler(bool = true);
	//返回true，表示该Socket将会被Socketandler删除
	bool DeleteByHandler();

	// LIST_CLOSE - conditional event queue

	/** Set close and delete to terminate the connection. */

	//设置关闭和删除 达到关闭该连接目的
	void SetCloseAndDelete(bool = true);
	//返回true，表示该Socket将会被关闭和删除
	bool CloseAndDelete();

	//返回到关闭该Socket的时间
	time_t TimeSinceClose();

	//忽略读事件（仅仅对套接字有效）
	void DisableRead(bool x = true);
	//返回true，表明读事件被忽略
	bool IsDisableRead();

	//设置connect状态
	void SetConnected(bool = true);
	//返回true，表明已经connect
	bool IsConnected();

	//tcpSocket
	//当读或写错误发生时设置连接丢失
	void SetLost();
	//返回true，表明读或写错误导致套接字关闭
	bool Lost();

	/** Set flag indicating the socket is being actively deleted by the sockethandler. */
	void SetErasedByHandler(bool x = true);
	/** Get value of flag indicating socket is deleted by sockethandler. */
	bool ErasedByHandler();


	//返回远程主机SocketAddress
	std::auto_ptr<SocketAddress> GetRemoteSocketAddress();
	//返回远程主机IP地址
	ipaddr_t GetRemoteIP4();
	//返回端口
	port_t GetRemotePort();
	//以字符串返回
	std::string GetRemoteAddress();
	//返回远程主机名
	std::string GetRemoteHostname();

	//返回该Socket使用的端口
	port_t GetSockPort();
	//返回该Socket使用的ip地址
	ipaddr_t GetSockIP4();
	std::string GetSockAddress();

	//返回该Socket唯一的id
	socketuid_t UniqueIdentifier() { return m_uid; }

	// --------------------------------------------------------------------------
	// IP选项
	// --------------------------------------------------------------------------

	//设置tos
	bool SetIpTOS(unsigned char tos);
	unsigned char IpTOS();
	//设置ttl
	bool SetIpTTL(int ttl);
	int IpTTL();
	bool SetIpHdrincl(bool x = true);
	bool SetIpMulticastTTL(int);
	int IpMulticastTTL();
	bool SetMulticastLoop(bool x = true);
	bool IpAddMembership(struct ip_mreq&);
	bool IpDropMembership(struct ip_mreq&);

#ifdef IP_PKTINFO
	bool SetIpPktinfo(bool x = true);
#endif
#ifdef IP_RECVTOS
	bool SetIpRecvTOS(bool x = true);
#endif
#ifdef IP_RECVTTL
	bool SetIpRecvTTL(bool x = true);
#endif
#ifdef IP_RECVOPTS
	bool SetIpRecvopts(bool x = true);
#endif
#ifdef IP_RETOPTS
	bool SetIpRetopts(bool x = true);
#endif
#ifdef IP_RECVERR
	bool SetIpRecverr(bool x = true);
#endif
#ifdef IP_MTU_DISCOVER
	bool SetIpMtudiscover(bool x = true);
#endif
#ifdef IP_MTU
	int IpMtu();
#endif
#ifdef IP_ROUTER_ALERT
	bool SetIpRouterAlert(bool x = true);
#endif
#ifdef LINUX
	bool IpAddMembership(struct ip_mreqn&);
#endif
#ifdef LINUX
	bool IpDropMembership(struct ip_mreqn&);
#endif

	//-----------------------------------------------
	//Socket选项
	//-----------------------------------------------

	bool SoAcceptconn();
	bool SetSoBroadcast(bool x = true);
	bool SetSoDebug(bool x = true);
	int SoError();
	bool SetSoDontroute(bool x = true);
	bool SetSoLinger(int onoff, int linger);
	bool SetSoOobinline(bool x = true);
	bool SetSoRcvlowat(int);
	bool SetSoSndlowat(int);
	bool SetSoRcvtimeo(struct timeval&);
	bool SetSoSndtimeo(struct timeval&);
	bool SetSoRcvbuf(int);
	int SoRcvbuf();
	bool SetSoSndbuf(int);
	int SoSndbuf();
	int SoType();
	bool SetSoReuseaddr(bool x = true);
	bool SetSoKeepalive(bool x = true);

#ifdef SO_BSDCOMPAT
	bool SetSoBsdcompat(bool x = true);
#endif
#ifdef SO_BINDTODEVICE
	bool SetSoBindtodevice(const std::string& intf);
#endif
#ifdef SO_PASSCRED
	bool SetSoPasscred(bool x = true);
#endif
#ifdef SO_PEERCRED
	bool SoPeercred(struct ucred& );
#endif
#ifdef SO_PRIORITY
	bool SetSoPriority(int);
#endif
#ifdef SO_RCVBUFFORCE
	bool SetSoRcvbufforce(int);
#endif
#ifdef SO_SNDBUFFORCE
	bool SetSoSndbufforce(int);
#endif
#ifdef SO_TIMESTAMP
	bool SetSoTimestamp(bool x = true);
#endif
#ifdef SO_NOSIGPIPE
	bool SetSoNosigpipe(bool x = true);
#endif

	// -------------------------------------------------------------
	//TCP选项在TcpSocket里
	// -------------------------------------------------------------


	// LIST_CALLONCONNECT

	//设置在下一次SocketHandler循环时调用OnConnect回调
	void SetCallOnConnect(bool x = true);

	//返回true，表示OnConnect()将会被调用
	bool CallOnConnect();



	//设置是否在一个connect超时后，是否继续连接
	void SetRetryClientConnect(bool x = true);

	//返回true，表示将会继续连接
	bool RetryClientConnect();


#ifdef ENABLE_POOL
	//设置是客户端-用于连接-TCPSocket
	void SetIsClient();
	//设置Socket的type。在socket（）里设置
	void SetSocketType(int x);
	int GetSocketType();
	void SetSocketProtocol(const std::string& x);
	const std::string& GetSocketProtocol();

	/** Instruct a client socket to stay open in the connection pool after use.
		If you have connected to a server using tcp, you can call SetRetain
		to leave the connection open after your socket instance has been deleted.
		The next connection you make to the same server will reuse the already
		opened connection, if it is still available.
	*/
	void SetRetain();
	/** Check retain flag.
		\return true if the socket should be moved to connection pool after use */
	bool Retain();
	/** Copy connection parameters from sock. */
	void CopyConnection(Socket *sock);
	//@}
#endif // ENABLE_POOL

#ifdef ENABLE_SOCKS4
	/** \name Socks4 support */
	//@{
	/** Socks4 client support internal use. \sa TcpSocket */
	virtual void OnSocks4Connect();
	/** Socks4 client support internal use. \sa TcpSocket */
	virtual void OnSocks4ConnectFailed();
	/** Socks4 client support internal use. \sa TcpSocket */
	virtual bool OnSocks4Read();
	/** Called when the last write caused the tcp output buffer to
	 * become empty. */
	/** socket still in socks4 negotiation mode */
	bool Socks4();
	/** Set flag indicating Socks4 handshaking in progress */
	void SetSocks4(bool x = true);

	/** Set socks4 server host address to use */
	void SetSocks4Host(ipaddr_t a);
	/** Set socks4 server hostname to use. */
	void SetSocks4Host(const std::string& );
	/** Socks4 server port to use. */
	void SetSocks4Port(port_t p);
	/** Provide a socks4 userid if required by the socks4 server. */
	void SetSocks4Userid(const std::string& x);
	/** Get the ip address of socks4 server to use.
		\return socks4 server host address */
	ipaddr_t GetSocks4Host();
	/** Get the socks4 server port to use.
		\return socks4 server port */
	port_t GetSocks4Port();
	/** Get socks4 userid.
		\return Socks4 userid */
	const std::string& GetSocks4Userid();
	//@}
#endif // ENABLE_SOCKS4

#ifdef ENABLE_RESOLVER
	/** \name Asynchronous Resolver */
	//@{
	/** Request an asynchronous dns resolution.
		\param host hostname to be resolved
		\param port port number passed along for the ride
		\return Resolve ID */
	int Resolve(const std::string& host,port_t port = 0);
#ifdef ENABLE_IPV6
	int Resolve6(const std::string& host, port_t port = 0);
#endif
	/** Callback returning a resolved address.
		\param id Resolve ID from Resolve call
		\param a resolved ip address
		\param port port number passed to Resolve */
	virtual void OnResolved(int id,ipaddr_t a,port_t port);
#ifdef ENABLE_IPV6
	virtual void OnResolved(int id,in6_addr& a,port_t port);
#endif
	/** Request asynchronous reverse dns lookup.
		\param a in_addr to be translated */
	int Resolve(ipaddr_t a);
#ifdef ENABLE_IPV6
	int Resolve(in6_addr& a);
#endif
	/** Callback returning reverse resolve results.
		\param id Resolve ID
		\param name Resolved hostname */
	virtual void OnReverseResolved(int id,const std::string& name);
	/** Callback indicating failed dns lookup.
		\param id Resolve ID */
	virtual void OnResolveFailed(int id);
	//@}
#endif  // ENABLE_RESOLVER

#ifdef ENABLE_DETACH
	/** \name Thread Support */
	//@{
	/** Callback fires when a new socket thread has started and this
		socket is ready for operation again.
		\sa ResolvSocket */
	virtual void OnDetached();

	// LIST_DETACH

	/** Internal use. */
	void SetDetach(bool x = true);
	/** Check detach flag.
		\return true if the socket should detach to its own thread */
	bool IsDetach();

	/** Internal use. */
	void SetDetached(bool x = true);
	/** Check detached flag.
		\return true if the socket runs in its own thread. */
	const bool IsDetached() const;
	/** Order this socket to start its own thread and call OnDetached
		when ready for operation. */
	bool Detach();
	/** Store the slave sockethandler pointer. */
	void SetSlaveHandler(ISocketHandler *);
	/** Create new thread for this socket to run detached in. */
	void DetachSocket();
	//@}
#endif // ENABLE_DETACH

	/** Write traffic to an IFile. Socket will not delete this object. */
	void SetTrafficMonitor(IFile *p) { m_traffic_monitor = p; }

protected:
	/** default constructor not available */
	Socket() : m_handler(m_handler) {}
	/** copy constructor not available */
	Socket(const Socket& s) : m_handler(s.m_handler) {}

	/** assignment operator not available. */
	Socket& operator=(const Socket& ) { return *this; }

	/** All traffic will be written to this IFile, if set. */
	IFile *GetTrafficMonitor() { return m_traffic_monitor; }

//	unsigned long m_flags; ///< boolean flags, replacing old 'bool' members

private:
	ISocketHandler& m_handler; ///< Reference of ISocketHandler in control of this socket
	SOCKET m_socket; ///< File descriptor
	bool m_bDel; ///< Delete by handler flag
	bool m_bClose; ///< Close and delete flag
	time_t m_tCreate; ///< Time in seconds when this socket was created
	Socket *m_parent; ///< Pointer to ListenSocket class, valid for incoming sockets
	bool m_b_disable_read; ///< Disable checking for read events
	bool m_connected; ///< Socket is connected (tcp/udp)
	bool m_b_erased_by_handler; ///< Set by handler before delete
	time_t m_tClose; ///< Time in seconds when ordered to close
	std::auto_ptr<SocketAddress> m_client_remote_address; ///< Address of last connect()
	std::auto_ptr<SocketAddress> m_remote_address; ///< Remote end address
	IFile *m_traffic_monitor;
	time_t m_timeout_start; ///< Set by SetTimeout
	time_t m_timeout_limit; ///< Defined by SetTimeout
	bool m_bLost; ///< connection lost
static	socketuid_t m_next_uid;
	socketuid_t m_uid;
	bool m_call_on_connect; ///< OnConnect will be called next ISocketHandler cycle if true
	bool m_b_retry_connect; ///< Try another connection attempt next ISocketHandler cycle

#ifdef _WIN32
static	WSAInitializer m_winsock_init; ///< Winsock initialization singleton class
#endif

#ifdef HAVE_OPENSSL
	bool m_b_enable_ssl; ///< Enable SSL for this TcpSocket
	bool m_b_ssl; ///< ssl negotiation mode (TcpSocket)
	bool m_b_ssl_server; ///< True if this is an incoming ssl TcpSocket connection
#endif

#ifdef ENABLE_IPV6
	bool m_ipv6; ///< This is an ipv6 socket if this one is true
#endif

#ifdef ENABLE_POOL
	int m_socket_type; ///< Type of socket, from socket() call
	std::string m_socket_protocol; ///< Protocol, from socket() call
	bool m_bClient; ///< only client connections are pooled
	bool m_bRetain; ///< keep connection on close
#endif

#ifdef ENABLE_SOCKS4
	bool m_bSocks4; ///< socks4 negotiation mode (TcpSocket)
	ipaddr_t m_socks4_host; ///< socks4 server address
	port_t m_socks4_port; ///< socks4 server port number
	std::string m_socks4_userid; ///< socks4 server usedid
#endif

#ifdef ENABLE_DETACH
	bool m_detach; ///< Socket ordered to detach flag
	bool m_detached; ///< Socket has been detached
	SocketThread *m_pThread; ///< Detach socket thread class pointer
	ISocketHandler *m_slave_handler; ///< Actual sockethandler while detached
#endif
};

#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _SOCKETS_Socket_H

