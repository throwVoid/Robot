#include "RobotConfig.h"
using namespace std;
#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif
RobotConfig::RobotConfig(string fileName, string spliter, string comments )
: spliterString(spliter)
,mComment(comments)
{
	//定义输入流来提取配置文件
	std::ifstream readStream(fileName.c_str());
	//如果文件不存在
	if(!readStream ) 
		throw FileNotFound(fileName); 
	//读取配置文件
	readStream >> (*this);
}


RobotConfig::~RobotConfig()
{
	//清理map中的键与值对
	keyAndValueMap.clear();
}


//默认构造函数
//默认键与值分隔符为=
//默认注释值分隔符号为#
RobotConfig::RobotConfig()
:spliterString("=")
,mComment("#")
{
}


//查找是否已经存在一个键值
bool RobotConfig::KeyIsExisted(const string& key) const
{
	//定义const迭代器在map里查找关键字
	mapConstIter iter = keyAndValueMap.find(key);
	return (iter != keyAndValueMap.end());
}


void RobotConfig::TrimWhiteSpace( string& s )
{
	//消除前置和后置的空字符
	static const char whiteSpaceCharter[] = " \n\t\v\r\f";
	s.erase( 0, s.find_first_not_of(whiteSpaceCharter) );
	s.erase(s.find_last_not_of(whiteSpaceCharter) + 1U );
}


//重载的<<,用于输出获得的配置信息
std::ostream& operator<<( std::ostream& os, const RobotConfig& config )
{
	for( RobotConfig::mapConstIter iter = config.keyAndValueMap.begin(); iter != config.keyAndValueMap.end(); ++iter)
	{
		//输出键
		os << iter->first << " " << config.spliterString << " ";

		//输出值
		os << iter->second << std::endl;
	}
	return os;
}



//从映射里删除键
void RobotConfig::RemoveByKey( const string& key )
{
	keyAndValueMap.erase(keyAndValueMap.find(key));
}

//从配置文件中读取键值对
std::istream& operator>>( std::istream& is, RobotConfig& config )
{
	//键值分隔符
	const string& splitStr  = config.spliterString;
	//注释分隔符
	const string& comm   = config.mComment;
	//分隔符长度，可以使用字符串作为分隔符
	const string::size_type skip = splitStr.length();

	//用于读取文件的下一行
	string nextline = "";

	while( is || nextline.length() > 0 )
	{
		string line;
		//第一次读时判断
		if( nextline.length() > 0 )
		{
			line = nextline;
			nextline = "";
		}
		else
		{
			//读一行到line中
			std::getline( is, line );
		}
		//忽略注释
		line = line.substr( 0, line.find(comm) );
		//找到键与值的分隔符位置
		string::size_type splitStrPos = line.find( splitStr );
		//找到分隔符
		if( splitStrPos < string::npos )
		{
			//取得键值
			string key = line.substr( 0, splitStrPos );
			//将原字符串line的键值与分隔符号字符串删除，保留下值
			line.replace( 0, splitStrPos + skip, "" );
			bool nextLineHaveValue = false;
			//以下用来解决把键与值写在多行的情况
			while( !nextLineHaveValue && is )
			{
				std::getline( is, nextline );
				nextLineHaveValue = true;

				string nextLineCopy = nextline;
				RobotConfig::TrimWhiteSpace(nextLineCopy);
				if( nextLineCopy == "" ) 
					continue;
				//去除注释
				nextline = nextline.substr( 0, nextline.find(comm) );
				//找分隔符
				if( nextline.find(splitStr) != string::npos )
					continue;

				nextLineCopy = nextline;
				RobotConfig::TrimWhiteSpace(nextLineCopy);
				if( nextLineCopy != "" ) 
					line += "\n";
					//在新行里找到值
				line += nextline;
				nextLineHaveValue = false;
			}

			//存储到map中
			RobotConfig::TrimWhiteSpace(key);
			RobotConfig::TrimWhiteSpace(line);
			config.keyAndValueMap[key] = line;
		}
	}

	return is;
}
bool RobotConfig::FileIsExisted(string filename)
{
	bool exist = false;
	std::ifstream in( filename.c_str() );
	if( in ) 
		exist = true;
	return exist;
}

void RobotConfig::ReadRobotConfigFile( string filename, string spliter, string comments )
{
	spliterString = spliter;
	mComment = comments;
	std::ifstream in( filename.c_str() );
	if( !in ) 
		throw FileNotFound( filename ); 
	in >> (*this);
}

#ifdef ROBOT_NAMESPACE
}
#endif
