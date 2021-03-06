//main.cpp
#include "RobotConfig.h"
int main()
{
	int port;
	std::string ipAddress;
	std::string username;
	std::string password;
	const char ConfigFile[]= "config.txt"; 
	RobotConfig configSettings(ConfigFile);
	
	port = configSettings.Read("port", 0);
	ipAddress = configSettings.Read("ipAddress", ipAddress);
	username = configSettings.Read("username", username);
	password = configSettings.Read("password", password);
	std::cout<<"port:"<<port<<std::endl;
	std::cout<<"ipAddress:"<<ipAddress<<std::endl;
	std::cout<<"username:"<<username<<std::endl;
	std::cout<<"password:"<<password<<std::endl;
	configSettings.Remove("port");
	
	return 0;
}

