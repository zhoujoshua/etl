#include "tinyxml.h"
#include "config.h"
#include "main.h"
#include <time.h>
#include <Windows.h>
#include "xzip/XZip.h"
#include "strptime.h"
#include <shellapi.h>
#include <WinInet.h>
#include "db.h"
#include "getopt.h"


MY_FILE_INFO* g_pFileInfo;

int g_iDiskFree;

vector<MY_OPTFILE_INFO*> g_optfile_list;

STRARR g_extension_exclude;

STRARR g_uploadfileList;

using namespace std;



void filesearch(string path,int layer)   
{   
	_tfinddata64_t filefind;
	int   done=0,handle;

	char tAll[1024];
	memset(tAll,'\0',1024);
	time_t t = time(0);  
	memset(tAll,0,1024);
	strftime(tAll,sizeof(tAll),"%Y_%m_%d",localtime(&t));
	


	if (path.size() > 0)
	{
		
		string   curr=path+"\\*.*";   
			
		//char file_name[_MAX_FNAME];
		string sFilename;

		string sFullFileName;
		


		if((handle=_tfindfirst64(curr.c_str(),&filefind))==-1)return;  

		while(!(done=_tfindnext64(handle,&filefind)))   
		{   
			if(!strcmp(filefind.name,".."))continue;   
			//for(i=0;i<layer;i++)cout<<"     ";   
			if   ((_A_SUBDIR==filefind.attrib) && g_pFileInfo->recursive)   
			{               
				//cout<<filefind.name<<"(dir)"<<endl;   
				curr=path+"\\"+filefind.name;
				filesearch(curr,layer+1);   
			}   
			else
			{
				sFilename = filefind.name;
// 				string::size_type pos = sFilename.find('.');
// 				if (pos == string::npos)
// 					continue;
// 				pos = pos+1;
// 				sExt = sFilename.substr(pos);
				if (check_extension(sFilename))
				{
					if(check_rules(&filefind)){
						sFullFileName = path+"\\"+sFilename; 
						//optfile_list.push_back(sFullFileName);
						//cout<<sFullFileName<<endl;

						MY_OPTFILE_INFO* pOptFile = (MY_OPTFILE_INFO*)malloc(sizeof(MY_OPTFILE_INFO));
						memset(pOptFile, 0, sizeof(MY_OPTFILE_INFO));
						pOptFile->full_path = sFullFileName;
						pOptFile->file_name = sFilename;
						pOptFile->file_size = filefind.size;
						g_optfile_list.push_back(pOptFile);
					}
				}
			}   
		}           
		_findclose(handle);   
	}
	for (STRARR::iterator filename_iter = g_pFileInfo->filename->begin(); filename_iter != g_pFileInfo->filename->end(); ++filename_iter){
		string strTmpFileName;
		strTmpFileName = replace_all(*filename_iter,"$date",string(tAll));
		if((handle=_tfindfirst64(strTmpFileName.c_str(),&filefind)) == -1) continue;
		
		
		MY_OPTFILE_INFO* pOptFile = (MY_OPTFILE_INFO*)malloc(sizeof(MY_OPTFILE_INFO));
		memset(pOptFile, 0, sizeof(MY_OPTFILE_INFO));
		pOptFile->full_path = strTmpFileName.c_str();
		pOptFile->file_name = filefind.name;
		pOptFile->file_size = filefind.size;
		g_optfile_list.push_back(pOptFile);


	}


}

size_t check_extension(string sFileName) {
	string sExt;
	string::size_type pos = sFileName.find('.');
	if (pos == string::npos)
		sExt = "";
	else {
		pos = pos+1;
		sExt = sFileName.substr(pos);
	}



	//先检查是否是需要排除的后缀名
	for (STRARR::iterator exclude_iter = g_extension_exclude.begin(); exclude_iter != g_extension_exclude.end(); ++exclude_iter){
		if (sExt == *exclude_iter)
			return 0;
	}
	if (g_pFileInfo->extensions->size() == 0)
	{
		return 1;
	}
	for (STRARR::iterator ext_iter = g_pFileInfo->extensions->begin(); ext_iter != g_pFileInfo->extensions->end(); ++ext_iter){
		
		if(file_match(sFileName.c_str(),(*ext_iter).c_str()))
			return 1;
	}
	return 0;
}

void DeleteDirectory(string path,int layer)   
{   
	struct   _finddata_t   filefind;
	int   done=0,handle;

	if (path.size() > 0)
	{
		string   curr=path+"\\*.*";   
		if((handle=_findfirst(curr.c_str(),&filefind))==-1)return;  
		while(!(done=_findnext(handle,&filefind)))   
		{   
			if (!strcmp(filefind.name,".."))continue;   
			if ((_A_SUBDIR==filefind.attrib) && g_pFileInfo->recursive)   
			{               
				//cout<<filefind.name<<"(dir)"<<endl;   
				curr=path+"\\"+filefind.name;
				DeleteDirectory(curr,layer+1);   
			}   
		}
		_findclose(handle);
		if(layer != 0) {
			if (RemoveDirectory(path.c_str()))
			{
				mycnf.logerror("删除了空目录[%s]",path.c_str());
			}
		}
		
	}

} 


size_t check_rules(_tfinddata64_t * pFindedFileInfo) {
	int months = 0;
	int hours = 0;
	for (RULEARR::iterator rule_iter = g_pFileInfo->rules->begin(); rule_iter != g_pFileInfo->rules->end(); ++rule_iter){
		if((*rule_iter)->rule_name == "monthMore") {
			time_t rawtime;
			struct tm * st;
			struct tm outst;
			time(&rawtime);
			st = localtime(&rawtime); 

			//void OffsetDateTime(const struct tm* inST, struct tm* outST,
			//int dYears, int dMonths, int dDays, 
			//	int dHours, int dMinutes, int dSeconds);

			months = atoi(((*rule_iter)->rule_value).c_str());
			OffsetDateTime(st, &outst, 0, months, 0, 0, 0, 0);
			time_t newTime = mktime(&outst);

			if (pFindedFileInfo->time_create > newTime)
			{
				return 0;

			}

		}
		if((*rule_iter)->rule_name == "monthLess") {
			time_t rawtime;
			struct tm * st;
			struct tm outst;
			time(&rawtime);
			st = localtime(&rawtime); 

			//void OffsetDateTime(const struct tm* inST, struct tm* outST,
			//int dYears, int dMonths, int dDays, 
			//	int dHours, int dMinutes, int dSeconds);

			months = atoi(((*rule_iter)->rule_value).c_str());
			OffsetDateTime(st, &outst, 0, months, 0, 0, 0, 0);
			time_t newTime = mktime(&outst);

			if (pFindedFileInfo->time_create < newTime)
			{
				return 0;

			}

		}
		if((*rule_iter)->rule_name == "monthModifyMore") {
			time_t rawtime;
			struct tm * st;
			struct tm outst;
			time(&rawtime);
			st = localtime(&rawtime); 

			//void OffsetDateTime(const struct tm* inST, struct tm* outST,
			//int dYears, int dMonths, int dDays, 
			//	int dHours, int dMinutes, int dSeconds);

			months = atoi(((*rule_iter)->rule_value).c_str());
			OffsetDateTime(st, &outst, 0, months, 0, 0, 0, 0);
			time_t newTime = mktime(&outst);

			if (pFindedFileInfo->time_write > newTime)
			{
				return 0;

			}

		}
		if((*rule_iter)->rule_name == "monthModifyLess") {
			time_t rawtime;
			struct tm * st;
			struct tm outst;
			time(&rawtime);
			st = localtime(&rawtime); 

			//void OffsetDateTime(const struct tm* inST, struct tm* outST,
			//int dYears, int dMonths, int dDays, 
			//	int dHours, int dMinutes, int dSeconds);

			months = atoi(((*rule_iter)->rule_value).c_str());
			OffsetDateTime(st, &outst, 0, months, 0, 0, 0, 0);
			time_t newTime = mktime(&outst);

			if (pFindedFileInfo->time_write < newTime)
			{
				return 0;

			}

		}
		if((*rule_iter)->rule_name == "dateBefore") {
			time_t newTime;
			if ((*rule_iter)->rule_value.size() >10)
			{
				struct tm* tmp_time = (struct tm*)malloc(sizeof(struct tm));
				strptime((*rule_iter)->rule_value.c_str(),"%Y-%m-%d %T",tmp_time);
				newTime = mktime(tmp_time);
				free(tmp_time); 
			} else {
				time_t rawtime;
				struct tm * st;
				struct tm outst;
				time(&rawtime);
				st = localtime(&rawtime); 

				hours = atoi(((*rule_iter)->rule_value).c_str());
				OffsetDateTime(st, &outst, 0, 0, 0, hours, 0, 0);
				newTime = mktime(&outst);

			}
			

			
			if (pFindedFileInfo->time_create > newTime)
			{
				
				return 0;

			}

		}
		if((*rule_iter)->rule_name == "dateAfter") {
			time_t newTime;
			if ((*rule_iter)->rule_value.size() >10)
			{
				struct tm* tmp_time = (struct tm*)malloc(sizeof(struct tm));
				strptime((*rule_iter)->rule_value.c_str(),"%Y-%m-%d %T",tmp_time);
				newTime = mktime(tmp_time);
				free(tmp_time); 
			} else {
				time_t rawtime;
				struct tm * st;
				struct tm outst;
				time(&rawtime);
				st = localtime(&rawtime); 

				hours = atoi(((*rule_iter)->rule_value).c_str());
				OffsetDateTime(st, &outst, 0, 0, 0, hours, 0, 0);
				newTime = mktime(&outst);

			}
			if (pFindedFileInfo->time_create < newTime)
			{
				return 0;

			}

		}
		if ((*rule_iter)->rule_name == "lessThanSize")
		{
			unsigned long ulSize = 0;
			ulSize = atoi(((*rule_iter)->rule_value).c_str());
			if (((double)pFindedFileInfo->size)/1024/1024 > ulSize)
			{
				return 0;

			}
		}
		if ((*rule_iter)->rule_name == "moreThanSize")
		{
			unsigned long ulSize = 0;
			ulSize = atoi(((*rule_iter)->rule_value).c_str());
			if (((double)pFindedFileInfo->size)/1024/1024 < ulSize)
			{
				return 0;

			}
		}
	}
	return 1;
}



int main(int argc,char * argv[])
{
	int c;
	char conf[1024];
//	system("PAUSE"); 
	ZeroMemory(conf,1024);
	if (argc <= 2)
	{
		display_usage();
		return -1;
	}
	while (1)
	{
		int this_option_optind = optind ? optind : 1;
		c = getopt (argc, argv, "c:t:f:");
		if (c == -1)
			break;
		switch (c)
		{
		case 'c':
			strcpy(conf,optarg);
			break;
		case 't':
			mycnf.setDate(string(optarg));
			break;
		case 'f':
			mycnf.m_fileName = optarg;
			break;
		case '?':
			display_usage();
			return -1;
		case ':':
			printf("need parameter: %c\n", optopt);
			break;
		default:
			display_usage();
			return -1;
		}
	}
	if (!strlen(conf))
	{
		mycnf.logerror("config file error , we exit");
		return -1;
	}

	if(!mycnf.ReadConf(conf))
	{
		//mycnf.logerror("config file error , we exit");
		return -1;
	}

 	
	int iRet;
	string tarfilename;

	if(!CreateDirectory(mycnf.m_dataDir.c_str(),NULL))
		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{
			mycnf.logerror("create directory[%s] failure!",mycnf.m_dataDir.c_str());
			return -1;
		}

	if(!SetCurrentDirectory(mycnf.m_dataDir.c_str())) {
		mycnf.logerror("SetCurrentDirectory failure!Error:%d",GetLastError());
		return -1;
	}

	int i = 0;
	for (vector<DB_INFO*>::iterator opt_iter = mycnf.m_db.begin(); opt_iter != mycnf.m_db.end(); ++opt_iter)
	{
		if (mycnf.m_fileName.length() > 0 && mycnf.m_fileName !=  (*opt_iter)->file_name )
		{
			continue;
		}
		i++;
	}
	int thread_num = i;
	PMYDATA *pDataArray;
	pDataArray = (PMYDATA*)malloc(sizeof(PMYDATA)*thread_num);
	DWORD   dwThreadIdArray[1024];
	HANDLE  *hThreadArray;
	hThreadArray = (HANDLE*)malloc(sizeof(HANDLE)*thread_num);
	

	i = 0;
	for (vector<DB_INFO*>::iterator opt_iter = mycnf.m_db.begin(); opt_iter != mycnf.m_db.end(); ++opt_iter)
	{
		if (mycnf.m_fileName.length() > 0 && mycnf.m_fileName !=  (*opt_iter)->file_name )
		{
			continue;
		}
		pDataArray[i] = (PMYDATA) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,sizeof(MYDATA));

		string strFileName;
		strFileName = (*opt_iter)->file_name + "--" + mycnf.m_svrname + "__" + mycnf.m_currentsqldate + ".etl" ;
		//日期替换
		string strTmpSql;
		strTmpSql = replace_all((*opt_iter)->sqlscript,"$date",mycnf.m_curr_date_sql2);
		//mycnf.logerror("%s",strTmpSql.c_str());

		pDataArray[i]->db_ip			= (*opt_iter)->db_ip;
		pDataArray[i]->db_name			= (*opt_iter)->db_name;
		pDataArray[i]->db_password		= (*opt_iter)->db_passwd;
		pDataArray[i]->db_port			= (*opt_iter)->db_port;
		pDataArray[i]->db_type			= (*opt_iter)->db_type;
		pDataArray[i]->db_sql			= strTmpSql;
		pDataArray[i]->db_username		= (*opt_iter)->db_username;
		pDataArray[i]->file_column_num	= (*opt_iter)->column_num;
		pDataArray[i]->file_name		= strFileName;
		pDataArray[i]->file_serial		= (*opt_iter)->serial;
		pDataArray[i]->db_encoding		= (*opt_iter)->db_character_set;
		pDataArray[i]->db_parms		=  (*opt_iter)->db_parms;
		
		hThreadArray[i] = CreateThread( 
			NULL,                   // default security attributes
			0,                      // use default stack size  
			MyThreadFunction,       // thread function name
			pDataArray[i],          // argument to thread function 
			0,                      // use default creation flags 
			&dwThreadIdArray[i]);   // returns the thread identifier 
		if (hThreadArray[i] == NULL) 
		{
			mycnf.logerror("CreateThread failed[%s]",strFileName);
		}
		Sleep(2000);
		i++;
	}
	
	WaitForMultipleObjects(thread_num, hThreadArray, TRUE, INFINITE);

	
	for(int i=0; i<thread_num; i++)
	{
		CloseHandle(hThreadArray[i]);
		if(pDataArray[i] != NULL)
		{
			HeapFree(GetProcessHeap(), 0, pDataArray[i]);
			pDataArray[i] = NULL;    // Ensure address is not reused.
		}
	}

		

	tarfilename = mycnf.m_svrname + "_" + mycnf.m_currentdate + ".zip" ;
	//压缩
	HZIP hz = CreateZip((void *)_T(tarfilename.c_str()), 0, ZIP_FILENAME);

	if (hz)
	{
		for (STRARR::iterator exe_iter = g_uploadfileList.begin(); exe_iter != g_uploadfileList.end(); ++exe_iter){
			//string stmp = (*exe_iter)->full_path.substr(len);

			if(ZipAdd(hz, _T((*exe_iter).c_str()),  (void *)_T((*exe_iter).c_str()), 0, ZIP_FILENAME) == ZR_OK) {
				mycnf.logerror("文件[%s]压缩成功",(*exe_iter).c_str());
			} else {
				mycnf.logerror("文件[%s]压缩失败",(*exe_iter).c_str());
			}
		}
		CloseZip(hz);
	} else {
		mycnf.logerror("生成压缩包：%s 失败,请检查磁盘空间及文件路径",tarfilename.c_str());
		return -1;
	}

	//上传文件
	for (vector<FTP_INFO*>::iterator ftp_iter = mycnf.m_ftp.begin(); ftp_iter != mycnf.m_ftp.end(); ++ftp_iter)
	{
		//int FtpUpdataFiles(string filename,string distfilename,string upload_ip,string upload_user,string upload_pwd,string upload_path,__int64 f_size,string sMd5);
		iRet = FtpUpdataFiles(tarfilename,tarfilename,(*ftp_iter)->ip,(*ftp_iter)->user,(*ftp_iter)->pwd,(*ftp_iter)->path,0,string(""),(*ftp_iter)->port);
		if (iRet == -1)
		{
			mycnf.logerror("文件[%s]上传失败",tarfilename.c_str());
		} else {
			mycnf.logerror("文件[%s]上传成功",tarfilename.c_str());
		}
	}


	return 0;

}

void OffsetDateTime(const struct tm* inST, struct tm* outST,
					int dYears, int dMonths, int dDays, 
					int dHours, int dMinutes, int dSeconds)
{
	if (inST != NULL && outST != NULL)
	{
		// 偏移当前时间 
		outST->tm_year = inST->tm_year - dYears;
		outST->tm_mon  = inST->tm_mon  - dMonths;
		outST->tm_mday = inST->tm_mday - dDays;
		outST->tm_hour = inST->tm_hour - dHours;
		outST->tm_min  = inST->tm_min  - dMinutes;
		outST->tm_sec  = inST->tm_sec  - dSeconds;

		// 转换到从1900.1.1开始的总秒数
		time_t newRawTime = mktime(outST);

		// 将秒数转换成时间结构体
		outST = localtime(&newRawTime);
	}
}

static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
	/* in real-world cases, this would probably get this data differently
	as this fread() stuff is exactly what the library already would do
	by default internally */ 
	size_t retcode = fread(ptr, size, nmemb, (FILE *)stream);


	//fprintf(stderr, "*** We read %d bytes from file\n", retcode);
	return retcode;
}



unsigned __int64 DiskFree(string mount)
{
	ULARGE_INTEGER nFreeBytesAvailable;
	ULARGE_INTEGER nTotalNumberOfBytes;
	ULARGE_INTEGER nTotalNumberOfFreeBytes;
	
	if (GetDiskFreeSpaceEx(_T(mount.c_str()),

	&nFreeBytesAvailable,
	&nTotalNumberOfBytes,
	&nTotalNumberOfFreeBytes))
	{
// 		TCHAR chBuf[256];
// 		wsprintf(chBuf,_T("Av=%I64d,Total=%I64d,Free=%I64d\r\n"),
// 		nFreeBytesAvailable,
// 		nTotalNumberOfBytes,
// 		nTotalNumberOfFreeBytes);
// 		OutputDebugString(chBuf);
		return nTotalNumberOfFreeBytes.QuadPart;
		
	} else {
		return 0;
	}
}

int file_index(const char *s,const char *t,int pos)
{
	int i=pos,j=0,lens=strlen(s),lent=strlen(t);
	while(i<lens&&j<lent)
	{
		if(s[i]==t[j]||t[j]=='?')
		{
			++i;
			++j;
		}
		else
		{
			i=i-j+1;
			j=0;
		}
	}
	if(j==lent)return i-lent;
	else return -1;
}
bool file_match(const char *s,const char *t)
{
	int i=0,j=0,lens=strlen(s),lent=strlen(t);
	char buf[128];
	int bufp=0;
	while(j<lent&&t[j]!='*')
	{
		buf[bufp]=t[j];
		++bufp;++j;
	}
	buf[bufp]='\0';
	if(file_index(s,buf,0)!=0)return false;
	i=bufp;
	while(1)
	{
		while(j<lent&&t[j]=='*')++j;
		if(j==lent)return true;
		bufp=0;
		while(j<lent&&t[j]!='*')
		{
			buf[bufp]=t[j];
			++bufp;++j;
		}
		buf[bufp]='\0';
		if(j==lent)
		{
			if(file_index(s,buf,i)!=lens-bufp)return false;
			return true;
		}
		if((i=file_index(s,buf,i))==-1)return false;
		i+=bufp;
	}
	return true;
}



int FtpUpdataFiles(string& filename,string& distfilename,string& upload_ip,string& upload_user,string& upload_pwd,string& upload_path,__int64 f_size,string& sMd5,int port)
{
	HINTERNET	hNet,hFind,hConnection;
	__int64   iFileLen=0; 


	hNet = InternetOpen("Mozilla/4.0 (compatible)", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (hNet==NULL)
	{
		mycnf.logerror("InternetOpen error '%s': %d\n", filename.c_str(), GetLastError());
		return -1;
	}

	hConnection = InternetConnect(hNet, upload_ip.c_str(), port, upload_user.c_str(), upload_pwd.c_str(), INTERNET_SERVICE_FTP,INTERNET_FLAG_PASSIVE, 0);
	if (hConnection==NULL)
	{
		mycnf.logerror("InternetConnect error '%s': %d\n", filename.c_str(), GetLastError());
		InternetCloseHandle(hNet);
		return -1;
	}

	char strRemoteFolder[MAX_PATH] = "/";	//设置当前目录为服务器根目录
	if (FtpSetCurrentDirectory(hConnection, strRemoteFolder)==false)
	{
		mycnf.logerror("FtpSetCurrentDirectory 1 error '%s': %d\n", filename.c_str(), GetLastError());
		InternetCloseHandle(hConnection);
		InternetCloseHandle(hNet);
		return -1;
	}

	char strDirectory[MAX_PATH];
	memset(strDirectory,0,sizeof(strDirectory));
	lstrcpy(strDirectory,upload_path.c_str());

	if (strstr(strDirectory, "/") != NULL)	//是否在根目录要判断一下
	{
		lstrcpy(strRemoteFolder,strDirectory);
		if (FtpSetCurrentDirectory(hConnection, strRemoteFolder)==false)
		{
			mycnf.logerror("FtpSetCurrentDirectory 2 error '%s': %d\n", strRemoteFolder, GetLastError());
			InternetCloseHandle(hConnection);
			InternetCloseHandle(hNet);
			return -1;
		}
	}else
	{
		mycnf.logerror("directory error '%s'\n", strDirectory);
		InternetCloseHandle(hConnection);
		InternetCloseHandle(hNet);
		return -1;
	}

	//每天建一个目录
	WIN32_FIND_DATA dirInfo;
	hFind = FtpFindFirstFile( hConnection, mycnf.m_currentdate.c_str(), 
		&dirInfo, INTERNET_FLAG_RELOAD, 0 );
	if (hFind == NULL)
	{
		if( !FtpCreateDirectory( hConnection, mycnf.m_currentdate.c_str() ) )
		{
			mycnf.logerror("FtpCreateDirectory error '%s'\n", mycnf.m_currentdate.c_str());
			InternetCloseHandle(hConnection);
			InternetCloseHandle(hNet);
			return -1;
		}
		string strTmpRemoteFolder = strRemoteFolder + mycnf.m_currentdate;
		if (FtpSetCurrentDirectory(hConnection, strTmpRemoteFolder.c_str())==false)
		{
			mycnf.logerror("FtpSetCurrentDirectory 3 error '%s': %d\n", strTmpRemoteFolder.c_str(), GetLastError());
			InternetCloseHandle(hConnection);
			InternetCloseHandle(hNet);
			return -1;
		}
	} else {
		string strTmpRemoteFolder = strRemoteFolder + mycnf.m_currentdate;
		if (FtpSetCurrentDirectory(hConnection, strTmpRemoteFolder.c_str())==false)
		{
			mycnf.logerror("FtpSetCurrentDirectory 4 error '%s': %d\n", strTmpRemoteFolder.c_str(), GetLastError());
			InternetCloseHandle(hConnection);
			InternetCloseHandle(hNet);
			return -1;
		}
		InternetCloseHandle(hFind);
	}
	
	string tmpFileName = "";
	
	tmpFileName = distfilename+".tmp";



// 	//上传文件
	if (!FtpPutFile(hConnection, filename.c_str(), tmpFileName.c_str(),FTP_TRANSFER_TYPE_BINARY, 0))
	{
		mycnf.logerror("FtpPutFile error '%s' '%s': %d\n", filename.c_str(),tmpFileName.c_str(), GetLastError());
		InternetCloseHandle(hConnection);
		InternetCloseHandle(hNet);
		return -1;
	}

	
// 	string sCmd;
// 	sCmd = string("SIZE ") + tmpFileName.c_str();
// 	FtpCommand(hConnection, FALSE, FTP_TRANSFER_TYPE_ASCII, _T(sCmd.c_str()), 0, NULL);
// 
// 	char szBuff[128];
// 	ZeroMemory(szBuff,128);
// 	DWORD dwBuffSize = 128;
// 	DWORD dwLastErrorMsg;
// 	if(!InternetGetLastResponseInfo( &dwLastErrorMsg, szBuff, &dwBuffSize)) {
// 		mycnf.logerror("InternetGetLastResponseInfo error '%s': %d  '%s'\n", tmpFileName.c_str(), GetLastError(),szBuff);
// 		InternetCloseHandle(hConnection);
// 		InternetCloseHandle(hNet);
// 		return -1;
// 	}
// 
// 	__int64 filesize = _tstoi64(szBuff+3);
// 
// 	
// 	if (filesize != f_size)
// 	{
// 		mycnf.logerror("文件大小不匹配 '%s': %I64d %I64d\n", tmpFileName.c_str(), f_size,filesize);
// 		InternetCloseHandle(hConnection);
// 		InternetCloseHandle(hNet);
// 		return -1;
// 	}
// 	
	//检查原文件是否存在
	hFind = FtpFindFirstFile( hConnection, distfilename.c_str(), 
		&dirInfo, INTERNET_FLAG_RELOAD, 0 );
	if (hFind != NULL)
	{
		if( !FtpDeleteFile( hConnection, distfilename.c_str() ) )
		{
			mycnf.logerror("FtpDeleteFile error '%s': %d\n", distfilename.c_str(), GetLastError());
			InternetCloseHandle(hConnection);
			InternetCloseHandle(hNet);
			return -1;
		}
	}


	//上传完毕改名
	if( !FtpRenameFile(hConnection, tmpFileName.c_str(), distfilename.c_str()))
	{
		mycnf.logerror("FtpRenameFile error '%s' '%s': %d\n", tmpFileName.c_str(),distfilename.c_str(), GetLastError());
		InternetCloseHandle(hConnection);
		InternetCloseHandle(hNet);
		return -1;
	}
	
 	

// 	//生成md5值文件
// 	fstream fileMD5(sLocalMD5FileName.c_str(), std::ios::app);
// 	if (!fileMD5.fail())
// 	{
// 		fileMD5 << f_size;
// 	}
// 	else
// 	{
// 		mycnf.logerror("MD5 file fstream error '%s' '%s': %d\n", sLocalMD5FileName.c_str(),distfilename.c_str(), GetLastError());
// 		InternetCloseHandle(hConnection);
// 		InternetCloseHandle(hNet);
// 		return -1;
// 	}
// 
// 	fileMD5.close();
// 	//上传md5文件
// 	if (!FtpPutFile(hConnection, sLocalMD5FileName.c_str(), sDistMD5FileName.c_str(),FTP_TRANSFER_TYPE_BINARY, 0))
// 	{
// 		mycnf.logerror("md5 FtpPutFile error '%s' '%s': %d\n", sLocalMD5FileName.c_str(),sDistMD5FileName.c_str(), GetLastError());
// 		InternetCloseHandle(hConnection);
// 		InternetCloseHandle(hNet);
// 		return -1;
// 	}
// 
// 	_unlink(sLocalMD5FileName.c_str());

	
	InternetCloseHandle(hConnection);
	InternetCloseHandle(hNet);
	return 0;

}

void display_usage( void )
{
	cout << "使用方法：" << endl;
	cout << "  ETL_collector.exe -c config [-t date] [-f TableName]" << endl;
	cout << "  -c [必选]：配置文件的路径及文件名，如conf.xml" << endl;
	cout << "  -t [可选]：指定读取数据的日期，缺省为当前时间，格式为YYYY-MM-DD" << endl;
	cout << "  -f [可选]：指定读取数据的表名，缺省为全部表" << endl;
}

DWORD WINAPI MyThreadFunction( LPVOID lpParam ) 
{ 
	PMYDATA pDataArray;
	pDataArray = (PMYDATA)lpParam;
	int iRet=0;


	if (pDataArray->db_type == "mysql")
	{
		iRet = getMysqlData(pDataArray->db_name,pDataArray->db_ip,pDataArray->db_username,pDataArray->db_password,pDataArray->db_port,pDataArray->db_sql,pDataArray->file_name,pDataArray->file_serial,pDataArray->file_column_num,pDataArray->db_encoding);
	}
	if (pDataArray->db_type == "mssql") {
		iRet = getMssqlData(pDataArray->db_name,pDataArray->db_ip,pDataArray->db_username,pDataArray->db_password,pDataArray->db_port,pDataArray->db_sql,pDataArray->file_name,pDataArray->file_serial,pDataArray->file_column_num,pDataArray->db_encoding);
	}
	if (pDataArray->db_type == "oracle") {
		iRet = getOracleData(pDataArray->db_name,pDataArray->db_ip,pDataArray->db_username,pDataArray->db_password,pDataArray->db_port,pDataArray->db_sql,pDataArray->file_name,pDataArray->file_serial,pDataArray->file_column_num,pDataArray->db_encoding);
	}
	if (pDataArray->db_type == "tsql") {
		iRet = getMsTsqlData(pDataArray->db_name,pDataArray->db_ip,pDataArray->db_username,pDataArray->db_password,pDataArray->db_port,pDataArray->db_sql,pDataArray->file_name,pDataArray->file_serial,pDataArray->file_column_num,pDataArray->db_encoding,pDataArray->db_parms);
	}

	if (iRet < 0)
	{
		return -1;
	} else {
		g_uploadfileList.push_back(pDataArray->file_name);
	}

	return 0; 
} 