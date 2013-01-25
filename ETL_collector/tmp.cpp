g_extension_exclude = mycnf.m_fileFilterList;
for (vector<OPT_INFO*>::iterator opt_iter = mycnf.m_opt.begin(); opt_iter != mycnf.m_opt.end(); ++opt_iter)
{
	if((*opt_iter)->type == "remove") {

		g_iDiskFree = (*opt_iter)->freediskspace;
		dFreeMegebyte = ((double)DiskFree((*opt_iter)->driver))/1024/1024;
		if (g_iDiskFree != -1 && g_iDiskFree < dFreeMegebyte)
		{
			myperror("磁盘[%s]有足够的空间[%.2f]，不需要清理文件",(*opt_iter)->driver.c_str(),dFreeMegebyte);

			//删除空目录
			if (mycnf.m_iDelEmptyDir)
			{
				for (vector<MY_FILE_INFO*>::iterator file_iter = (*opt_iter)->filelist->begin(); file_iter != (*opt_iter)->filelist->end(); ++file_iter)
				{
					//cout<<(*file_iter)->path<<endl;  
					g_pFileInfo = *file_iter;
					DeleteDirectory((*file_iter)->path,0);    
				}
			}
			continue;
		}


		g_optfile_list.clear();
		for (vector<MY_FILE_INFO*>::iterator file_iter = (*opt_iter)->filelist->begin(); file_iter != (*opt_iter)->filelist->end(); ++file_iter)
		{
			//cout<<(*file_iter)->path<<endl;  
			g_pFileInfo = *file_iter;
			filesearch((*file_iter)->path,0);    
		}


		for (vector<MY_OPTFILE_INFO*>::iterator exe_iter = g_optfile_list.begin(); exe_iter != g_optfile_list.end(); ++exe_iter){
			if (mycnf.m_iConfirm)
			{
				printf("你确认删除：%s  (y/n)[n]: ",(*exe_iter)->full_path.c_str());
				//scanf("%c",&confirm); 
				confirm = getchar();
				fflush(stdin);   
				//getchar();
				if (confirm=='Y' || confirm=='y')
				{
					//cout<<"删除了"<<(*exe_iter)->full_path.c_str()<<endl;
					myperror("删除了[%s]",(*exe_iter)->full_path.c_str());
					_unlink((*exe_iter)->full_path.c_str());
				} else {
					myperror("删除[%s]被用户取消",(*exe_iter)->full_path.c_str());
				}

			} else {
				//cout<<"删除了"<<(*exe_iter)->full_path.c_str()<<endl;
				myperror("删除了[%s]",(*exe_iter)->full_path.c_str());
				_unlink((*exe_iter)->full_path.c_str());
			}
		}

		//删除空目录
		if (mycnf.m_iDelEmptyDir)
		{
			for (vector<MY_FILE_INFO*>::iterator file_iter = (*opt_iter)->filelist->begin(); file_iter != (*opt_iter)->filelist->end(); ++file_iter)
			{
				//cout<<(*file_iter)->path<<endl;  
				g_pFileInfo = *file_iter;
				DeleteDirectory((*file_iter)->path,0);    
			}
		}



		// 			string::size_type loc = (*opt_iter)->full_path.find("\\");
		// 			if( loc != string::npos ) {
		// 				string mount = (*exe_iter)->full_path.substr(0, loc);
		// 				cout<<mount<<endl;
		// 				unsigned long long gsize = DiskFree(mount)/1024/1024;
		// 				printf("disk free:%llu\n",gsize);
		// 				(*opt_iter)->freediskspace
		// 			}


	}  else if ((*opt_iter)->type == "compress") {
		g_optfile_list.clear();
		string tmpPath;
		string strTarFile;
		char tAll[1024];
		memset(tAll,'\0',1024);
		time_t t = time(0);  
		memset(tAll,0,1024);
		strftime(tAll,sizeof(tAll),"%Y_%m_%d",localtime(&t));


		if((*opt_iter)->filelist->size() > 1) {
			myperror("压缩操作只支持一个<files>标签");
			goto go_end;
		}
		for (vector<MY_FILE_INFO*>::iterator file_iter = (*opt_iter)->filelist->begin(); file_iter != (*opt_iter)->filelist->end(); ++file_iter)
		{
			//cout<<(*file_iter)->path<<endl;  
			g_pFileInfo = *file_iter;
			tmpPath = (*file_iter)->path;
			filesearch((*file_iter)->path,0);    
		}
		strTarFile = replace_all((*opt_iter)->archive_name,"$date",string(tAll));


		NCompress::NLzma2::CEncoder enc; 
		CInFileStream InStm; 
		COutFileStream OutStm; 


		// 			if(!InStm.Open(strTarFile.c_str())) {
		// 				myperror("生成压缩包：%s 失败,请检查磁盘空间及文件路径",strTarFile.c_str());
		// 				goto go_end;	
		// 			}

		OutStm.Create(strTarFile.c_str(), true); 
		enc.SetCoderProperties(NULL,NULL,0); //使用默认压缩参数 
		enc.WriteCoderProperties(&OutStm); //向LZMA文件写入压缩参数 
		UInt64 size; 




		size_t len = tmpPath.length();

		len++;
		for (vector<MY_OPTFILE_INFO*>::iterator exe_iter = g_optfile_list.begin(); exe_iter != g_optfile_list.end(); ++exe_iter){
			string stmp = (*exe_iter)->full_path.substr(len);


			if(!InStm.Open(strTarFile.c_str())) {
				//myperror("生成压缩包：%s 失败,请检查磁盘空间及文件路径",strTarFile.c_str());
				myperror("文件[%s]不存在",(*exe_iter)->full_path.c_str());
				//goto go_end;	
			}
			InStm.GetSize(&size); 


			// 向LZMA文件写入源大小，注意字节顺序 
			for(int i=0; i<8; i++) 
			{ 
				BYTE b = BYTE(size >> (8*i)); 
				OutStm.Write(&b, sizeof(b), NULL); 
			} 

			// 压缩 
			enc.Code(&InStm, &OutStm, NULL, NULL, NULL); 






			if(ZipAdd(hz, _T(stmp.c_str()),  (void *)_T((*exe_iter)->full_path.c_str()), 0, ZIP_FILENAME) == ZR_OK) {
				myperror("文件[%s]压缩成功",(*exe_iter)->full_path.c_str());
			} else {
				myperror("文件[%s]压缩失败",(*exe_iter)->full_path.c_str());
			}

		}



		// 			HZIP hz = CreateZip((void *)_T(strTarFile.c_str()), 0, ZIP_FILENAME);
		// 
		// 			if (hz)
		// 			{
		// 
		// 				size_t len = tmpPath.length();
		// 				
		// 				len++;
		// 				for (vector<MY_OPTFILE_INFO*>::iterator exe_iter = g_optfile_list.begin(); exe_iter != g_optfile_list.end(); ++exe_iter){
		// 					string stmp = (*exe_iter)->full_path.substr(len);
		// 
		// 
		// 					if(ZipAdd(hz, _T(stmp.c_str()),  (void *)_T((*exe_iter)->full_path.c_str()), 0, ZIP_FILENAME) == ZR_OK) {
		// 						myperror("文件[%s]压缩成功",(*exe_iter)->full_path.c_str());
		// 					} else {
		// 						myperror("文件[%s]压缩失败",(*exe_iter)->full_path.c_str());
		// 					}
		// 
		// 				}
		// 
		// 				CloseZip(hz);
		// 
		// 				
		// 			} else {
		// 				myperror("生成压缩包：%s 失败,请检查磁盘空间及文件路径",(*opt_iter)->archive_name.c_str());
		// 				goto go_end;
		// 			}
	} else if ((*opt_iter)->type == "upload") {
		g_optfile_list.clear();
		string new_file_name;
		for (vector<MY_FILE_INFO*>::iterator file_iter = (*opt_iter)->filelist->begin(); file_iter != (*opt_iter)->filelist->end(); ++file_iter)
		{
			//cout<<(*file_iter)->path<<endl;  
			g_pFileInfo = *file_iter;
			filesearch((*file_iter)->path,0);    
		}
		for (vector<MY_OPTFILE_INFO*>::iterator exe_iter = g_optfile_list.begin(); exe_iter != g_optfile_list.end(); ++exe_iter){

			for (int k =0;k<(*opt_iter)->try_again;k++)
			{
				string sPreHash = "";
				iRet = FtpUpdataFiles((*exe_iter)->full_path,(*exe_iter)->file_name,(*opt_iter)->upload_ip,(*opt_iter)->upload_user,(*opt_iter)->upload_pwd,(*opt_iter)->upload_path,(*exe_iter)->file_size,sPreHash);
				if (iRet == -1)
				{
					myperror("文件[%s]上传失败",(*exe_iter)->full_path.c_str());
				} else {

					if ((*opt_iter)->suffix.size() != 0)
					{
						new_file_name = (*exe_iter)->full_path + (*opt_iter)->suffix;
						iRet = rename((*exe_iter)->full_path.c_str(),new_file_name.c_str());
						if (iRet == -1)
						{
							myperror("文件[%s]上传后改名失败",(*exe_iter)->full_path.c_str());
						} else {
							myperror("文件[%s]上传并改名成功",(*exe_iter)->full_path.c_str());
						}
					} else {
						myperror("文件[%s]上传成功",(*exe_iter)->full_path.c_str());
					}
					break;
				}
			}
		}
	} else if ((*opt_iter)->type == "rename") {
		g_optfile_list.clear();
		string new_file_name;
		for (vector<MY_FILE_INFO*>::iterator file_iter = (*opt_iter)->filelist->begin(); file_iter != (*opt_iter)->filelist->end(); ++file_iter)
		{
			//cout<<(*file_iter)->path<<endl;  
			g_pFileInfo = *file_iter;
			filesearch((*file_iter)->path,0);    
		}
		for (vector<MY_OPTFILE_INFO*>::iterator exe_iter = g_optfile_list.begin(); exe_iter != g_optfile_list.end(); ++exe_iter){

			new_file_name = (*exe_iter)->full_path + (*opt_iter)->suffix;
			iRet = rename((*exe_iter)->full_path.c_str(),new_file_name.c_str());
			if (iRet == -1)
			{
				myperror("文件[%s]改名失败",(*exe_iter)->full_path.c_str());
			} else {
				myperror("文件[%s]改名成功",(*exe_iter)->full_path.c_str());
			}
		}
	} else if ((*opt_iter)->type == "stop" || (*opt_iter)->type == "start") {
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		ZeroMemory( &pi, sizeof(pi) );

		string strCmd;


		if((*opt_iter)->cmdType == "exe") {
			//ShellExecute(NULL,_T("open"), _T((*opt_iter)->cmdTargetName.c_str()), _T(""),NULL,SW_SHOWNORMAL); 
			strCmd = (*opt_iter)->cmdTargetName.c_str();


		} else if((*opt_iter)->cmdType == "service") {

			if ((*opt_iter)->type == "stop")
			{
				strCmd = "net stop ";
				strCmd.append((*opt_iter)->cmdTargetName);

			} else {
				strCmd = "net start ";
				strCmd.append((*opt_iter)->cmdTargetName);
			}
			//ShellExecute(NULL,_T("open"), _T("net"), _T(strCmd.c_str()),NULL,SW_SHOWNORMAL); 


			// Start the child process. 



		}
		_bstr_t bstr(strCmd.c_str());
		LPTSTR lpzCmd = (LPTSTR)bstr;
		if( !CreateProcess( NULL,   // No module name (use command line)
			lpzCmd,        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi )           // Pointer to PROCESS_INFORMATION structure
			) 
		{
			myperror( "CreateProcess failed (%d).\n", GetLastError() );
			goto go_end;
		}

		// Wait until child process exits.
		WaitForSingleObject( pi.hProcess, INFINITE );

		// Close process and thread handles. 
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );

	}
}