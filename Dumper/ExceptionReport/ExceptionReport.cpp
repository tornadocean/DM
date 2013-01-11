#include "StdAfx.h"
#include "ExceptionReport.h"
#include "Tlhelp32.h"

#pragma comment(lib, "Toolhelp.lib")

TCHAR CExceptionReport::m_szLogFileName[MAX_PATH];
TCHAR CExceptionReport::m_szDumpFileName[MAX_PATH];
//LPTOP_LEVEL_EXCEPTION_FILTER CExceptionReport::m_previousFilter;
HANDLE CExceptionReport::m_hReportFile;
HANDLE CExceptionReport::m_hDumpFile;
HANDLE CExceptionReport::m_hProcess;

//CExceptionReport g_ExceptionReport;

CExceptionReport::CExceptionReport(void)
{
	AddVectoredExceptionHandler(0, WheatyUnhandledExceptionFilter);
	m_hProcess = GetCurrentProcess();
}

CExceptionReport::~CExceptionReport(void)
{
	RemoveVectoredExceptionHandler(WheatyUnhandledExceptionFilter);
}

//===========================================================
// Entry point where control comes on an unhandled exception
//===========================================================
LONG WINAPI CExceptionReport::WheatyUnhandledExceptionFilter(
	PEXCEPTION_POINTERS pExceptionInfo)
{
	TCHAR module_folder_name[MAX_PATH];
	GetModuleFileName(0, module_folder_name, MAX_PATH);
	TCHAR* pos = _tcsrchr(module_folder_name, '\\');
	if (!pos)
		return 0;
	pos[0] = '\0';
	++pos;

	TCHAR crash_folder_path[MAX_PATH];
	swprintf(crash_folder_path, _T("%s\\%s"), module_folder_name, CrashFolder);
	if (!CreateDirectory(crash_folder_path, NULL))
	{
		if (GetLastError() != ERROR_ALREADY_EXISTS)
			return 0;
	}

	SYSTEMTIME systime;
	GetLocalTime(&systime);
	DWORD dwTime = GetTickCount();

	swprintf(m_szLogFileName, _T("%s\\%s_[%u-%u_%u-%u-%u].txt"),
		crash_folder_path, pos, systime.wDay, systime.wMonth, systime.wHour, systime.wMinute, systime.wSecond);

	m_hReportFile = CreateFile(m_szLogFileName,
		GENERIC_WRITE,
		0,
		0,
		OPEN_ALWAYS,
		FILE_FLAG_WRITE_THROUGH,
		0);

	if (m_hReportFile)
	{
		SetFilePointer(m_hReportFile, 0, 0, FILE_END);
		GenerateExceptionReport(pExceptionInfo);
		CloseHandle(m_hReportFile);
		m_hReportFile = 0;
	}

		return EXCEPTION_EXECUTE_HANDLER/*EXCEPTION_CONTINUE_SEARCH*/;
}

//===========================================================================
// Open the report file, and write the desired information to it.  Called by
// WheatyUnhandledExceptionFilter
//===========================================================================
void CExceptionReport::GenerateExceptionReport(
	PEXCEPTION_POINTERS pExceptionInfo)
{
	SYSTEMTIME systime;
	GetLocalTime(&systime);

	TCHAR buf[1024] = {0};
	int nSize = 0;
	// Start out with a banner
	//nSize = swprintf_s(buf, 1024, _T("Date %u:%u:%u. Time %u:%u \r\n"), 
	//	systime.wDay, 
	//	systime.wMonth,
	//	systime.wYear, 
	//	systime.wHour, 
	//	systime.wMinute);
	_Log(_T("Date %u:%u:%u. Time %u:%u \r\n"), 
		systime.wDay, 
		systime.wMonth,
		systime.wYear, 
		systime.wHour, 
		systime.wMinute);
	
	PEXCEPTION_RECORD pExceptionRecord = pExceptionInfo->ExceptionRecord;


	//PrintSystemInfo();
	// First print information about the type of fault
	_Log(_T("\r\n//=====================================================\r\n"));
	_Log(_T("Exception code: %08X %s\r\n"),
		pExceptionRecord->ExceptionCode,
		GetExceptionString(pExceptionRecord->ExceptionCode));

	// Now print information about where the fault occured
	TCHAR szFaultingModule[MAX_PATH];
	DWORD section;
	DWORD_PTR offset;
	GetLogicalAddress(pExceptionRecord->ExceptionAddress,
		szFaultingModule,
		sizeof(szFaultingModule),
		section, offset);

#ifdef _M_IX86
	_Log(_T("Fault address:  %08X %02X:%08X %s\r\n"),
		pExceptionRecord->ExceptionAddress,
		section, offset, szFaultingModule);
#endif
#ifdef _M_X64
	_Log(_T("Fault address:  %016I64X %02X:%016I64X %s\r\n"),
		pExceptionRecord->ExceptionAddress,
		section, offset, szFaultingModule);
#endif

	PCONTEXT pCtx = pExceptionInfo->ContextRecord;

	// Show the registers
#ifdef _M_IX86                                          // X86 Only!
	_Log(_T("\r\nRegisters:\r\n"));

	_Log(_T("EAX:%08X\r\nEBX:%08X\r\nECX:%08X\r\nEDX:%08X\r\nESI:%08X\r\nEDI:%08X\r\n")
		, pCtx->Eax, pCtx->Ebx, pCtx->Ecx, pCtx->Edx,
		pCtx->Esi, pCtx->Edi);

	_Log(_T("CS:EIP:%04X:%08X\r\n"), pCtx->SegCs, pCtx->Eip);
	_Log(_T("SS:ESP:%04X:%08X  EBP:%08X\r\n"),
		pCtx->SegSs, pCtx->Esp, pCtx->Ebp);
	_Log(_T("DS:%04X  ES:%04X  FS:%04X  GS:%04X\r\n"),
		pCtx->SegDs, pCtx->SegEs, pCtx->SegFs, pCtx->SegGs);
	_Log(_T("Flags:%08X\r\n"), pCtx->EFlags);
#endif

#ifdef _M_X64
	_Log(_T("\r\nRegisters:\r\n"));
	_Log(_T("RAX:%016I64X\r\nRBX:%016I64X\r\nRCX:%016I64X\r\nRDX:%016I64X\r\nRSI:%016I64X\r\nRDI:%016I64X\r\n")
		_T("R8: %016I64X\r\nR9: %016I64X\r\nR10:%016I64X\r\nR11:%016I64X\r\nR12:%016I64X\r\nR13:%016I64X\r\nR14:%016I64X\r\nR15:%016I64X\r\n")
		, pCtx->Rax, pCtx->Rbx, pCtx->Rcx, pCtx->Rdx,
		pCtx->Rsi, pCtx->Rdi, pCtx->R9, pCtx->R10, pCtx->R11, pCtx->R12, pCtx->R13, pCtx->R14, pCtx->R15);
	_Log(_T("CS:RIP:%04X:%016I64X\r\n"), pCtx->SegCs, pCtx->Rip);
	_Log(_T("SS:RSP:%04X:%016X  RBP:%08X\r\n"),
		pCtx->SegSs, pCtx->Rsp, pCtx->Rbp);
	_Log(_T("DS:%04X  ES:%04X  FS:%04X  GS:%04X\r\n"),
		pCtx->SegDs, pCtx->SegEs, pCtx->SegFs, pCtx->SegGs);
	_Log(_T("Flags:%08X\r\n"), pCtx->EFlags);
#endif

	//SymSetOptions(SYMOPT_DEFERRED_LOADS);

	//// Initialize DbgHelp
	//if (!SymInitialize(GetCurrentProcess(), 0, TRUE))
	//{
	//	_Log(_T("\n\rCRITICAL ERROR.\n\r Couldn't initialize the symbol handler for process.\n\rError [%s].\n\r\n\r"),
	//		//ErrorMessage(GetLastError()));
	//}

	CONTEXT trashableContext = *pCtx;

	//WriteStackDetails(&trashableContext, false, NULL);

	_Log(_T("========================\r\n"));
	_Log(_T("Last Exception Call Stack:\r\n"));

	#define MAX_FRAME_COUNT 10
	CallSnapshot lpFrames[MAX_FRAME_COUNT];
	DWORD dwRet = KLibGetLastExcpCallStack(MAX_FRAME_COUNT, lpFrames);
	for (int i=0; i<MAX_FRAME_COUNT; i++)
	{
		_Log(_T("Stack address:  %08X \r\n"), lpFrames[i].dwReturnAddr);
	}
//	_Log(_T("========================\r\n"));
//	_Log(_T(" Call Stack:\r\n"));
//
//GetCallStackSnapshot (MAX_FRAME_COUNT, lpFrames, 0, 0);
//for (int i=0; i<MAX_FRAME_COUNT; i++)
//{
//	_Log(_T("Stack address:  %08X \r\n"), lpFrames[i].dwReturnAddr);
//}

	//DumpKCallProfile(0);
	//BOOL bStart = OEMKDIoControl(KD_IOCTL_DMPGEN_START, NULL, 4096);
	//BOOL bEnd = OEMKDIoControl(KD_IOCTL_DMPGEN_END, NULL, 4096);
	//printTracesForAllThreads();
	//HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPHEAPLIST, GetCurrentProcessId());
	//HEAPLIST32 hl;
	//hl.dwSize = sizeof(HEAPLIST32);

	//if ( hSnap == INVALID_HANDLE_VALUE )
	//{
	//	printf ("CreateToolhelp32Snapshot failed (%d)\n", GetLastError());
	//}
	//if( Heap32ListFirst( hSnap, &hl ) )
	//{
	//	do
	//	{
	//		HEAPENTRY32 he;
	//		ZeroMemory(&he, sizeof(HEAPENTRY32));
	//		he.dwSize = sizeof(HEAPENTRY32);

	//		if( Heap32First( hSnap, &he, GetCurrentProcessId(), hl.th32HeapID ) )
	//		{
	//			printf( "\nHeap ID: %d\n", hl.th32HeapID );
	//			do
	//			{
	//				//printf( "Block size: %d\n", he.dwBlockSize );
	//				//printf( "Address: %d\n", he.dwAddress );

	//				/*unsigned char buf[4096] = {};
	//				DWORD dwRead = 0;
	//				Toolhelp32ReadProcessMemory(GetCurrentProcessId(), &he.dwAddress, buf,
	//					he.dwBlockSize, &dwRead);

	//				for (int i=0; i<dwRead; i++)
	//				{
	//					printf("%02X ", buf[i]);
	//					if ((i+1) % 16 == 0 )
	//					{
	//						printf("\r\n");
	//					}
	//				}
	//				printf("\r\n");*/

	//				he.dwSize = sizeof(HEAPENTRY32);
	//			} while( Heap32Next(hSnap, &he) );
	//		}
	//		hl.dwSize = sizeof(HEAPLIST32);
	//	} while (Heap32ListNext( hSnap, &hl ));
	//}
	//else printf ("Cannot list first heap (%d)\n", GetLastError());

	//    #ifdef _M_IX86                                          // X86 Only!

	_Log(_T("========================\r\n"));
	_Log(_T("Local Variables And Parameters\r\n"));

	trashableContext = *pCtx;
	//WriteStackDetails(&trashableContext, true, NULL);

	_Log(_T("========================\r\n"));
	_Log(_T("Global Variables\r\n"));

	//SymEnumSymbols(GetCurrentProcess(),
	//	(UINT_PTR)GetModuleHandle(szFaultingModule),
	//	0, EnumerateSymbolsCallback, 0);
	//  #endif                                                  // X86 Only!

	//SymCleanup(GetCurrentProcess());

	_Log(_T("\r\n"));
}

//======================================================================
// Given an exception code, returns a pointer to a static string with a
// description of the exception
//======================================================================
LPTSTR CExceptionReport::GetExceptionString(DWORD dwCode)
{
#define EXCEPTION(x) case EXCEPTION_##x: return _T(#x);

	switch (dwCode)
	{
		EXCEPTION(ACCESS_VIOLATION)
			EXCEPTION(DATATYPE_MISALIGNMENT)
			EXCEPTION(BREAKPOINT)
			EXCEPTION(SINGLE_STEP)
			EXCEPTION(ARRAY_BOUNDS_EXCEEDED)
			EXCEPTION(FLT_DENORMAL_OPERAND)
			EXCEPTION(FLT_DIVIDE_BY_ZERO)
			EXCEPTION(FLT_INEXACT_RESULT)
			EXCEPTION(FLT_INVALID_OPERATION)
			EXCEPTION(FLT_OVERFLOW)
			EXCEPTION(FLT_STACK_CHECK)
			EXCEPTION(FLT_UNDERFLOW)
			EXCEPTION(INT_DIVIDE_BY_ZERO)
			EXCEPTION(INT_OVERFLOW)
			EXCEPTION(PRIV_INSTRUCTION)
			EXCEPTION(IN_PAGE_ERROR)
			EXCEPTION(ILLEGAL_INSTRUCTION)
			EXCEPTION(NONCONTINUABLE_EXCEPTION)
			EXCEPTION(STACK_OVERFLOW)
			EXCEPTION(INVALID_DISPOSITION)
			EXCEPTION(GUARD_PAGE)
			EXCEPTION(INVALID_HANDLE)
	}

	// If not one of the "known" exceptions, try to get the string
	// from NTDLL.DLL's message table.

	static TCHAR szBuffer[512] = { 0 };

	FormatMessage(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_HMODULE,
		GetModuleHandle(_T("Coredll.dll")),
		dwCode, 0, szBuffer, sizeof(szBuffer), 0);

	return szBuffer;
}

//=============================================================================
// Given a linear address, locates the module, section, and offset containing
// that address.
//
// Note: the szModule paramater buffer is an output buffer of length specified
// by the len parameter (in characters!)
//=============================================================================
BOOL CExceptionReport::GetLogicalAddress(
	PVOID addr, PTSTR szModule, DWORD len, DWORD& section, DWORD_PTR& offset)
{
	MEMORY_BASIC_INFORMATION mbi;

	if (!VirtualQuery(addr, &mbi, sizeof(mbi)))
		return FALSE;

	DWORD_PTR hMod = (DWORD_PTR)mbi.AllocationBase;

	if (!GetModuleFileName((HMODULE)hMod, szModule, len))
		return FALSE;

	// Point to the DOS header in memory
	PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)hMod;

	// From the DOS header, find the NT (PE) header
	PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)(hMod + DWORD_PTR(pDosHdr->e_lfanew));

	PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNtHdr);

	DWORD_PTR rva = (DWORD_PTR)addr - hMod;                         // RVA is offset from module load address

	// Iterate through the section table, looking for the one that encompasses
	// the linear address.
	for (unsigned i = 0;
		i < pNtHdr->FileHeader.NumberOfSections;
		i++, pSection++)
	{
		DWORD_PTR sectionStart = pSection->VirtualAddress;
		DWORD_PTR sectionEnd = sectionStart
			+ (DWORD_PTR)( max(pSection->SizeOfRawData, pSection->Misc.VirtualSize));

		// Is the address in this section???
		if ((rva >= sectionStart) && (rva <= sectionEnd))
		{
			// Yes, address is in the section.  Calculate section and offset,
			// and store in the "section" & "offset" params, which were
			// passed by reference.
			section = i+1;
			offset = rva - sectionStart;
			return TRUE;
		}
	}

	return FALSE;                                           // Should never get here!
}

//============================================================================
// Helper function that writes to the report file, and allows the user to use
// printf style formating
//============================================================================
void __stdcall CExceptionReport::_Log(const TCHAR * format, ...)
{
	TCHAR buf[1024] = {0};
	DWORD cbWritten = 0;

	va_list argptr;
	int cnt;

	va_start(argptr, format);
	cnt = vswprintf_s(buf, 1024, format, argptr);
	va_end(argptr);
	
	wprintf(buf);
	WriteFile(m_hReportFile, buf, cnt * sizeof(TCHAR), &cbWritten, 0);
}