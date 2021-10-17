/*
	   _ _____ _   _  _____ _  _______ _   _  _____
	  | |_   _| \ | |/ ____| |/ /_   _| \ | |/ ____|
	  | | | | |  \| | |  __| ' /  | | |  \| | |  __
  _   | | | | | . ` | | |_ |  <   | | | . ` | | |_ |
 | |__| |_| |_| |\  | |__| | . \ _| |_| |\  | |__| |
  \____/|_____|_| \_|\_____|_|\_\_____|_| \_|\_____|

 Author: Jing
*/
#ifndef _MYSQL_CONNECTION_POOL_SIMPLE
#define _MYSQL_CONNECTION_POOL_SIMPLE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include "sys_def.h"

/**
* @brief MySQL Native C API 的 C++ 封装
*/
class mysql_conn_plus
{
private:
	unsigned long long counter;

	UINT16 mysql_db_port;

	MYSQL conn;

	char mysql_db_host[32];

	char mysql_db_user[32];

	char mysql_db_pass[32];

	char mysql_db_name[32];

public:
	mysql_conn_plus();

	mysql_conn_plus(const char *db_host, const char *db_user, const char *db_pass, const char *db_name, UINT16 db_port);

	~mysql_conn_plus();

	void set_paras(const char* db_host, const char* db_user, const char* db_pass, const char* db_name, UINT16 db_port);

	BOOL connect();

	void close();

	BOOL reconnect();

	signed long long query(const char* sqlstr);

	signed long long safe_query(const char* sqlstr, UINT64 cnt, UINT64 max_cnt);

	MYSQL* get_native_conn();

	unsigned int get_error_code();

	const char* get_error_description();

	unsigned long long get_counter();

	void set_counter(unsigned long long value);
};

/**
* @brief MySQL读取查询记录的result对象
*/
class mysql_result_plus
{
private:
	MYSQL_RES* result_ptr;

	MYSQL_ROW result_row;

	MYSQL_FIELD* fields_ptr;

	unsigned int fields_count;

	char fields_names[64][64];

public:
	mysql_result_plus(MYSQL* lp_conn);

	mysql_result_plus(mysql_conn_plus* lp_conn);

	~mysql_result_plus();

	BOOL available();

	unsigned int get_fields_count();

	int get_field_index(const char* field_name);

	char* get_row_field_value(const char* field_name);

	MYSQL_ROW *get_native_row();

	BOOL read_row();

	const char* operator[] (const unsigned int index);

	const char* operator[] (const char* field_name);
};

/**
* @brief mysql_conn_plus 构造函数
*/
inline mysql_conn_plus::mysql_conn_plus()
{
	memset(mysql_db_host, 0, 32);
	memset(mysql_db_user, 0, 32);
	memset(mysql_db_pass, 0, 32);
	memset(mysql_db_name, 0, 32);
	mysql_db_port = 0;

	counter = 0;

	mysql_init(&conn);
}

/**
* @param db_host 数据库服务器地址
* @param db_user 数据库服务的登录账号
* @param db_pass 数据库服务的登录密码
* @param db_name 数据库服务的登录后默认选择的数据库名称
* @brief mysql_conn_plus 构造函数
*/
inline mysql_conn_plus::mysql_conn_plus(const char* db_host, const char* db_user, const char* db_pass, const char* db_name, UINT16 db_port)
{
	memset(mysql_db_host, 0, 32);
	memset(mysql_db_user, 0, 32);
	memset(mysql_db_pass, 0, 32);
	memset(mysql_db_name, 0, 32);
	mysql_db_port = db_port;
	counter = 0;

	strncpy(mysql_db_host, db_host, 30);
	strncpy(mysql_db_user, db_user, 30);
	strncpy(mysql_db_pass, db_pass, 30);
	strncpy(mysql_db_name, db_name, 30);

	mysql_init(&conn);
}

inline mysql_conn_plus::~mysql_conn_plus()
{
	mysql_close(&conn);
}

/**
* @param db_host 数据库服务器地址
* @param db_user 数据库服务的登录账号
* @param db_pass 数据库服务的登录密码
* @param db_name 数据库服务的登录后默认选择的数据库名称
* @brief 设置数据库连接参数
*/
inline void mysql_conn_plus::set_paras(const char* db_host, const char* db_user, const char* db_pass, const char* db_name, UINT16 db_port)
{
	memset(mysql_db_host, 0, 32);
	memset(mysql_db_user, 0, 32);
	memset(mysql_db_pass, 0, 32);
	memset(mysql_db_name, 0, 32);
	mysql_db_port = db_port;
	counter = 0;

	strncpy(mysql_db_host, db_host, 30);
	strncpy(mysql_db_user, db_user, 30);
	strncpy(mysql_db_pass, db_pass, 30);
	strncpy(mysql_db_name, db_name, 30);
}

/**
* @brief 连接数据库
* @return 是否连接成功
*/
inline BOOL mysql_conn_plus::connect()
{
	if (!mysql_real_connect(&conn, mysql_db_host, mysql_db_user, mysql_db_pass, mysql_db_name, mysql_db_port, NULL, CLIENT_FOUND_ROWS))
	{
		printf("Can not connect to mysql : %s.\n", (mysql_error(&conn)));
		return FALSE;
	}
	else
	{
		printf("MySQL database connected.\n");
		return TRUE;
	}
}

/**
* @brief 与数据库断开连接
*/
inline void mysql_conn_plus::close()
{
	mysql_close(&conn);
}

/**
* @brief 与数据库重连
* @return 是否连接成功
*/
inline BOOL mysql_conn_plus::reconnect()
{
	printf("MySQL database reconnecting...\n");
	mysql_close(&conn);
	return connect();
}

/**
* @param sqlstr 要执行的SQL语句的字符串
* @brief 执行SQL语句
* @return 影响的行数
*/
inline signed long long mysql_conn_plus::query(const char* sqlstr)
{
	int res = mysql_query(&conn, sqlstr);
	if (res)
	{
		reconnect();
		res = mysql_query(&conn, sqlstr);
		if (res)
			return -1;
	}
	return mysql_affected_rows(&conn);
}

/**
* @param sqlstr 要执行的SQL语句的字符串
* @param cnt 当前计数器的值
* @param max_cnt 与上次计数器的值最大允许的差值，超过则先重置连接确保连接质量
* @brief 执行SQL语句
* @return 影响的行数
*/
inline signed long long mysql_conn_plus::safe_query(const char* sqlstr, UINT64 cnt, UINT64 max_cnt)
{
	if (cnt - counter >= max_cnt)
		reconnect();
	counter = cnt;

	return query(sqlstr);
}

/**
* @brief 获取数据库原始连接对象的指针(MYSQL *)
* @return 数据库原始连接对象的指针(MYSQL *)
*/
inline MYSQL* mysql_conn_plus::get_native_conn()
{
	return &conn;
}

/**
* @brief 获取数据库操作错误的代码，为0则表示执行正常
* @return 数据库操作错误的代码
*/
inline unsigned int mysql_conn_plus::get_error_code()
{
	return mysql_errno(&conn);
}

/**
* @brief 获取数据库操作错误的描述字符串
* @return 数据库操作错误的描述字符串
*/
inline const char* mysql_conn_plus::get_error_description()
{
	return mysql_error(&conn);
}

/**
* @brief 获取上次设置的计数器的值
* @return 上次设置的计数器的值
*/
inline unsigned long long mysql_conn_plus::get_counter()
{
	return counter;
}

/**
* @brief 设置的计数器的值
* @param value 要填入计数器的当前值（一般是当前的秒数）
*/
inline void mysql_conn_plus::set_counter(unsigned long long value)
{
	counter = value;
}

/**
* @param lp_conn MYSQL 原始连接的指针
* @brief MySQL读取记录的result的构造函数
*/
inline mysql_result_plus::mysql_result_plus(MYSQL* lp_conn)
{
	memset(fields_names, 0, sizeof(fields_names));
	result_ptr = mysql_use_result(lp_conn);

	if (!result_ptr)
		return;

	fields_count = mysql_num_fields(result_ptr);
	fields_ptr = mysql_fetch_fields(result_ptr);

	for (unsigned int i = 0; i < fields_count; i++)
		strncpy(fields_names[i], fields_ptr[i].name, 60);

	for (unsigned int i = 0; i < fields_count; i++)
		printf("%s ", fields_names[i]);
	printf("\n");
}

/**
* @param lp_conn 数据库连接 mysql_conn_plus 的指针
* @brief MySQL读取记录的result的构造函数
*/
inline mysql_result_plus::mysql_result_plus(mysql_conn_plus* lp_conn)
{
	memset(fields_names, 0, sizeof(fields_names));
	result_ptr = mysql_use_result(lp_conn->get_native_conn());

	if (!result_ptr)
		return;

	fields_count = mysql_num_fields(result_ptr);
	fields_ptr = mysql_fetch_fields(result_ptr);

	for (unsigned int i = 0; i < fields_count; i++)
		strncpy(fields_names[i], fields_ptr[i].name, 60);

	for (unsigned int i = 0; i < fields_count; i++)
		printf("%s ", fields_names[i]);
	printf("\n");
}

inline mysql_result_plus::~mysql_result_plus()
{
	memset(fields_names, 0, sizeof(fields_names));
	if (result_ptr)
		mysql_free_result(result_ptr);
}

/**
* @brief 返回当前记录集是否可用
* @return 当前记录集是否可用
*/
inline BOOL mysql_result_plus::available()
{
	if (result_ptr)
		return TRUE;
	else
		return FALSE;
}

/**
* @brief 返回当前列的数量
* @return 列的数量
*/
inline unsigned int mysql_result_plus::get_fields_count()
{
	return fields_count;
}

/**
* @param field_name 字段名称
* @brief 获取该字段名称对应的字段序号
* @return 字段序号
*/
inline int mysql_result_plus::get_field_index(const char* field_name)
{
	if (!field_name)
		return -1;

	for (int i = 0; i < fields_count; i++)
	{
		if (!strcmp(fields_names[i], field_name))
			return i;
	}

	return -1;
}

/**
* @param field_name 字段名称
* @brief 获取当前行下该字段名称对应的值（字符串）
* @return 该字段名称对应的值（字符串）
*/
inline char* mysql_result_plus::get_row_field_value(const char* field_name)
{
	static char blank_value[32] = { 0 };
	if (!field_name)
		return blank_value;

	int index = get_field_index(field_name);
	if(index == -1)
		return blank_value;

	return result_row[index];
}

/**
* @param index 字段序号
* @brief 获取当前行下该字段序号对应的值（字符串）
* @return 该字段序号对应的值（字符串）
*/
inline const char* mysql_result_plus::operator[] (const unsigned int index)
{
	static char blank_value[32] = { 0 };
	if(index >= fields_count)
		return blank_value;
	else
		return result_row[index];
}

/**
* @param field_name 字段名称
* @brief 获取当前行下该字段名称对应的值（字符串）
* @return 该字段名称对应的值（字符串）
*/
inline const char* mysql_result_plus::operator[] (const char* field_name)
{
	static char blank_value[32] = { 0 };
	if (!field_name)
		return blank_value;

	int index = get_field_index(field_name);
	if (index == -1)
		return blank_value;

	return result_row[index];
}

/**
* @brief 获取当前行对应的原始 MYSQL_ROW 的指针
* @return 当前行对应的原始 MYSQL_ROW 的指针
*/
inline MYSQL_ROW* mysql_result_plus::get_native_row()
{
	return &result_row;
}

/**
* @brief 读取新的一行
* @return 读取新的一行是否成功，如果为否则代表该查询的结果已经读完
*/
inline BOOL mysql_result_plus::read_row()
{
	if (result_row = mysql_fetch_row(result_ptr))
		return TRUE;
	else
		return FALSE;
}

#endif // !_MYSQL_CONNECTION_POOL_SIMPLE
