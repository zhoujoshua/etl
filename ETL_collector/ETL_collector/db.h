//the head of db.cpp 
// using mysql++

#ifndef _DB_H_
#define _DB_H_

#include <string>   
#include "config.h"
#include "util/strutil.h"

#import "c:\program files\common files\system\ado\msado60.tlb" no_namespace rename("EOF","adoEOF")

using namespace std;


int getMysqlData(const string& db,const string& ip,const string& user,const string& pwd,const string& port,const string& sql,const string& filename,const string& serial,int column_num,const string& encoding);

int getMssqlData(const string& db,const string& ip,const string& user,const string& pwd,const string& port,const string& sql,const string& filename,const string& serial,int column_num,const string& encoding);

int getOracleData(const string& db,const string& ip,const string& user,const string& pwd,const string& port,const string& sql,const string& filename,const string& serial,int column_num,const string& encoding);

int getMsTsqlData(const string& db,const string& ip,const string& user,const string& pwd,const string& port,const string& sql,const string& filename,const string& serial,int column_num,const string& encoding,PARMARR pParms);


#endif