#include "RobotUtility.h"

#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif



std::string RobotUtility::l2string(long l)
{
	std::string str;
	char tmp[100];
	snprintf(tmp,sizeof(tmp),"%ld",l);
	str = tmp;
	return str;
}

std::string RobotUtility::int642string(int64_t l)
{
	std::string str;
	int64_t tmp = l;
	if (l < 0)
	{
		str = "-";
		tmp = -l;
	}
	while (tmp)
	{
		uint64_t a = tmp % 10;
		str = (char)(a + 48) + str;
		tmp /= 10;
	}
	if (!str.size())
	{
		str = "0";
	}
	return str;
}

std::string RobotUtility::int642string(uint64_t l)
{
	std::string str;
	uint64_t tmp = l;
	while (tmp)
	{
		uint64_t a = tmp % 10;
		str = (char)(a + 48) + str;
		tmp /= 10;
	}
	if (!str.size())
	{
		str = "0";
	}
	return str;
}




std::string RobotUtility::ToString(double d)
{
	char tmp[100];
	snprintf(tmp, sizeof(tmp), "%f", d);
	return tmp;
}




const std::string RobotUtility::Stack()
{
#define BFSIZE 255
	void *buffer[BFSIZE];
	int n = backtrace(buffer, BFSIZE);
	char **res = backtrace_symbols(buffer, n);
	std::string tmp;
	for (int i = 0; i < n; ++i)
	{
		std::string x = res[i];
		std::string plus;
		std::string addr;
		size_t pos = x.find("(");
		if (pos != std::string::npos)
		{
			x = x.substr(pos + 1); 

			pos = x.find(")");
			if (pos != std::string::npos)
			{
				addr = x.substr(pos + 1);
				x = x.substr(0, pos);
			}

			pos = x.find("+");
			if (pos != std::string::npos)
			{
				plus = x.substr(pos);
				x = x.substr(0, pos);
			}
		}
		char zz[1000];
		{
			size_t sz = 1000;
			int status = 0;
			abi::__cxa_demangle( x.c_str(), zz, &sz, &status);

			if (!status)
			{
				tmp += zz;
				tmp += plus;
				tmp += addr;
			}
			else
			{
				tmp += res[i];
			}
			tmp += "\n";
		}
		if (0)
		{
			Dl_info info;
			int n = dladdr(buffer[i], &info);
			if (!n)
				printf("%d: dladdr() failed\n", i);
			else
			{
				size_t sz = 1000;
				int status = 0;
				abi::__cxa_demangle( info.dli_sname, zz, &sz, &status);

				printf("%d: %s: %s\n", i, info.dli_fname, info.dli_sname);
				if (!status)
					printf("		%s\n", zz);
			}
		}
	} // for (i)
	free(res);
	return tmp;
}

#ifdef ROBOT_NAMESPACE
}
#endif

