// config.h: interface for the config class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string>
#include <vector>
#include <map>
#include "tinyxml.h"
#define MY_BUFFER_LEN 2048

#define MY_ERROR_LOG	"uploader.log"

#pragma   warning(disable:4996)


using namespace std;

typedef vector<string> STRARR;

typedef struct
{
	string rule_name;
	string rule_value;
} MY_RULE_INFO;

typedef vector<MY_RULE_INFO*> RULEARR;


typedef struct
{
	string path; //操作路径
	int recursive; //0不递归 1递归
	STRARR* extensions;//后缀名
	RULEARR* rules;
	STRARR* filename;
	
} MY_FILE_INFO;

typedef vector<MY_FILE_INFO*> FILEARR;

typedef struct
{
	string type; 
	int freediskspace;
	string driver;
	FILEARR* filelist;
	string archive_name;
	string upload_ip;
	string upload_user;
	string upload_pwd;
	string upload_path;
	string suffix;
	int try_again;
	string cmdType;
	string cmdTargetName;
}OPT_INFO;

typedef struct
{
	string type;
	string name;
	string value;
	long size;
	int direct;
} PARM_INFO;

typedef vector<PARM_INFO*> PARMARR;

typedef struct
{
	string db_type; //sqlserver mysql
	string db_ip;
	string db_port;
	string db_name;
	string db_username;
	string db_passwd;
	string db_character_set;
	string sqlscript;
	string file_name;
	string serial;
	int column_num;
	PARMARR db_parms;
}DB_INFO;

typedef struct
{
	string ip; 
	string user;
	string pwd;
	string path;
	int tryagain;
	int port;
}FTP_INFO;

typedef vector<OPT_INFO*> OPTARR;
typedef vector<DB_INFO*> DBARR;
typedef vector<FTP_INFO*> FTPRR;

class config  
{
public:
	int m_iConfirm;
	int m_iDelEmptyDir;
	int m_iLog;
	int m_iEmptyRecycleBin;
	STRARR m_fileFilterList;
	string m_path;
	OPTARR m_opt;//操作集合
	int m_iFilesystem;
	int m_iDbsystem;
	DBARR m_db;
	FTPRR m_ftp;
	string m_currentdate;
	string m_currentsqldate;
	string m_curr_date_sql2;
	FILE * m_log;
	string m_svrname;
	string m_fileName;
	string m_dataDir;

public:
	config();
	virtual ~config();
	void toString();
	bool ReadConf(const char* g_szConf);
	string GetChildContent(TiXmlElement* pTiElement, const char* pszElement);
	string ReadFileContent(const char* pszFile);
	bool check_word(string strSql);
	void logerror(const char* lpszFormat,...);
	void setDate(string date);
};

string& replace_all(string& str,const string& old_value,const string& new_value);

#endif
