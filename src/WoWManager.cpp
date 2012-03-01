#include "PreCompiled.h"

// Constructor for WoWManager will give the program SeDebug privileges
WoWManager::WoWManager() : hProcess(INVALID_HANDLE_VALUE), dwPID(0), baseAddress(NULL), gameVersion(NULL), plr(NULL), cam(NULL)
{
	HANDLE hToken = INVALID_HANDLE_VALUE;
	TOKEN_PRIVILEGES tp;

	// Grab a token for the current process with adjustment process
	// On fail, delete the class instance, and set the pointers to it to NULL (fail)
	if( !OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken) )
	{
		delete this;
		const_cast<WoWManager *>(this) = NULL;
		return;
	}

	// Lookup the priviledge for debug
	// On fail, delete the class instance, and set the pointers to it to NULL (fail)
	if( !LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid) )
	{
		CloseHandle(hToken);
		delete this;
		const_cast<WoWManager *>(this) = NULL;
		return;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Enable Debug priviledges
	// On fail, delete the class instance, and set the pointers to it to NULL (fail)
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
	// Call all external Deinitializations that need to take place (deleting the player and camera class, for instance)
	Deinitialize();

	// Remove Debug priviledges for the current process
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

	// If still attached to Wow.exe, close the handle associated with it
	if( hProcess )
		CloseHandle(hProcess);
}

// Attaches the class to an already-loaded instance of WoW.
// Return true on success
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

	// Create a snapshot of all modules of the passed process ID (or try until we do)
	do
	{
		hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE|TH32CS_SNAPMODULE32, dwInPID);
		if( hModuleSnap != INVALID_HANDLE_VALUE )
			break;
	} while( GetLastError() == ERROR_BAD_LENGTH );

	if( hModuleSnap == INVALID_HANDLE_VALUE )
		return false;

	// Start to loop through the module list
	me32.dwSize = sizeof(MODULEENTRY32);
	if( !Module32First(hModuleSnap, &me32) )
		return false;

	// Loop through the module list until we find Wow.exe (or reach the end)
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

	// Retrieve the lower word of the version (Ex: 2.4.3.8606 would return 8606)
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

	// Grab the base address of the module
	baseAddr = me32.modBaseAddr;

	// Attempt to open the passed Process ID
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwInPID);
	if( !hProcess )
		return false;

	// Read the DOS header so we can retrieve the address to the NT header
	if( !ReadProcessMemory(hProcess, baseAddr, &dosHeader, sizeof(IMAGE_DOS_HEADER), &size) || size != sizeof(IMAGE_DOS_HEADER) || dosHeader.e_magic != IMAGE_DOS_SIGNATURE )
	{
		CloseHandle(hProcess);
		hProcess = INVALID_HANDLE_VALUE;
		return false;
	}
	// Read the NT header so we can get the base of code address, and image base address
	if( !ReadProcessMemory(hProcess, ((PBYTE)baseAddr + dosHeader.e_lfanew), &ntHeaders, sizeof(IMAGE_NT_HEADERS32), &size) || size != sizeof(IMAGE_NT_HEADERS32) || ntHeaders.Signature != IMAGE_NT_SIGNATURE )
	{
		CloseHandle(hProcess);
		hProcess = INVALID_HANDLE_VALUE;
		return false;
	}

	// Since we're here, we've successfully read everything above.
	// Set the baseAddress, PID, and call basic initializers (creating an instance of the Player class, for instance)
	if( hProcess != NULL && hProcess != INVALID_HANDLE_VALUE )
	{
		dwPID = dwInPID;
		baseAddress = reinterpret_cast<PBYTE>(ntHeaders.OptionalHeader.ImageBase + ntHeaders.OptionalHeader.BaseOfCode);

		Initialize();
		return true;
	}
	return false;
}

// Initialize basic classes and other variables inside of private
void WoWManager::Initialize()
{
	plr = new Player(hProcess, baseAddress);
	cam = new Camera(hProcess, baseAddress);
}

// delete initializations of classes and other variables inside of private
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
// Returns true on success
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

	// Make sure the passed file is there
	hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFile == INVALID_HANDLE_VALUE )
		return false;
	else
	{
		CloseHandle(hFile);

		// Attempt to launch the passed path and commandlines
		if( !CreateProcess(path, ((commandline != NULL && *commandline != NULL) ? commandline : NULL), NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi) )
			return false;
		else
		{
			// On success, sore the handle to the process, as well as the PID - then wait for the program to idle (max 5sec)
			hProcess = pi.hProcess;
			dwPID = pi.dwProcessId;
			WaitForInputIdle(pi.hProcess, 5000);
		}
	}

	return (hProcess != INVALID_HANDLE_VALUE);
}

// Returns the location of the attached copy of WoW, NULL on failure
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
// Returns true on success
bool WoWManager::SetAnimationSpeed(double speed)
{
	if( !hProcess || !dwPID )
		return false;

	SIZE_T size;
	return (WriteProcessMemory(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_8606), &speed, sizeof(double), &size) && size == sizeof(double));
}

// Sets the game speed of the game (speed of everything from time, animations, etc)
// Default: 0.00100000004749
// Returns true on success
bool WoWManager::SetGameSpeed(double speed)
{
	if( !hProcess || !dwPID )
		return false;

	SIZE_T size;
	return (WriteProcessMemory(hProcess, (baseAddress + ENGINE_GAME_SPEED_8606), &speed, sizeof(double), &size) && size == sizeof(double));
}