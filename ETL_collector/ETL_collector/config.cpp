// config.cpp: implementation of the config class.
//
//////////////////////////////////////////////////////////////////////


#include "config.h"
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "strptime.h"
#include <windows.h>
#include <iostream>   

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


config::config()
{
	m_iFilesystem = 0;
	m_iDbsystem = 0;
	//当前日期(目录格式)
	char tAll[1024];
	memset(tAll,'\0',1024);
	time_t t = time(0);
	time_t t2;
	t2 = t-24*3600;
	memset(tAll,0,1024);
	strftime(tAll,sizeof(tAll),"%Y_%m_%d",localtime(&t));
	m_currentdate = tAll;
	//当前日期(数据库格式)
	memset(tAll,0,1024);
	strftime(tAll,sizeof(tAll),"%Y-%m-%d",localtime(&t));
	m_currentsqldate = tAll;
	memset(tAll,0,1024);
	strftime(tAll,sizeof(tAll),"%Y-%m-%d",localtime(&t2));
	m_curr_date_sql2 = tAll;
		
	//日志句柄
	m_log = fopen(MY_ERROR_LOG,"a");

}

config::~config()
{
	if (m_iConfirm)
	{
		//system("PAUSE"); 
	}
	if(NULL != m_log) {
		fclose(m_log);
	}
}


bool config::ReadConf(const char* szConf)
{
	//read conf
	string sConf = ReadFileContent(szConf);
	string sTemp;
	
	if (sConf.empty())
	{
		logerror("parse config file : [%s] error",szConf);
		return false;
	}
	
	TiXmlDocument doc;
	doc.Parse(sConf.c_str());

	if (doc.Error())
	{
		logerror("parse config file: [%s] error, description = %s",szConf,doc.ErrorDesc());
		return false;
	}
	
	TiXmlElement* pTiShanda = doc.RootElement();
	if (pTiShanda == NULL)
	{
		logerror("cannot find snda tag in config file : [%s]",szConf);
		return false;
	}

	sTemp = "";
	sTemp = GetChildContent(pTiShanda,"confirm");
	if(sTemp.empty())
	{
		m_iConfirm = 0;
	} else {
		if(sTemp == "ON") {
			m_iConfirm = 1;
		} else {
			m_iConfirm = 0;
		}
	}

	sTemp = "";
	sTemp = GetChildContent(pTiShanda,"log");
	if(sTemp.empty())
	{
		m_iLog = 1;
	} else {
		if(sTemp == "ON") {
			m_iLog = 1;
		} else {
			m_iLog = 0;
		}
	}

	sTemp = "";
	sTemp = GetChildContent(pTiShanda,"svrName");
	if(sTemp.empty())
	{
		logerror("svrName not specified in config file : [%s]",szConf);
		return false;
	}
	m_svrname = sTemp;


	


	sTemp = GetChildContent(pTiShanda,"dataDir");
	if(sTemp.empty())
	{
		logerror("dataDir not specified in config file : [%s]",szConf);
		return false;
	}
	m_dataDir = sTemp;
	
	

	TiXmlElement* pTiSystemType2 = pTiShanda->FirstChildElement("dbsystem");
	if (pTiSystemType2)
	{
		m_iDbsystem = 1;
	}
	
	if (1 == m_iDbsystem) {
		for(TiXmlElement* pTiOperate = pTiSystemType2->FirstChildElement("database");pTiOperate != NULL;pTiOperate=pTiOperate->NextSiblingElement("database"))
		{
			DB_INFO* pDB = (DB_INFO*)calloc(1,sizeof(DB_INFO));
				
			
			sTemp = "";
			sTemp = GetChildContent(pTiOperate,"type");
			if(sTemp.empty())
			{
				logerror("db type not specified in config file : [%s]",szConf);
				return false;
			} else {
				pDB->db_type = sTemp.c_str();
			}

			sTemp = "";
			sTemp = GetChildContent(pTiOperate,"ip");
			if(sTemp.empty())
			{
				logerror("db ip not specified in config file : [%s]",szConf);
				return false;
			} else {
				pDB->db_ip = sTemp.c_str();
			}

			sTemp = "";
			sTemp = GetChildContent(pTiOperate,"port");
			if(sTemp.empty())
			{
				logerror("db port not specified in config file : [%s]",szConf);
				return false;
			} else {
				pDB->db_port = sTemp.c_str();
			}

		
			sTemp = "";
			sTemp = GetChildContent(pTiOperate,"db");
			if(sTemp.empty())
			{
				logerror("db name not specified in config file : [%s]",szConf);
				return false;
			} else {
				pDB->db_name = sTemp.c_str();
			}

			sTemp = "";
			sTemp = GetChildContent(pTiOperate,"user");
			if(sTemp.empty())
			{
				logerror("db username not specified in config file : [%s]",szConf);
				return false;
			} else {
				pDB->db_username = sTemp.c_str();
			}

			sTemp = "";
			sTemp = GetChildContent(pTiOperate,"password");
			if(sTemp.empty())
			{
				logerror("db username not specified in config file : [%s]",szConf);
				return false;
			} else {
				pDB->db_passwd = sTemp.c_str();
			}
			
			sTemp = "";
			sTemp = GetChildContent(pTiOperate,"encoding");
			if(sTemp.empty())
			{
				pDB->db_character_set = "utf8";
			} else {
				pDB->db_character_set = sTemp.c_str();
			}


			sTemp = "";
			sTemp = GetChildContent(pTiOperate,"sql");
			if (sTemp.size()>0)
			{
				if (!check_word(sTemp))
				{
					logerror("sql warning in config file : [%s]",szConf);
					return false;
				}
			}

			if(pDB->db_type == "tsql") {

				//存储过程参数
				TiXmlElement* pTiParms = pTiOperate->FirstChildElement("parms");
				if (pTiParms == NULL )
				{
					logerror("tag parms not specified in config file : [%s]",szConf);
					return false;
				}
			
				for(TiXmlElement* pTiParm = pTiParms->FirstChildElement("parm");pTiParm != NULL;pTiParm = pTiParm->NextSiblingElement("parm"))
				{
					PARM_INFO* pParm = (PARM_INFO*)calloc(1,sizeof(PARM_INFO));

					pParm->type = pTiParm->Attribute("type");
					pParm->name = pTiParm->Attribute("name");
					pParm->value = pTiParm->FirstChild()->Value();
					pParm->value = replace_all(pParm->value,"$date",m_curr_date_sql2);
					string sDir = pTiParm->Attribute("dir");
					if(sDir == "in"){
						pParm->direct = 1;
					} else if(sDir == "return") {
						pParm->direct = 4;
					} else {
						pParm->direct = 0;
					}
					pParm->size = atoi(pTiParm->Attribute("size"));

					pDB->db_parms.push_back(pParm);
				}
			}


			pDB->sqlscript = sTemp.c_str();

			sTemp = "";
			sTemp = GetChildContent(pTiOperate,"file");
			if(sTemp.empty())
			{
				logerror("file name not specified in config file : [%s]",szConf);
				return false;
			} else {
				pDB->file_name = sTemp.c_str();
			}

			sTemp = "";
			sTemp = GetChildContent(pTiOperate,"serial");
			if(sTemp.empty())
			{
				logerror("serial not specified in config file : [%s]",szConf);
				return false;
			} else {
				pDB->serial = sTemp.c_str();
			}


			sTemp = GetChildContent(pTiOperate,"columnNum");
			if(sTemp.empty())
			{
				logerror("columnNum not specified in config file : [%s]",szConf);
				return false;
			} else {
				pDB->column_num = atoi(sTemp.c_str());
			}

			m_db.push_back(pDB);
		}
	}
	TiXmlElement* pTiUpload = pTiShanda->FirstChildElement("upload");
	for(TiXmlElement* pTiftp = pTiUpload->FirstChildElement("ftp");pTiftp != NULL;pTiftp=pTiftp->NextSiblingElement("ftp"))
	{
		FTP_INFO* pFTP = (FTP_INFO*)calloc(1,sizeof(FTP_INFO));


		sTemp = "";
		sTemp = GetChildContent(pTiftp,"ip");
		if(sTemp.empty())
		{
			logerror("ftp ip not specified in config file : [%s]",szConf);
			return false;
		} else {
			pFTP->ip = sTemp.c_str();
		}

		sTemp = GetChildContent(pTiftp,"port");
		if(sTemp.empty())
		{
			logerror("ftp port not specified in config file : [%s]",szConf);
			return false;
		} else {
			pFTP->port = atoi(sTemp.c_str());
		}


		sTemp = "";
		sTemp = GetChildContent(pTiftp,"user");
		if(sTemp.empty())
		{
			logerror("ftp user not specified in config file : [%s]",szConf);
			return false;
		} else {
			pFTP->user = sTemp.c_str();
		}

		sTemp = "";
		sTemp = GetChildContent(pTiftp,"pwd");
		if(sTemp.empty())
		{
			logerror("ftp pwd not specified in config file : [%s]",szConf);
			return false;
		} else {
			pFTP->pwd = sTemp.c_str();
		}


		sTemp = "";
		sTemp = GetChildContent(pTiftp,"path");
		if(sTemp.empty())
		{
			pFTP->path = "";
		} else {
			pFTP->path = sTemp.c_str();
		}

		sTemp = "";
		sTemp = GetChildContent(pTiftp,"tryagain");
		if(sTemp.empty())
		{
			logerror("tryagain not specified in config file : [%s]",szConf);
			return false;
		} else {
			pFTP->tryagain = atoi(sTemp.c_str());
		}
		m_ftp.push_back(pFTP);
	}

	return true;
}

bool config::check_word(string strSql){
	size_t found;

	int length2 = strSql.size(); 
	for (int i = 0; i < length2; ++i) 
	{ 
		strSql[i] = tolower(strSql[i]); 
	} 

	found = strSql.find("drop");
	if (found!=string::npos) {
		logerror("sql [%s] has 'drop',pession denied",strSql.c_str());
		return false;

	}

	return true;
}


string config::GetChildContent(TiXmlElement* pTiElement, const char* pszElement)
{
	if (pTiElement == NULL)
	{	
		//log("Read pTiElement error");
		//cerr << "Read pTiElement error" << endl;
		return string();
	}
	TiXmlElement* pTiChild = pTiElement->FirstChildElement(pszElement);
	if (pTiChild == NULL)
	{	
		//log("Read pTiChild error: %s",pszElement);
		//cerr << "Read pTiChild error:" << pszElement << endl;
		return string();
	}
	
	TiXmlNode* pTiNode = pTiChild->FirstChild();
	if (pTiNode == NULL)
	{	
		//log("Read pTiNode error");
		return string();
	}
	
	return pTiNode->Value();
}


string config::ReadFileContent(const char* pszFile)
{
	char* pszContent = NULL;
	int nContentLen = 0;

	FILE* pf = fopen(pszFile, "rb");
	if (pf == NULL)
	{
		//log("config file not exists");
		return string();
	}
	
	while (!feof(pf))
	{
		pszContent = (char*)realloc(pszContent, nContentLen+1024+1);
		memset(pszContent+nContentLen, 0, 1024+1);
		int nLen = fread(pszContent+nContentLen, 1, 1024, pf);
		nContentLen += nLen;
	}
	fclose(pf);
	
	string sContent = pszContent;
	free(pszContent);
	return sContent;
}

//错误提示
void config::logerror(const char* lpszFormat,...)
{
	char buf[MY_BUFFER_LEN];
	va_list argList;
	va_start(argList, lpszFormat);	  
	vsprintf(buf, lpszFormat,argList);	  
	va_end(argList);
	//int semop_ret;

	time_t timer = time(NULL);
	struct tm *tblock;
	tblock=localtime(&timer);
	char buf2[MY_BUFFER_LEN];
	ZeroMemory(buf2,MY_BUFFER_LEN);
	memcpy(buf2,asctime(tblock),strlen(asctime(tblock))-1);


	cout<<buf<<endl;
	if (m_iLog)
	{
		if(NULL != m_log)
		{

			fprintf(m_log,"%s %s\r\n",buf2,buf);
			fflush(m_log);
		}
	}

}

void config::setDate(string date)
{
	string strDate;
	time_t timeData;
	strDate = replace_all(date,"_","-");

	m_currentsqldate  = strDate;

	strDate = replace_all(date,"-","_");

	m_currentdate = strDate;

	char *pBeginPos = (char*) m_currentsqldate.c_str();
	char *pPos = strstr(pBeginPos,"-");
	if(pPos == NULL)
	{
		return;
	}
	int iYear = atoi(pBeginPos);
	int iMonth = atoi(pPos + 1);

	pPos = strstr(pPos + 1,"-");
	if(pPos == NULL)
	{
		return;
	}

	int iDay = atoi(pPos + 1);

	struct tm sourcedate;
	ZeroMemory(&sourcedate,sizeof(sourcedate));
	sourcedate.tm_mday = iDay;
	sourcedate.tm_mon = iMonth - 1; 
	sourcedate.tm_year = iYear - 1900;

	timeData = mktime(&sourcedate);  
	time_t t2;
	t2 = timeData-24*3600;
	char tAll[1024];
	memset(tAll,0,1024);
	strftime(tAll,sizeof(tAll),"%Y-%m-%d",localtime(&t2));
	m_curr_date_sql2 = tAll;
}

string& replace_all(string& str,const string& old_value,const string& new_value) 
{ 

	while(true) { 

		string::size_type pos(0); 

		if( (pos=str.find(old_value))!=string::npos ) 

			str.replace(pos,old_value.length(),new_value); 

		else break; 

	} 

	return str; 

} 

