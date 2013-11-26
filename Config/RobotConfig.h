#ifndef ROBOT_MAIN_CONFIG_H_
#define ROBOT_MAIN_CONFIG_H_

#pragma once

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;


#ifdef ROBOT_NAMESPACE
namespace ROBOT_NAMESPACE {
#endif


class RobotConfig 
{
protected:
	//键和值分割符，默认为"="
	string spliterString;
	//值和注释分割符，默认为"#"
	string mComment; 
	//键和值映射
	std::map<string,string> keyAndValueMap;
	//迭代器
	typedef std::map<string,string>::const_iterator mapConstIter;
public:
	virtual ~RobotConfig();
	RobotConfig();
	//构造函数，文件名+分隔符+注释符
	RobotConfig(string filename,string = "=",string = "#" );

	//读取key对应的键
	template<typename T> T Read( const string& ) const;  
	//读取key对应的键,如果没有则第二个参数为返回值,返回值为读取成功与否
	template<typename T> T Read( const string& , const T&) const;
	//读取第二个参数key的值到第一个参数，返回值为读取成功与否
	template<typename T> bool ReadInto( T& value, const string& key ) const;
	//在上边函数的基础上增加了默认传回值，返回值为读取成功与否
	template<typename T> bool ReadInto( T& value, const string& key, const T& defaultValue ) const;
	//文件存在性检测
	bool FileIsExisted(string filename);
	//读文件
	void ReadRobotConfigFile(string filename, string = "=",string = "#" );
	// 测试是否已经存在这样的键
	bool KeyIsExisted( const string& key ) const;

	//增加键值对
	template<typename T> void Add( const string& key, const T& value );
	//删除键
	void RemoveByKey( const string& key);

	
	string GetSpliterString() const { return spliterString; }
	string GetComments() const { return mComment; }
	string SetDelimiter( const string& inStream )
	{ string old = spliterString;  spliterString = inStream;  return old; }  
	string SetComment( const string& inStream )
	{ string old = mComment;  mComment =  inStream;  return old; }

	friend std::ostream& operator<<( std::ostream& os, const RobotConfig&);
	friend std::istream& operator>>( std::istream& is, RobotConfig& );

protected:
	template<typename T> static string TAsString( const T& t );
	template<typename T> static T stringAsT( const string& s );
	static void TrimWhiteSpace( string&);
public:
	//文件不存在时的异常抛出类
	struct FileNotFound 
	{
		string filename;
		FileNotFound( const string tFilename = " File not found" ) : filename(tFilename) 
		{
		} 
	};
	//键不存在时的异常抛出类
	struct KeyNotFound 
	{ 
		string key;
		KeyNotFound( const string tKey = "Key not found") : key(tKey)
			{
			} 
	};
};


template<typename T>
string RobotConfig::TAsString( const T& t )
{
	//将T类型对象转换成字符串
	//前提是T支持<<操作符
	std::ostringstream ost;
	ost << t;
	return ost.str();
}


template<typename T>
T RobotConfig::stringAsT( const string& s )
{
	//将string转换成T类型
	//前提是T支持operator>>操作符
	T t;
	std::istringstream ist(s);
	ist >> t;
	return t;
}


template<>
inline string RobotConfig::stringAsT<string>( const string& s )
{
	//全特化的转换函数，字符串与字符串转换什么也不做
	return s;
}


template<>
inline bool RobotConfig::stringAsT<bool>( const string& s )
{
	// "false", "F", "no", "n", "0" 为false
	// "true", "T", "yes", "y", "1", "-1"为true
	// 转换一个字符串到bool型
	bool b = true;
	string sup = s;
	for( string::iterator iter = sup.begin(); iter != sup.end(); ++iter )
		*iter = toupper(*iter);
	if( sup==string("FALSE") || sup==string("F") ||
		sup==string("NO") || sup==string("N") ||
		sup==string("0") || sup==string("NONE") )
		b = false;
	return b;
}


template<typename T>
T RobotConfig::Read( const string& key ) const
{
	mapConstIter p = keyAndValueMap.find(key);
	if( p == keyAndValueMap.end() ) throw KeyNotFound(key);
	return stringAsT<T>( p->second );
}


template<typename T>
T RobotConfig::Read( const string& key, const T& value ) const
{
	//如果不存在，返回默认值，而不是丢出一个异常
	mapConstIter p = keyAndValueMap.find(key);
	if( p == keyAndValueMap.end() ) return value;
	return stringAsT<T>( p->second );
}


template<typename T>
bool RobotConfig::ReadInto( T& var, const string& key ) const
{
	//读取值，如果返回true则var为值，否则返回false
	mapConstIter p = keyAndValueMap.find(key);
	bool found = ( p != keyAndValueMap.end() );
	if( found ) var = stringAsT<T>( p->second );
	return found;
}


template<typename T>
bool RobotConfig::ReadInto( T& var, const string& key, const T& value ) const
{
	//
	//读取值，如果返回true则var为值，否则返回false,并返回默认value的var
	mapConstIter p = keyAndValueMap.find(key);
	bool found = ( p != keyAndValueMap.end() );
	if( found )
		var = stringAsT<T>( p->second );
	else
		var = value;
	return found;
}

//在映射中增加一个键值对
template<typename T>
void RobotConfig::Add( const string& in_key, const T& value )
{
	string v = TAsString( value );
	string key=in_key;
	TrimWhiteSpace(key);
	TrimWhiteSpace(v);
	keyAndValueMap[key] = v;
	return;
}
#ifdef ROBOT_NAMESPACE
}
#endif
#endif//ROBOT_MAIN_CONFIG_H_
