#pragma once

// NT-only version
namespace	nForceLibrary
{
	// Forced structure
#pragma pack(push, 1)
	typedef	struct 
	{		
		BYTE	push1_;	// 0x68 = push [xxxxxxxx]
		DWORD	push1;	// addr
		BYTE	push2_;	// 0x68 = push [xxxxxxxx]
		DWORD	push2;	// addr
		BYTE	pushopc;	// 0x68 = push [xxxxxxxx]
		DWORD	pushaddr;	// addr

		

		BYTE	callopc;	// 0xE8 = call [xxxxxxxx]
		DWORD	calladdr;	// addr
		BYTE	retopc;		// 0xC2 = ret [xxxx]
		WORD	retvalue;	// addr
		TCHAR	lib_path[MAX_PATH];
	} forceStruct_t;
#pragma pack(pop)

	bool	ForceLibrary(const TCHAR * libName, PROCESS_INFORMATION * pi)
	{
		// Initialize base code struct
		forceStruct_t	code;
		code.push1_ = 0x68;
		code.push1 =  0x00;
		code.push2_ = 0x68;
		code.push2 =  0x00;
		code.pushopc = 0x68;
		code.callopc = 0xE8;
		code.retopc  = 0xC2;
		code.retvalue= 0x04;

#ifdef _UNICODE
		DWORD	lpLoadLibrary = (DWORD)GetProcAddress(LoadLibrary(_T("kernelbase.dll")), "LoadLibraryExW");
#else
		//DWORD	lpLoadLibrary = (DWORD)GetProcAddress(LoadLibrary(_T("kernelbase.dll")), _T("LoadLibraryExA"));
#endif
		
		LPVOID	lpCodeStart = VirtualAllocEx(pi->hProcess, NULL, sizeof(code), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (lpCodeStart == NULL)
		{
			return false;
		}

		DWORD	dwCodeStart = (DWORD)lpCodeStart;

		// String offset
		code.pushaddr = dwCodeStart + 13+10;

		// Call address
		code.calladdr = lpLoadLibrary - dwCodeStart - 10-10;

		// Copy dll name
		_tcscpy(code.lib_path, libName);

		DWORD	dwBytesWritten = 0;
		if (!WriteProcessMemory(pi->hProcess, lpCodeStart, &code, sizeof(code), &dwBytesWritten))
		{
			VirtualFreeEx(pi->hProcess, lpCodeStart, sizeof(code), MEM_DECOMMIT);
			return false;
		}

		DWORD	dwRemoteThreadID;
		HANDLE	hRemoteThread = CreateRemoteThread(pi->hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lpCodeStart, NULL, 0, &dwRemoteThreadID);
		if (hRemoteThread == 0)
		{
			VirtualFreeEx(pi->hProcess, lpCodeStart, sizeof(code), MEM_DECOMMIT);
			return false;
		}

		WaitForSingleObject(hRemoteThread, INFINITE);

		DWORD	dwLibBase;
		if (!GetExitCodeThread(hRemoteThread, &dwLibBase))
		{
			VirtualFreeEx(pi->hProcess, lpCodeStart, sizeof(code), MEM_DECOMMIT);
			return false;
		}

		VirtualFreeEx(pi->hProcess, lpCodeStart, sizeof(code), MEM_DECOMMIT);

		CloseHandle(hRemoteThread);

		return dwLibBase != 0;
	}
}