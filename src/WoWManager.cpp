#include "main.h"

// Constructor for WoWManager will give the program SeDebug privileges
WoWManager::WoWManager() : hProcess(INVALID_HANDLE_VALUE), dwPID(0), baseAddress(NULL), gameVersion(NULL), plr(NULL), cam(NULL)
{
	HANDLE hToken = INVALID_HANDLE_VALUE;

	if( !OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken) )
	{
		delete this;
		const_cast<WoWManager *>(this) = NULL;
		return;
	}

	TOKEN_PRIVILEGES tp;
	if( !LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid) )
	{
		CloseHandle(hToken);
		delete this;
		const_cast<WoWManager *>(this) = NULL;
		return;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if( !AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL) )
	{
		CloseHandle(hToken);
		delete this;
		const_cast<WoWManager *>(this) = NULL;
		return;
	}

	CloseHandle(hToken);
}

// Destructor for WoWManager, will remove SeDebug from the programs privilege level and Detach the program from WoW
WoWManager::~WoWManager()
{
	Deinitialize();

	HANDLE hToken = INVALID_HANDLE_VALUE;
	if( OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken) )
	{
		TOKEN_PRIVILEGES tp;
		if( LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid) )
		{
			tp.PrivilegeCount = 1;
			tp.Privileges[0].Attributes = SE_PRIVILEGE_REMOVED;

			AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
		}

		CloseHandle(hToken);
	}

	if( hProcess )
		CloseHandle(hProcess);
}

// Attaches the class to an already-loaded instance of WoW.
bool WoWManager::Attach(DWORD dwInPID)
{
	MODULEENTRY32 me32;
	IMAGE_DOS_HEADER dosHeader;
	IMAGE_NT_HEADERS32 ntHeaders;
	PBYTE baseAddr = NULL;
	HANDLE hModuleSnap;
	DWORD lastError = NULL;
	TCHAR *ptr = NULL;
	SIZE_T size = 0;

	do
	{
		hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE|TH32CS_SNAPMODULE32, dwInPID);
		if( hModuleSnap != INVALID_HANDLE_VALUE )
			break;
	} while( GetLastError() == ERROR_BAD_LENGTH );

	if( hModuleSnap == INVALID_HANDLE_VALUE )
		return false;

	me32.dwSize = sizeof(MODULEENTRY32);
	if( !Module32First(hModuleSnap, &me32) )
		return false;

	do
	{
		ptr = _tcsrchr(me32.szExePath, '\\');
		if( ptr == NULL )
			ptr = _tcsrchr(me32.szExePath, '/');

		if( ptr != NULL )
		{
			ptr++;
			if( !_tcscmp(ptr, _T("Wow.exe")) )
				break;
		}
	} while( Module32Next(hModuleSnap, &me32) );
	CloseHandle(hModuleSnap);

	DWORD dwDummy, dwLen;
	dwLen = GetFileVersionInfoSize(me32.szExePath, &dwDummy);
	if( dwLen )
	{
		BYTE *versionInfo = new BYTE[dwLen];
		if( versionInfo )
		{
			if( GetFileVersionInfo(me32.szExePath, NULL, dwLen, versionInfo) )
			{
				UINT uLen;
				VS_FIXEDFILEINFO *ffi;
				if( VerQueryValue(versionInfo, _T("\\"), (LPVOID *)&ffi, &uLen) != 0 )
					gameVersion = LOWORD(ffi->dwFileVersionLS);
			}
			delete[] versionInfo;
		}
	}
	baseAddr = me32.modBaseAddr;

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwInPID);
	if( !hProcess )
		return false;

	if( !ReadProcessMemory(hProcess, baseAddr, &dosHeader, sizeof(IMAGE_DOS_HEADER), &size) || size != sizeof(IMAGE_DOS_HEADER) || dosHeader.e_magic != IMAGE_DOS_SIGNATURE )
	{
		CloseHandle(hProcess);
		hProcess = INVALID_HANDLE_VALUE;
		return false;
	}
	if( !ReadProcessMemory(hProcess, ((PBYTE)baseAddr + dosHeader.e_lfanew), &ntHeaders, sizeof(IMAGE_NT_HEADERS32), &size) || size != sizeof(IMAGE_NT_HEADERS32) )
	{
		CloseHandle(hProcess);
		hProcess = INVALID_HANDLE_VALUE;
		return false;
	}

	if( hProcess != NULL && hProcess != INVALID_HANDLE_VALUE )
	{
		dwPID = dwInPID;
		baseAddress = reinterpret_cast<PBYTE>(ntHeaders.OptionalHeader.ImageBase + ntHeaders.OptionalHeader.BaseOfCode);

		Initialize();
		return true;
	}
	return false;
}

void WoWManager::Initialize()
{
	plr = new WoWPlayer(hProcess, baseAddress);
	cam = new WoWCamera(hProcess, baseAddress);
}

void WoWManager::Deinitialize()
{
	if( plr )
	{
		delete plr;
		plr = NULL;
	}
	if( cam )
	{
		delete cam;
		cam = NULL;
	}
}

// Launches an instance of WoW with the supplied commandline arguments, and then attaches the class to it.
bool WoWManager::Launch(TCHAR *path, TCHAR *commandline)
{
	if( path == NULL || *path == NULL )
		return false;

	HANDLE hFile;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	SECURITY_ATTRIBUTES sa;
	ZeroMemory(&si, sizeof(si)); si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&sa, sizeof(sa));
	hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFile == INVALID_HANDLE_VALUE )
		return false;
	else
	{
		CloseHandle(hFile);

		if( !CreateProcess(path, ((commandline != NULL && *commandline != NULL) ? commandline : NULL), NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi) )
			return false;
		else
		{
			hProcess = pi.hProcess;
			dwPID = pi.dwProcessId;
			WaitForInputIdle(pi.hProcess, 5000);
		}
	}

	return (hProcess != INVALID_HANDLE_VALUE);
}

// Returns the location of the attached copy of WoW
TCHAR *WoWManager::GetProgramLocation()
{
	MODULEENTRY32 me32;
	HANDLE hModuleSnap;
	TCHAR *ptr = NULL;
	TCHAR *final = NULL;

	if( !hProcess || !dwPID )
		return NULL;

	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if( hModuleSnap == INVALID_HANDLE_VALUE )
		return NULL;

	me32.dwSize = sizeof(MODULEENTRY32);
	if( !Module32First(hModuleSnap, &me32) )
		return NULL;

	do
	{
		ptr = _tcsrchr(me32.szExePath, '\\');
		if( ptr == NULL )
			ptr = _tcsrchr(me32.szExePath, '/');

		if( ptr != NULL )
		{
			ptr++;
			if( !_tcscmp(ptr, _T("Wow.exe")) )
			{
				final = _tcsdup(me32.szExePath);
				break;
			}
		}
	} while( Module32Next(hModuleSnap, &me32) );
	CloseHandle(hModuleSnap);

	return final;
}

// Sets the speed of animations
// Default: 1000
bool WoWManager::SetAnimationSpeed(double speed)
{
	if( !hProcess || !dwPID )
		return false;

	SIZE_T size;
	return (WriteProcessMemory(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_8606), &speed, sizeof(double), &size) && size == sizeof(double));
}

// Sets the game speed of the game (speed of everything from time, animations, etc)
// Default: 0.00100000004749
bool WoWManager::SetGameSpeed(double speed)
{
	if( !hProcess || !dwPID )
		return false;

	SIZE_T size;
	return (WriteProcessMemory(hProcess, (baseAddress + ENGINE_GAME_SPEED_8606), &speed, sizeof(double), &size) && size == sizeof(double));
}