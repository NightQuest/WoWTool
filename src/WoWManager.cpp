#include "main.h"

// Constructor for WoWManager will give the program SeDebug privileges
WoWManager::WoWManager() : hProcess(INVALID_HANDLE_VALUE), baseAddress(NULL)
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
bool WoWManager::Attach(DWORD dwPID)
{
	MODULEENTRY32 me32;
	IMAGE_DOS_HEADER dosHeader;
	IMAGE_NT_HEADERS32 ntHeaders;
	PBYTE baseAddr = NULL;
	HANDLE hModuleSnap;
	SIZE_T size = 0;
	
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if( hModuleSnap == INVALID_HANDLE_VALUE )
		return false;

	me32.dwSize = sizeof(MODULEENTRY32);
	if( !Module32First(hModuleSnap, &me32) )
		return false;

	baseAddr = me32.modBaseAddr;
	CloseHandle(hModuleSnap);

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
	if( !hProcess )
		return false;

	if( !ReadProcessMemory(hProcess, baseAddr, &dosHeader, sizeof(IMAGE_DOS_HEADER), &size) || size != sizeof(IMAGE_DOS_HEADER) )
	{
		CloseHandle(hProcess);
		return false;
	}
	if( !ReadProcessMemory(hProcess, ((PBYTE)baseAddr + dosHeader.e_lfanew), &ntHeaders, sizeof(IMAGE_NT_HEADERS32), &size) || size != sizeof(IMAGE_NT_HEADERS32) )
	{
		CloseHandle(hProcess);
		return false;
	}

	baseAddress = reinterpret_cast<PBYTE>(ntHeaders.OptionalHeader.ImageBase + ntHeaders.OptionalHeader.BaseOfCode);
	plr = new WoWPlayer(hProcess, baseAddress);
	return (hProcess != INVALID_HANDLE_VALUE);
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
			WaitForInputIdle(pi.hProcess, 5000);
		}
	}

	return (hProcess != INVALID_HANDLE_VALUE);
}

// Sets the speed of animations, default: 1000
bool WoWManager::SetAnimationSpeed(double speed)
{
	if( !hProcess )
		return false;

	SIZE_T size;
	return (WriteProcessMemory(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION), &speed, sizeof(double), &size) && size == sizeof(double));
}

// Sets the game speed of the game (speed of everything from time, animations, etc)
bool WoWManager::SetGameSpeed(double speed)
{
	if( !hProcess )
		return false;

	SIZE_T size;
	return (WriteProcessMemory(hProcess, (baseAddress + ENGINE_GAME_SPEED), &speed, sizeof(double), &size) && size == sizeof(double));
}