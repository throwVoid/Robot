#include <sys/types.h>
#ifndef _WIN32
#include <sys/socket.h>
#include <netdb.h>
#endif
#include <string.h>
#include <string>
#include <Sockets/Utility.h>
#include <Sockets/Ipv4Address.h>
#include <Sockets/Ipv6Address.h>
#include <assert.h>
#include <Sockets/SocketHandler.h>
#include <Sockets/StdoutLog.h>


int main(int argc, char *argv[])
{
	Sockets::StdoutLog log;
	Sockets::SocketHandler h(&log);
/*
	h.EnableResolver(9999);

//	printf("Waiting for resolver ...");
	while (!h.ResolverReady())
		;
//	printf(" resolver ready!\n");
*/
	std::string hostname = argc < 2 ? "ipv6.google.com" : argv[1];

	{
		printf("Using hostname              : %s\n", hostname.c_str());
		printf("------------------------------------------- normal (old) Utility::u2ip\n");
		ipaddr_t oa;
		if (!Sockets::Utility::u2ip(hostname, oa))
			printf("Ipv4 lookup failed\n");
#ifdef ENABLE_IPV6
		in6_addr oa6;
		if (!Sockets::Utility::u2ip(hostname, oa6))
			printf("Ipv6 lookup failed\n");
#endif
		std::string oname;
		Sockets::Utility::l2ip(oa, oname);
		printf("Ipv4                        : %s (old)\n", oname.c_str());

#ifdef ENABLE_IPV6
		std::string oname6;
		Sockets::Utility::l2ip(oa6, oname6);
		printf("Ipv6                        : %s (old)\n", oname6.c_str());
#endif

		printf("------------------------------------------- new Utility::u2ip, Utility::reverse\n");
		struct sockaddr_in sa;
		if (!Sockets::Utility::u2ip(hostname, sa))
			printf("Ipv4 lookup failed\n");

		ipaddr_t a;
		memcpy(&a, &sa.sin_addr, sizeof(a));
		std::string l2ipname;
		Sockets::Utility::l2ip(a, l2ipname);
		printf("Ipv4                        : %s\n", l2ipname.c_str());

		std::string numeric;
		Sockets::Utility::reverse((struct sockaddr *)&sa, sizeof(sa), numeric, NI_NUMERICHOST);
		printf("Ipv4 numeric                : %s\n", numeric.c_str());

		std::string rname;
		if (!Sockets::Utility::reverse( (struct sockaddr *)&sa, sizeof(sa), rname))
			printf("Reverse Ipv4 failed\n");
		else
		printf("Ipv4 Utility::reverse       : %s\n", rname.c_str());

#ifdef ENABLE_IPV6
		printf("------------------------------------------- new Utility::u2ip, Utility::reverse (Ipv6)\n");
		struct sockaddr_in6 sa6;
		if (!Sockets::Utility::u2ip(hostname, sa6))
			printf("Ipv6 lookup failed\n");

		std::string l2ipname6;
		Sockets::Utility::l2ip(sa6.sin6_addr, l2ipname6);
		printf("Ipv6                        : %s\n", l2ipname6.c_str());

		std::string numeric6;
		Sockets::Utility::reverse((struct sockaddr *)&sa6, sizeof(sa6), numeric6, NI_NUMERICHOST);
		printf("Ipv6 numeric                : %s\n", numeric6.c_str());

		std::string rname6;
		if (!Sockets::Utility::reverse( (struct sockaddr *)&sa6, sizeof(sa6), rname6))
			printf("Reverse Ipv6 failed\n");
		else
		printf("Ipv6 Utility::reverse       : %s\n", rname6.c_str());
#endif

		printf("-------------------------------------------\n");
		in_addr ia;
		/** Resolve hostname. */
//static	bool Resolve(const std::string& hostname,struct in_addr& a);
		/** Reverse resolve (IP to hostname). */
//static	bool Reverse(struct in_addr& a,std::string& name);
		/** Convert address struct to text. */
//static	std::string Convert(struct in_addr& a);
		std::string name;
		if (!Sockets::Ipv4Address::Resolve(hostname, ia))
			printf("Ipv4 lookup failed (Ipv4Address)\n");
		memcpy(&a, &ia, sizeof(a));
		Sockets::Utility::l2ip(a, name);
		printf("Ipv4                        : %s (Ipv4Address)\n", name.c_str());
		assert(name == l2ipname);
		if (!Sockets::Ipv4Address::Reverse(ia, name))
			printf("Reverse Ipv4 lookup failed (Ipv4Address)\n");
		else
		printf("Reverse Ipv4                : %s\n", name.c_str());
		assert(name == rname);
		assert(Sockets::Ipv4Address::Convert(ia) == l2ipname);

#ifdef ENABLE_IPV6
		printf("-------------------------------------------\n");
		/** Resolve hostname. */
//static	bool Resolve(const std::string& hostname,struct in6_addr& a);
		/** Reverse resolve (IP to hostname). */
//static	bool Reverse(struct in6_addr& a,std::string& name);
		/** Convert address struct to text. */
//static	std::string Convert(struct in6_addr& a,bool mixed = false);
		if (!Sockets::Ipv6Address::Resolve(hostname, oa6))
			printf("Ipv6 lookup failed (Ipv4Address)\n");
		Sockets::Utility::l2ip(oa6, name);
		assert(name == l2ipname6);
		printf("Ipv6                        : %s (Ipv6Address)\n", name.c_str());
		if (!Sockets::Ipv6Address::Reverse(oa6, name))
			printf("Reverse Ipv6 lookup failed (Ipv4Address)\n");
		else
		printf("Reverse Ipv6                : %s\n", name.c_str());
		assert(name == rname6);
		std::string mixed_false = Sockets::Ipv6Address::Convert(oa6, false);
		std::string mixed_true = Sockets::Ipv6Address::Convert(oa6, true);
		printf("Ipv6Address::Convert(false) : %s\n", mixed_false.c_str());
		printf("Ipv6Address::Convert(true ) : %s\n", mixed_true.c_str());
		assert(mixed_false == l2ipname6);
#endif
/*
		printf("-------------------------------------------\n");
		int protocol;
		Sockets::Utility::u2service("tcp", protocol);
		printf("tcp: %d\n", protocol);
		Sockets::Utility::u2service("udp", protocol);
		printf("udp: %d\n", protocol);
		Sockets::Utility::u2service("echo", protocol);
		printf("echo: %d\n", protocol);
*/
	}
	printf("\n");
	printf("OK\n");

//	sleep(100);
}


