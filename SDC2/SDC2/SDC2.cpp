// SDC2.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "SDC2.h"
#include <ShellAPI.h>
#include <windows.h>   
#include <tlhelp32.h>    

#define MAX_LOADSTRING 100

// 全局变量:

// 此代码模块中包含的函数的前向声明:

void Getprocess(LPCWSTR PsName);   // 获取对应进程   
void OnStop(HANDLE m_hPro);   // 阻杀对应进程    


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
	//ShellExecute(NULL,_T("open"),_T("calc.exe"),NULL,NULL,SW_SHOWNORMAL); 
	Getprocess( _T("HModemSvr.exe") );
	HINSTANCE hInst = 0;
	hInst = ShellExecute(NULL,_T("open"), _T(".\\HModemSvr.exe"), NULL, NULL, SW_SHOWNORMAL); 
	if (ERROR_FILE_NOT_FOUND == (long)hInst)
	{
		MessageBox(NULL, _T("Can not find HModemSvr.exe"), _T("Error"), MB_OK);    
	}

	hInst = ShellExecute(NULL,_T("open"), _T(".\\SDC625.exe"), NULL, NULL, SW_SHOWMAXIMIZED); 
	if (ERROR_FILE_NOT_FOUND == (long)hInst)
	{
		MessageBox(NULL, _T("Can not find SDC625.exe"), _T("Error"), MB_OK);    
	}

	return 0;
}

void Getprocess(LPCWSTR PsName)   
{   
	HANDLE hProcessSnap = NULL;   
	PROCESSENTRY32 pe32 = {0};   
	//获得句柄   
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);   
	if(hProcessSnap == (HANDLE)-1)   
	{   
		MessageBox(NULL,_T("CreateToolhelp32Snapshot()函数失败:%d"),_T("title"),MB_OK);    
		/*AfxMessageBox("CreateToolhelp32Snapshot()函数失败:%d",GetLastError());*/  
		/*  return;*/  
	}   
	pe32.dwSize = sizeof(PROCESSENTRY32);   
	//列举所有进程名称   
	if (Process32First(hProcessSnap, &pe32))   
	{   
		do  
		{      

			if(_tcscmp(pe32.szExeFile,PsName)==0)   
			{   
				//利用数组ID指定的获得存在的一个进程的句柄    

				HANDLE       ProcessHandle;    

				ProcessHandle=OpenProcess(PROCESS_TERMINATE,FALSE,pe32.th32ProcessID);    

				OnStop(ProcessHandle);   

			}   
			else  
			{   
				/*MessageBox(NULL,_T("失败"),_T("title"),MB_OK); */  
			}   

			//cout<<pe32.th32ProcessID<<endl;   
			//pe32.dwSize = sizeof(PROCESSENTRY32);   

		}   
		while (Process32Next(hProcessSnap, &pe32));//直到列举完毕   
	}   
	else  
	{   
		MessageBox(NULL,_T("Process32Firstt()函数执行失败:%d"),_T("title"),MB_OK);    
		/*AfxMessageBox("Process32Firstt()函数执行失败:%d",GetLastError());*/  
	}   
	//关闭句柄   
	CloseHandle (hProcessSnap);      

}  

void OnStop(HANDLE m_hPro)    
{   
	//判断进程句柄是否合法   
	if(m_hPro)   
	{   
		//根据句柄，终止刚才打开的记事本程序   
		if(!TerminateProcess(m_hPro,0))   
		{   
			//终止出现错误，显示错误信息   
			LPVOID lpMsgBuf;   
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |    
				FORMAT_MESSAGE_FROM_SYSTEM |    
				FORMAT_MESSAGE_IGNORE_INSERTS,   
				NULL,   
				GetLastError(),   
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),    
				(LPTSTR) &lpMsgBuf,   
				0,   
				NULL );   
			MessageBox(NULL,(LPCTSTR)lpMsgBuf,_T("title"),MB_OK);    
			LocalFree( lpMsgBuf );   
		}   
		else  
		{   
			/*AfxMessageBox("销毁进程成功");*/  
			//MessageBox(NULL,_T("销毁进程成功"),_T("title"),MB_OK);    
		}   
		m_hPro=NULL;   
	}   
	else  
	{   
		/*AfxMessageBox("进程句柄为空");*/  
		//MessageBox(NULL,_T("进程句柄为空"),_T("title"),MB_OK);    
	}   
}    