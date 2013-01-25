
#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <iostream>   
#include <string>   
#include <io.h>
#include <windows.h>
#include <Shellapi.h>
#include <conio.h>
#include <time.h>
#include <fstream>
#include "config.h"


//#pragma comment(lib,"Shell32.lib")



#pragma   warning(disable:4996)


#define MY_CONFIG_FILE	"conf.xml"

#define MAX_THREADS 4


config mycnf;

typedef struct
{
	string full_path;
	string file_name;
	__int64	file_size;
} MY_OPTFILE_INFO;

//getMysqlData((*opt_iter)->db_name,(*opt_iter)->db_ip,(*opt_iter)->db_username,(*opt_iter)->db_passwd,(*opt_iter)->db_port,strTmpSql,strFileName,(*opt_iter)->serial,(*opt_iter)->column_num);
typedef struct MyData {
	string db_name;
	string db_ip;
	string db_username;
	string db_password;
	string db_port;
	string db_type;
	string db_sql;
	string db_encoding;
	string file_name;
	string file_serial;
	int file_column_num;
	PARMARR db_parms;
} MYDATA, *PMYDATA;


size_t check_extension(string sFileName);

size_t check_rules(_tfinddata64_t * pFindedFileInfo);

void OffsetDateTime(const struct tm* inST, struct tm* outST,
					int dYears, int dMonths, int dDays, 
					int dHours, int dMinutes, int dSeconds);
//int fileupload(string filename,string distfilename,string upload_ip,string upload_user,string upload_pwd,string upload_path);

int FtpUpdataFiles(string& filename,string& distfilename,string& upload_ip,string& upload_user,string& upload_pwd,string& upload_path,__int64 f_size,string& sMd5,int port);

static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream);

unsigned __int64 DiskFree(string mount);

int file_index(const char *s,const char *t,int pos);
bool file_match(const char *s,const char *t);

//string& replace_all(string& str,const string& old_value,const string& new_value);

void display_usage( void );

DWORD WINAPI MyThreadFunction( LPVOID lpParam );

#endif
