
//#define WINVER 0x0400
//#define _WIN32_WINNT 0x0400

#include	<Windows.h>
#include	<tchar.h>
#include	"ForceLibrary.h"

#define MSL	512

TCHAR		theLibraryPath[MSL];

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	// Create library name reference
	theLibraryPath[0] = _T('\0');

	GetModuleFileName(GetModuleHandle(NULL), theLibraryPath, MSL);
	TCHAR * c = theLibraryPath + _tcslen(theLibraryPath);
	while (c > theLibraryPath && *c != _T('.')) c--;
	if (*c == _T('.'))
	{
		_tcscpy(c, _T(".dll"));
	}

	// Create process
	STARTUPINFO          si;
	PROCESS_INFORMATION  pi;

	// start the shit !
	ZeroMemory(&si,sizeof(STARTUPINFO));
	ZeroMemory(&pi,sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFO);

	if (CreateProcess(_T("orion95.exe"), lpstrCmdLine, NULL, NULL, FALSE, CREATE_SUSPENDED/*|CREATE_NEW_CONSOLE*/, NULL, NULL, &si, &pi))
	{
		if (!nForceLibrary::ForceLibrary(theLibraryPath, &pi))
		{
			MessageBox(0, _T("Failed to inject library. Aborting."), _T("Oops, error happened."), MB_OK);

			TerminateProcess(pi.hProcess, 0);
		} else
		{
			ResumeThread(pi.hThread);
		}
	} else
	{
		MessageBox(0, _T("Failed to start executable, please check path."), _T("Oops, error happened."), MB_OK);
		return false;
	}

	return 0;
}
