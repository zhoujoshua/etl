//

#include "db.h"
#include "config.h"
#include <mysql++.h>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include "ChineseCodeLib.h"

using namespace std;

extern config mycnf;

int getMysqlData(const string& db,const string& ip,const string& user,const string& pwd,const string& port,const string& sql,const string& filename,const string& serial,int column_num,const string& encoding){
	// Connect to the sample database.
	DWORD   dwStart,dwStop;   

	dwStart = GetTickCount();  

	mysqlpp::Connection conn(true);
	
	if(encoding == "utf8") {
		conn.set_option( new mysqlpp::SetCharsetNameOption("utf8") );
	}

	if(encoding == "gbk") {
		conn.set_option( new mysqlpp::SetCharsetNameOption("gbk") );
	}

	

	string file_arr[1024];
	vector<string> vSplit;
	vSplit = strutil::split(serial,",");

	fstream file(filename.c_str(),ios::trunc|ios::out);
	if(!file)
	{
		mycnf.logerror("open file error");
		return -1;
	}

	try {	
		conn.connect(db.c_str(), ip.c_str(),user.c_str(), pwd.c_str(),atoi(port.c_str()));

		if (!conn.connected())
		{
			mycnf.logerror("Database connection failed: %s" ,conn.error()); 
			return -1;
		}


		// Ask for all rows from the sample stock table and display
		// them.  Unlike simple2 example, we retreive each row one at
		// a time instead of storing the entire result set in memory
		// and then iterating over it.
		if(encoding == "utf8") {
			conn.query("SET NAMES utf8"); 
		}
		if(encoding == "gbk") {
			conn.query("SET NAMES gbk"); 
		}
		mysqlpp::Query query = conn.query(sql);
		if (mysqlpp::UseQueryResult res = query.use()) {
			int j = 0;
			// Get each row in result set, and print its contents
			while (mysqlpp::Row row = res.fetch_row()) {
				int num = row.size();
				j++;
				if (vSplit.size() != num)
				{
					mycnf.logerror("数据库返回数据列与<serial>配置的列不匹配");
					file.close();
					return -1;
				}
				if (num > column_num)
				{
					mycnf.logerror("数据库返回数据列数超出文件预设列数");
					file.close();
					return -1;
				}
				for (int i = 0;i < num;i++)
				{
					int index = atoi(vSplit[i].c_str());
					if (index > column_num)
					{
						mycnf.logerror("serial列数超出文件预设列数");
						file.close();
						return -1;
					}
					string sTmpRow,sTmpRow2;
					sTmpRow = row[i];
					if(encoding == "utf8") {
						CChineseCodeLib::UTF_8ToGB2312(sTmpRow2, (char*)sTmpRow.c_str(), sTmpRow.length());
					}
					if(encoding == "gbk") {
						sTmpRow2 = sTmpRow;
					}
					file_arr[index] = sTmpRow2;

					file_arr[index].erase(remove_if(file_arr[index].begin(),file_arr[index].end(),bind2nd(equal_to<char>(), '\n')),file_arr[index].end());
					
					//file<< row[i] << "|" ;
				}

				for (int i = 1;i <= column_num;i++)
				{
					file<< file_arr[i] << "|$%^|" ;
				}
				file<< endl;
			}
			mycnf.logerror("数据库返回数据%d行",j);
			// Check for error: can't distinguish "end of results" and
			// error cases in return from fetch_row() otherwise.
			if (conn.errnum()) {
				mycnf.logerror("Error received in fetching a row: %s",conn.error());
				file.close();
				return -1;
			}
			file.close();
			dwStop = GetTickCount() -dwStart;
			mycnf.logerror("耗时： %u ms",dwStop);
			return 0;
		}
		else {
			file.close();
			mycnf.logerror("Failed to get stock item: %s",query.error());
			return -1;
		}
	}
	catch (const mysqlpp::BadQuery& er) {
		// Handle any query errors
		mycnf.logerror("Query error: %s",er.what());
		file.close();
		return -1;
	}
	catch (const mysqlpp::BadConversion& er) {
		// Handle bad conversions; e.g. type mismatch populating 'stock'
		mycnf.logerror("Conversion error: %s\tretrieved data size: %d, actual size: ",er.what(), er.retrieved,er.actual_size);
		file.close();
		return -1;
	}
	catch (const mysqlpp::Exception& er) {
		// Catch-all for any other MySQL++ exceptions
		mycnf.logerror("Mysql Error: %s",er.what());
		file.close();
		return -1;
	}
	dwStop = GetTickCount() -dwStart;
	mycnf.logerror("耗时： %u ms",dwStop);
	return 0;
}



int getMssqlData(const string& db,const string& ip,const string& user,const string& pwd,const string& port,const string& sql,const string& filename,const string& serial,int column_num,const string& encoding){
	
	 DWORD   dwStart,dwStop;   
	 
	dwStart = GetTickCount();   
	fstream file(filename.c_str(),ios::trunc|ios::out);
	string str_file_tmp = "";
	if(!file)
	{
		mycnf.logerror("open file error");
		return -1;
	}

	string file_arr[1024];
	vector<string> vSplit;
	vSplit = strutil::split(serial,",");
	
	
	::CoInitialize(NULL); // 初始化OLE/COM库环境 ，为访问ADO接口做准备
	_ConnectionPtr pConnection;
	HRESULT hr;
	string strSql;
	hr = pConnection.CreateInstance("ADODB.Connection");
	string m_sConnect;
	m_sConnect = "Provider=SQLOLEDB.1;Data Source=";
	m_sConnect.append(ip);
	m_sConnect.append(",");
	m_sConnect.append(port);
	m_sConnect.append(";User ID=");
	m_sConnect.append(user);
	m_sConnect.append(";PWD=");
	m_sConnect.append(pwd);
	m_sConnect.append(";Initial Catalog=");
	m_sConnect.append(db);

	try
	{
		pConnection->CommandTimeout = 3000000;
		pConnection->Open(m_sConnect.c_str(),"","",adConnectUnspecified);//connect to database

	}
	catch(_com_error e)//catch exception
	{
		//cout<<"Connect to database failure!Please try again."<<endl;
		//cout<<"Error information:"<<e.ErrorMessage()<<endl;
		mycnf.logerror("Connect to database failure!Please try again.\r\nError information:%s (%08lx) %s %s",m_sConnect.c_str(),e.Error(),e.ErrorMessage(),e.Description());
		if(pConnection->State)
			pConnection->Close();
		::CoUninitialize();
		file.close();
		return -1;
	}
	_RecordsetPtr pRecordset("ADODB.Recordset");

	if (!sql.empty())
	{
		mycnf.logerror("开始读取[%s]数据",filename.c_str());
		try
		{
			int num;

			long long j=0;
			
			//open table
			pRecordset->Open(sql.c_str(),_variant_t((IDispatch*)pConnection,true),adOpenForwardOnly,adLockReadOnly,adCmdText);

			//mycnf.logerror("返回数据");
			if (pRecordset->RecordCount != 0)
			{
				pRecordset->MoveFirst();
					 
				//mycnf.logerror("MoveFirst");
				//Loop through the Record set
				if (!pRecordset->adoEOF )
				{
					while(!pRecordset->adoEOF)
					{
						j++;
						
						num = pRecordset->GetFields()->Count;

						//mycnf.logerror("%d",j);
						if (vSplit.size() != num)
						{
							mycnf.logerror("数据库返回数据列与<serial>配置的列不匹配");
							if(pConnection->State)
								pConnection->Close();
							::CoUninitialize();
							file.close();
							return -1;
						}
						if (num > column_num)
						{
							mycnf.logerror("数据库返回数据列数超出文件预设列数");
							if(pConnection->State)
								pConnection->Close();
							::CoUninitialize();
							file.close();
							return -1;
						}
						for (int i = 0;i < num;i++)
						{
							int index = atoi(vSplit[i].c_str());
							if (index > column_num)
							{
								mycnf.logerror("serial列数超出文件预设列数");
								if(pConnection->State)
									pConnection->Close();
								::CoUninitialize();
								file.close();
								return -1;
							}//pRs->GetFields()->Item[x]->Name
							//file_arr[index] = (char*) pRecordset->GetFields()->Item[(long)i]->Name;

							int nSize =  pRecordset->GetFields()->Item[(long)i]->ActualSize;   

							if (nSize > 0)
							{
								_variant_t vtFld;
								

								vtFld = pRecordset->GetFields()->Item[(long)i]->Value;
								if (vtFld.vt == VT_R4)
								{
									char str[100];
									ZeroMemory(str,100);
									sprintf(str,"%.2f",(double)vtFld.fltVal);
									file_arr[index].assign(str);
								} else if (vtFld.vt == VT_R8)
								{
									char str[100];
									ZeroMemory(str,100);
									sprintf(str,"%.2f",vtFld.dblVal);
									file_arr[index].assign(str);
								} else if (vtFld.vt == VT_CY)
								{
									char str[100];
									ZeroMemory(str,100);
									sprintf(str,"%.2f",(double)vtFld);
									file_arr[index].assign(str);
								} else if (vtFld.vt == VT_DECIMAL)
								{
									char str[100];
									ZeroMemory(str,100);
									sprintf(str,"%.2f",(double)vtFld);
									file_arr[index].assign(str);
								} else {
									file_arr[index].assign((char*) ((_bstr_t) vtFld));
								}
								file_arr[index].erase(remove_if(file_arr[index].begin(),file_arr[index].end(),bind2nd(equal_to<char>(), '\n')),file_arr[index].end());
							} else {
								file_arr[index].assign("NULL");
							}
							
							//file<< row[i] << "|" ;
						}

						for (int i = 1;i <= column_num;i++)
						{
							str_file_tmp.append(file_arr[i]);
							str_file_tmp.append("|$%^|");
						}
						str_file_tmp.append("\r\n");

						if (str_file_tmp.length()>1024*1024*30)
						{
							file << str_file_tmp;
							str_file_tmp.clear();
						}

						pRecordset->MoveNext();
					}
				}
			} else {
				mycnf.logerror("数据库返回数据0行:%s",sql.c_str());
			}
			if (str_file_tmp.length() > 0)
			{
				file << str_file_tmp;
			}

			pRecordset->Close();
			mycnf.logerror("数据库返回数据%lld行",j);

		}
		catch(_com_error e)
		{
			mycnf.logerror("fetch DataBase Error:%s", e.ErrorMessage());
			if(pConnection->State)
				pConnection->Close();
			::CoUninitialize();
			file.close();
			return -1;
		}
	}

	if(pConnection->State)
		pConnection->Close();

	::CoUninitialize();
	file.close();

	dwStop = GetTickCount() -dwStart;
	mycnf.logerror("耗时： %u ms",dwStop);


	return 0;

}

int getMsTsqlData(const string& db,const string& ip,const string& user,const string& pwd,const string& port,const string& sql,const string& filename,const string& serial,int column_num,const string& encoding,PARMARR pParms){

	DWORD   dwStart,dwStop;   

	dwStart = GetTickCount();   
	fstream file(filename.c_str(),ios::trunc|ios::out);
	string str_file_tmp = "";
	if(!file)
	{
		mycnf.logerror("open file error");
		return -1;
	}

	string file_arr[1024];
	vector<string> vSplit;
	vSplit = strutil::split(serial,",");


	::CoInitialize(NULL); // 初始化OLE/COM库环境 ，为访问ADO接口做准备
	_ConnectionPtr pConnection;
	HRESULT hr;
	string strSql;
	hr = pConnection.CreateInstance("ADODB.Connection");
	string m_sConnect;
	m_sConnect = "Provider=SQLOLEDB.1;Data Source=";
	m_sConnect.append(ip);
	m_sConnect.append(",");
	m_sConnect.append(port);
	m_sConnect.append(";User ID=");
	m_sConnect.append(user);
	m_sConnect.append(";PWD=");
	m_sConnect.append(pwd);
	m_sConnect.append(";Initial Catalog=");
	m_sConnect.append(db);


	try
	{
		pConnection->CommandTimeout = 3000000;
		pConnection->Open(m_sConnect.c_str(),"","",adConnectUnspecified);//connect to database

	}
	catch(_com_error e)//catch exception
	{
		//cout<<"Connect to database failure!Please try again."<<endl;
		//cout<<"Error information:"<<e.ErrorMessage()<<endl;
		mycnf.logerror("Connect to database failure!Please try again.\r\nError information:%s (%08lx) %s",m_sConnect.c_str(),e.Error(),e.ErrorMessage());
		if(pConnection->State)
			pConnection->Close();
		::CoUninitialize();
		file.close();
		return -1;
	}
	_RecordsetPtr pRecordset("ADODB.Recordset");

	_CommandPtr pCommand;//还是智能指针
	pCommand.CreateInstance("ADODB.Command");//实例
	pCommand->ActiveConnection = pConnection;//设置连接，别忘了啊
	pCommand->CommandText = sql.c_str();//存储过程
	pCommand->CommandType = adCmdStoredProc;
	
	for (vector<PARM_INFO*>::iterator parm_iter = pParms.begin(); parm_iter != pParms.end(); ++parm_iter)
	{
		_ParameterPtr pParam;
		DataTypeEnum dteType;
		pParam.CreateInstance("ADODB.Parameter");
		long size = (*parm_iter)->size;
		if((*parm_iter)->type == "varchar") {
			dteType = adVarChar;
		}
		if((*parm_iter)->type == "integer") {
			dteType = adInteger;
		}

		pParam = pCommand->CreateParameter((*parm_iter)->name.c_str(),dteType,(ParameterDirectionEnum)(*parm_iter)->direct,size,(_variant_t)(*parm_iter)->value.c_str());//给参数设置各属性
		pCommand->Parameters->Append(pParam);
	
	}


	if (!sql.empty())
	{
		mycnf.logerror("开始读取[%s]数据",filename.c_str());
		try
		{
			int num;

			long long j=0;


			//open table
			pRecordset = pCommand->Execute(NULL,NULL,adCmdStoredProc); 

			mycnf.logerror("读取完毕[%s]数据",filename.c_str());

			//mycnf.logerror("返回数据");
			
			while(!pRecordset->adoEOF)
			{
				j++;

				num = pRecordset->GetFields()->Count;

				//mycnf.logerror("%d",j);
				if (vSplit.size() != num)
				{
					mycnf.logerror("数据库返回数据列与<serial>配置的列不匹配");
					if(pConnection->State)
						pConnection->Close();
					::CoUninitialize();
					file.close();
					return -1;
				}
				if (num > column_num)
				{
					mycnf.logerror("数据库返回数据列数超出文件预设列数");
					if(pConnection->State)
						pConnection->Close();
					::CoUninitialize();
					file.close();
					return -1;
				}
				for (int i = 0;i < num;i++)
				{
					int index = atoi(vSplit[i].c_str());
					if (index > column_num)
					{
						mycnf.logerror("serial列数超出文件预设列数");
						if(pConnection->State)
							pConnection->Close();
						::CoUninitialize();
						file.close();
						return -1;
					}//pRs->GetFields()->Item[x]->Name
					//file_arr[index] = (char*) pRecordset->GetFields()->Item[(long)i]->Name;

					int nSize =  pRecordset->GetFields()->Item[(long)i]->ActualSize;   

					if (nSize > 0)
					{
						_variant_t vtFld;


						vtFld = pRecordset->GetFields()->Item[(long)i]->Value;
						if (vtFld.vt == VT_R4)
						{
							char str[100];
							ZeroMemory(str,100);
							sprintf(str,"%.2f",(double)vtFld.fltVal);
							file_arr[index].assign(str);
						} else if (vtFld.vt == VT_R8)
						{
							char str[100];
							ZeroMemory(str,100);
							sprintf(str,"%.2f",vtFld.dblVal);
							file_arr[index].assign(str);
						} else if (vtFld.vt == VT_CY)
						{
							char str[100];
							ZeroMemory(str,100);
							sprintf(str,"%.2f",(double)vtFld);
							file_arr[index].assign(str);
						} else if (vtFld.vt == VT_DECIMAL)
						{
							char str[100];
							ZeroMemory(str,100);
							sprintf(str,"%.2f",(double)vtFld);
							file_arr[index].assign(str);
						} else {
							file_arr[index].assign((char*) ((_bstr_t) vtFld));
						}
						file_arr[index].erase(remove_if(file_arr[index].begin(),file_arr[index].end(),bind2nd(equal_to<char>(), '\n')),file_arr[index].end());
					} else {
						file_arr[index].assign("NULL");
					}

					//file<< row[i] << "|" ;
				}

				for (int i = 1;i <= column_num;i++)
				{
					str_file_tmp.append(file_arr[i]);
					str_file_tmp.append("|$%^|");
				}
				str_file_tmp.append("\r\n");

				if (str_file_tmp.length()>1024*1024*30)
				{
					file << str_file_tmp;
					str_file_tmp.clear();
				}

				pRecordset->MoveNext();
			}
			
			if (str_file_tmp.length() > 0)
			{
				file << str_file_tmp;
			}

			pRecordset->Close();
			mycnf.logerror("数据库返回数据%lld行",j);

		}
		catch(_com_error e)
		{
			mycnf.logerror("fetch DataBase Error:%s", e.ErrorMessage());
			if(pConnection->State)
				pConnection->Close();
			::CoUninitialize();
			file.close();
			return -1;
		}
	}

	if(pConnection->State)
		pConnection->Close();

	::CoUninitialize();
	file.close();

	dwStop = GetTickCount() -dwStart;
	mycnf.logerror("耗时： %u ms",dwStop);


	return 0;

}

int getOracleData(const string& db,const string& ip,const string& user,const string& pwd,const string& port,const string& sql,const string& filename,const string& serial,int column_num,const string& encoding) {
	DWORD   dwStart,dwStop;   

	dwStart = GetTickCount();   
	fstream file(filename.c_str(),ios::trunc|ios::out);
	string str_file_tmp = "";
	if(!file)
	{
		mycnf.logerror("open file error");
		return -1;
	}

	string file_arr[1024];
	vector<string> vSplit;
	vSplit = strutil::split(serial,",");


	::CoInitialize(NULL); // 初始化OLE/COM库环境 ，为访问ADO接口做准备
	_ConnectionPtr pConnection;
	HRESULT hr;
	string strSql;
	hr = pConnection.CreateInstance("ADODB.Connection");
	string m_sConnect;
	m_sConnect = "Provider=OraOLEDB.Oracle.1";
	m_sConnect+= ";Persist Security Info=true";
	m_sConnect+= ";User ID=";
	m_sConnect+= user;
	m_sConnect+= ";Password=";
	m_sConnect+= pwd;
	m_sConnect+= ";Data Source=(DESCRIPTION=(ADDRESS=(PROTOCOL=TCP)";
	m_sConnect+= "(HOST=";
	m_sConnect+= ip;
	m_sConnect+= ")(PORT=1521))(CONNECT_DATA=";
	m_sConnect+= "(SERVICE_NAME=";
	m_sConnect+= db;
	m_sConnect+= ")))";

	try
	{
		//pConnection->CommandTimeout = 3000000;
		pConnection->Open(m_sConnect.c_str(),"","",adModeUnknown);//connect to database

	}
	catch(_com_error e)//catch exception
	{
		//cout<<"Connect to database failure!Please try again."<<endl;
		//cout<<"Error information:"<<e.ErrorMessage()<<endl;
		mycnf.logerror("Connect to database failure!Please try again.\r\nError information:%s  %s %s",m_sConnect.c_str(),e.ErrorMessage(),e.Description());
		if(pConnection->State == adStateOpen)
			pConnection->Close();
		::CoUninitialize();
		file.close();
		return -1;
	}
	_RecordsetPtr pRecordset("ADODB.Recordset");

	if (!sql.empty())
	{
		mycnf.logerror("开始读取[%s]数据",filename.c_str());
		try
		{
			int num;

			long long j=0;

			//open table
			pRecordset->Open(sql.c_str(),_variant_t((IDispatch*)pConnection,true),adOpenForwardOnly,adLockReadOnly,adCmdText);

			//mycnf.logerror("返回数据");
			if (pRecordset->RecordCount != 0)
			{
				pRecordset->MoveFirst();

				//mycnf.logerror("MoveFirst");
				//Loop through the Record set
				if (!pRecordset->adoEOF )
				{
					while(!pRecordset->adoEOF)
					{
						j++;

						num = pRecordset->GetFields()->Count;

						//mycnf.logerror("%d",j);
						if (vSplit.size() != num)
						{
							mycnf.logerror("数据库返回数据列与<serial>配置的列不匹配");
							if(pConnection->State)
								pConnection->Close();
							::CoUninitialize();
							file.close();
							return -1;
						}
						if (num > column_num)
						{
							mycnf.logerror("数据库返回数据列数超出文件预设列数");
							if(pConnection->State)
								pConnection->Close();
							::CoUninitialize();
							file.close();
							return -1;
						}
						for (int i = 0;i < num;i++)
						{
							int index = atoi(vSplit[i].c_str());
							if (index > column_num)
							{
								mycnf.logerror("serial列数超出文件预设列数");
								if(pConnection->State)
									pConnection->Close();
								::CoUninitialize();
								file.close();
								return -1;
							}//pRs->GetFields()->Item[x]->Name
							//file_arr[index] = (char*) pRecordset->GetFields()->Item[(long)i]->Name;

							int nSize =  pRecordset->GetFields()->Item[(long)i]->ActualSize;   

							if (nSize > 0)
							{
								_variant_t vtFld;


								vtFld = pRecordset->GetFields()->Item[(long)i]->Value;
								if (vtFld.vt == VT_R4)
								{
									char str[100];
									ZeroMemory(str,100);
									sprintf(str,"%.2f",(double)vtFld.fltVal);
									file_arr[index].assign(str);
								} else if (vtFld.vt == VT_R8)
								{
									char str[100];
									ZeroMemory(str,100);
									sprintf(str,"%.2f",vtFld.dblVal);
									file_arr[index].assign(str);
								} else if (vtFld.vt == VT_CY)
								{
									char str[100];
									ZeroMemory(str,100);
									sprintf(str,"%.2f",(double)vtFld);
									file_arr[index].assign(str);
								} else if (vtFld.vt == VT_DECIMAL)
								{
									char str[100];
									ZeroMemory(str,100);
									sprintf(str,"%.2f",(double)vtFld);
									file_arr[index].assign(str);
								} else {
									file_arr[index].assign((char*) ((_bstr_t) vtFld));
								}
								file_arr[index].erase(remove_if(file_arr[index].begin(),file_arr[index].end(),bind2nd(equal_to<char>(), '\n')),file_arr[index].end());
							} else {
								file_arr[index].assign("NULL");
							}

							//file<< row[i] << "|" ;
						}

						for (int i = 1;i <= column_num;i++)
						{
							str_file_tmp.append(file_arr[i]);
							str_file_tmp.append("|$%^|");
						}
						str_file_tmp.append("\r\n");

						if (str_file_tmp.length()>1024*1024*30)
						{
							file << str_file_tmp;
							str_file_tmp.clear();
						}

						pRecordset->MoveNext();
					}
				}
			}
			if (str_file_tmp.length() > 0)
			{
				file << str_file_tmp;
			}

			pRecordset->Close();
			mycnf.logerror("数据库返回数据%lld行",j);

		}
		catch(_com_error e)
		{
			mycnf.logerror("fetch DataBase Error:%s", e.ErrorMessage());
			if(pConnection->State)
				pConnection->Close();
			::CoUninitialize();
			file.close();
			return -1;
		}
	}

	if(pConnection->State)
		pConnection->Close();

	::CoUninitialize();
	file.close();

	dwStop = GetTickCount() -dwStart;
	mycnf.logerror("耗时： %u ms",dwStop);


	return 0;


}


