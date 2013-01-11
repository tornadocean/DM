#pragma once
#include <Pkfuncs.h>
#include <exception>

#define CrashFolder _T("Crashes")
//typedef   BOOL(*CaptureDumpFileOnDevice)(DWORD dwProcessId,DWORD dwThreadId, LPCWSTR pwzExtraFilesPath) ;

class CExceptionReport
{
public:
	CExceptionReport(void);
	~CExceptionReport(void);

	//static CaptureDumpFileOnDevice   m_pCaptureDumpFileOnDevice;

	static LONG WINAPI WheatyUnhandledExceptionFilter(
		PEXCEPTION_POINTERS pExceptionInfo);

	// Variables used by the class
	static TCHAR m_szLogFileName[MAX_PATH];
	static TCHAR m_szDumpFileName[MAX_PATH];
	//static LPTOP_LEVEL_EXCEPTION_FILTER m_previousFilter;
	static HANDLE m_hReportFile;
	static HANDLE m_hDumpFile;
	static HANDLE m_hProcess;
private:
	static void GenerateExceptionReport(PEXCEPTION_POINTERS pExceptionInfo);
	// Helper functions
	static LPTSTR GetExceptionString(DWORD dwCode);
	static BOOL GetLogicalAddress(PVOID addr, PTSTR szModule, DWORD len,
		DWORD& section, DWORD_PTR& offset);

	//static void WriteStackDetails(PCONTEXT pContext, bool bWriteVariables, HANDLE pThreadHandle);

	 static void __stdcall _Log(const TCHAR * format, ...);
	//static BOOL CALLBACK EnumerateSymbolsCallback(PSYMBOL_INFO, ULONG, PVOID);
};
//extern CExceptionReport g_ExceptionReport; 