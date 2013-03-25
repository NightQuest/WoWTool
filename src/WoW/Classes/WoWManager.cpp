#include "PreCompiled.h"

// Constructor for WoWManager will give the program SeDebug privileges
WoWManager::WoWManager()
{
	HANDLE hToken = INVALID_HANDLE_VALUE;
	TOKEN_PRIVILEGES tp;

	// Initialize private data
	hProcess = INVALID_HANDLE_VALUE;
	dwPID = 0;
	baseAddress = NULL;
	gameVersion = 0;
	gameLocation = NULL;
	eng = NULL;
	plr = NULL;
	cam = NULL;

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

bool WoWManager::IsAttached()
{
	HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 pe32;
	TCHAR *ptr = NULL;
	DWORD dwInPID = 0;

	// All of these should have a value if we're attached to the game
	if( hProcess == NULL || hProcess == INVALID_HANDLE_VALUE || dwPID == 0 || baseAddress == NULL )
		return false;

	// Create a list of all the processes running on the system (or try until we do)
	do
	{
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if( hProcessSnap != INVALID_HANDLE_VALUE )
			break;
	} while( GetLastError() == ERROR_BAD_LENGTH );

	if( hProcessSnap == INVALID_HANDLE_VALUE )
		return FALSE;

	// Start to loop through the process list
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if( !Process32First(hProcessSnap, &pe32) )
		return FALSE;

	// Find "Wow.exe" and save the PID if found
	do
	{
		if( !_tcscmp(pe32.szExeFile, _T("Wow.exe")) )
		{
			dwInPID = pe32.th32ProcessID;
			break;
		}
	} while( Process32Next(hProcessSnap, &pe32) );
	CloseHandle( hProcessSnap );

	// Verify the PID
	if( dwInPID == 0 || dwInPID != dwPID )
		return false;

	return true;
}

// Initialize basic classes and other variables inside of private
void WoWManager::Initialize()
{
	MODULEENTRY32 me32;
	HANDLE hModuleSnap;
	TCHAR *ptr = NULL;
	TCHAR *final = NULL;

	if( hProcess && dwPID )
	{
		hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
		if( hModuleSnap != INVALID_HANDLE_VALUE )
		{
			me32.dwSize = sizeof(MODULEENTRY32);
			if( Module32First(hModuleSnap, &me32) )
			{
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
							gameLocation = _tcsdup(me32.szExePath);
							break;
						}
					}
				} while( Module32Next(hModuleSnap, &me32) );
				CloseHandle(hModuleSnap);
			}
		}
	}

	if( gameLocation != NULL )
	{
		// Retrieve the lower word of the version (Ex: 2.4.3.8606 would return 8606)
		DWORD dwDummy, dwLen;
		dwLen = GetFileVersionInfoSize(gameLocation, &dwDummy);
		if( dwLen )
		{
			BYTE *versionInfo = new BYTE[dwLen];
			if( versionInfo )
			{
				if( GetFileVersionInfo(gameLocation, NULL, dwLen, versionInfo) )
				{
					UINT uLen;
					VS_FIXEDFILEINFO *ffi;
					if( VerQueryValue(versionInfo, _T("\\"), (LPVOID *)&ffi, &uLen) != 0 )
						gameVersion = LOWORD(ffi->dwFileVersionLS);
				}
				delete[] versionInfo;
			}
		}
	}

	eng = new Engine(hProcess, baseAddress);
	plr = new Player(hProcess, baseAddress);
	cam = new Camera(hProcess, baseAddress);
	Patch();
}

// delete initializations of classes and other variables inside of private
void WoWManager::Deinitialize()
{
	if( eng )
	{
		delete eng;
		eng = NULL;
	}
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
	if( hProcess )
	{
		Depatch();
	}
	if( gameLocation )
	{
		free(gameLocation);
		gameLocation = NULL;
	}
}

// Edits the games code so we can do certain things (edit the speed of animations without crashing, for instance)
void WoWManager::Patch()
{
	switch( gameVersion )
	{
	case FINAL_WOTLK:
		{
			SIZE_T size = 0;
			BYTE speedOfAnimationPatchOrig[6] = { NULL };
			BYTE speedOfAnimationPatchVerify[6] = ENGINE_SPEED_OF_ANIMATION_PATCH_VERIFY_12340;
			BYTE speedOfAnimationPatchData[6] = ENGINE_SPEED_OF_ANIMATION_PATCH_PATCH_12340;
			float speedOfAnimation = 0.0f;
			float speedOfAnimationNew = 0.0f;

			// Read the code at the address we're about to modify
			if( ReadProcessMemory(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_PATCH_ADDRESS_12340), &speedOfAnimationPatchOrig, sizeof(BYTE)*6, &size) && size == sizeof(BYTE)*6 )
			{
				// Make sure the code read what matches what we know - we don't want to be writing to the wrong address
				if( memcmp(speedOfAnimationPatchOrig, speedOfAnimationPatchVerify, sizeof(BYTE)*6) == 0 )
				{
					// Read both the new, and old addresses of the speed of animation
					if( (ReadProcessMemory(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_12340), &speedOfAnimation, sizeof(float), &size) && size == sizeof(float)) &&
						(ReadProcessMemory(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_NEW_12340), &speedOfAnimationNew, sizeof(float), &size) && size == sizeof(float)) )
					{
						// Make sure the new address is all 0's, we don't want overwrite something in use..
						// Then write the old value to the new address before we change the address being accessed
						if( speedOfAnimationNew == 0.0f )
						{
							DWORD oldProtect, oldProtect2;
							// This code region doesn't have write permissions, so we need to change that before we put our new code in
							if( VirtualProtectEx(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_PATCH_ADDRESS_12340), 6, PAGE_EXECUTE_READWRITE, &oldProtect) != 0 &&
								VirtualProtectEx(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_NEW_12340), sizeof(float), PAGE_READWRITE, &oldProtect2) != 0 )
							{
								// Write the old value into the new address
								if( WriteProcessMemory(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_NEW_12340), &speedOfAnimation, sizeof(float), &size) && size == sizeof(float) )
								{
									// Now we finally change the code to access the new address
									if( WriteProcessMemory(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_PATCH_ADDRESS_12340), &speedOfAnimationPatchData, sizeof(BYTE)*6, &size) && size == sizeof(BYTE)*6 )
									{
										// Restore the old permissions
										VirtualProtectEx(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_PATCH_ADDRESS_12340), 6, oldProtect, &oldProtect);
										VirtualProtectEx(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_NEW_12340), sizeof(float), oldProtect2, &oldProtect2);
									}
								}
							}
						}
					}
				}
			}
		}
		break;

	case FINAL_TBC:
		{
			SIZE_T size = 0;
			BYTE speedOfAnimationPatchOrig[6] = { NULL };
			BYTE speedOfAnimationPatchVerify[6] = ENGINE_SPEED_OF_ANIMATION_PATCH_VERIFY_8606;
			BYTE speedOfAnimationPatchData[6] = ENGINE_SPEED_OF_ANIMATION_PATCH_PATCH_8606;
			double speedOfAnimation = 0.0f;
			double speedOfAnimationNew = 0.0f;

			// Read the code at the address we're about to modify
			if( ReadProcessMemory(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_PATCH_ADDRESS_8606), &speedOfAnimationPatchOrig, sizeof(BYTE)*6, &size) && size == sizeof(BYTE)*6 )
			{
				// Make sure the code read what matches what we know - we don't want to be writing to the wrong address
				if( memcmp(speedOfAnimationPatchOrig, speedOfAnimationPatchVerify, sizeof(BYTE)*6) == 0 )
				{
					// Read both the new, and old addresses of the speed of animation
					if( (ReadProcessMemory(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_8606), &speedOfAnimation, sizeof(double), &size) && size == sizeof(double)) &&
						(ReadProcessMemory(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_NEW_8606), &speedOfAnimationNew, sizeof(double), &size) && size == sizeof(double)) )
					{
						// Make sure the new address is all 0's, we don't want overwrite something in use..
						// Then write the old value to the new address before we change the address being accessed
						if( speedOfAnimationNew == 0.0f )
						{
							DWORD oldProtect, oldProtect2;
							// This code region doesn't have write permissions, so we need to change that before we put our new code in
							if( VirtualProtectEx(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_PATCH_ADDRESS_8606), 6, PAGE_EXECUTE_READWRITE, &oldProtect) != 0 &&
								VirtualProtectEx(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_NEW_8606), sizeof(double), PAGE_READWRITE, &oldProtect2) != 0 )
							{
								// Write the old value into the new address
								if( WriteProcessMemory(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_NEW_8606), &speedOfAnimation, sizeof(double), &size) && size == sizeof(double) )
								{
									// Now we finally change the code to access the new address
									if( WriteProcessMemory(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_PATCH_ADDRESS_8606), &speedOfAnimationPatchData, sizeof(BYTE)*6, &size) && size == sizeof(BYTE)*6 )
									{
										// Restore the old permissions
										VirtualProtectEx(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_PATCH_ADDRESS_8606), 6, oldProtect, &oldProtect);
										VirtualProtectEx(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_NEW_8606), sizeof(double), oldProtect2, &oldProtect2);
									}
								}
							}
						}
					}
				}
			}
		}
		break;
	}
}

// This should only be called when Detaching from the game, it will restore all code edits we've done.
void WoWManager::Depatch()
{
	switch( gameVersion )
	{
	case FINAL_WOTLK:
		{
			SIZE_T size = 0;
			BYTE speedOfAnimationPatchOrig[6] = { NULL };
			BYTE speedOfAnimationPatchVerify[6] = ENGINE_SPEED_OF_ANIMATION_PATCH_PATCH_12340;
			BYTE speedOfAnimationPatchData[6] = ENGINE_SPEED_OF_ANIMATION_PATCH_VERIFY_12340;
			float speedOfAnimation = 0.0f;

			// Read the code at the address we're about to modify
			if( ReadProcessMemory(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_PATCH_ADDRESS_12340), &speedOfAnimationPatchOrig, sizeof(BYTE)*6, &size) && size == sizeof(BYTE)*6 )
			{
				// Make sure the code read what matches what we know - we don't want to be writing to the wrong address
				if( memcmp(speedOfAnimationPatchOrig, speedOfAnimationPatchVerify, sizeof(BYTE)*6) == 0 )
				{
					DWORD oldProtect, oldProtect2;
					// This code region doesn't have write permissions, so we need to change that before we put our new code in
					if( VirtualProtectEx(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_PATCH_ADDRESS_12340), 6, PAGE_EXECUTE_READWRITE, &oldProtect) != 0 &&
						VirtualProtectEx(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_NEW_12340), sizeof(float), PAGE_READWRITE, &oldProtect2) != 0 )
					{
						// Write the 0s value into the new address
						if( WriteProcessMemory(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_NEW_12340), &speedOfAnimation, sizeof(float), &size) && size == sizeof(float) )
						{
							// Now we finally change the code to access the old address again
							if( WriteProcessMemory(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_PATCH_ADDRESS_12340), &speedOfAnimationPatchData, sizeof(BYTE)*6, &size) && size == sizeof(BYTE)*6 )
							{
								// Restore the old permissions
								VirtualProtectEx(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_PATCH_ADDRESS_12340), 6, oldProtect, &oldProtect);
								VirtualProtectEx(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_NEW_12340), sizeof(float), oldProtect2, &oldProtect2);
							}
						}
					}
				}
			}
		}
		break;

	case FINAL_TBC:
		{
			SIZE_T size = 0;
			BYTE speedOfAnimationPatchOrig[6] = { NULL };
			BYTE speedOfAnimationPatchVerify[6] = ENGINE_SPEED_OF_ANIMATION_PATCH_PATCH_8606;
			BYTE speedOfAnimationPatchData[6] = ENGINE_SPEED_OF_ANIMATION_PATCH_VERIFY_8606;
			double speedOfAnimation = 0.0f;

			// Read the code at the address we're about to modify
			if( ReadProcessMemory(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_PATCH_ADDRESS_8606), &speedOfAnimationPatchOrig, sizeof(BYTE)*6, &size) && size == sizeof(BYTE)*6 )
			{
				// Make sure the code read what matches what we know - we don't want to be writing to the wrong address
				if( memcmp(speedOfAnimationPatchOrig, speedOfAnimationPatchVerify, sizeof(BYTE)*6) == 0 )
				{
					DWORD oldProtect, oldProtect2;
					// This code region doesn't have write permissions, so we need to change that before we put our new code in
					if( VirtualProtectEx(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_PATCH_ADDRESS_8606), 6, PAGE_EXECUTE_READWRITE, &oldProtect) != 0 &&
						VirtualProtectEx(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_NEW_8606), sizeof(double), PAGE_READWRITE, &oldProtect2) != 0 )
					{
						// Write the 0s value into the new address
						if( WriteProcessMemory(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_NEW_8606), &speedOfAnimation, sizeof(double), &size) && size == sizeof(double) )
						{
							// Now we finally change the code to access the old address again
							if( WriteProcessMemory(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_PATCH_ADDRESS_8606), &speedOfAnimationPatchData, sizeof(BYTE)*6, &size) && size == sizeof(BYTE)*6 )
							{
								// Restore the old permissions
								VirtualProtectEx(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_PATCH_ADDRESS_8606), 6, oldProtect, &oldProtect);
								VirtualProtectEx(hProcess, (baseAddress + ENGINE_SPEED_OF_ANIMATION_NEW_8606), sizeof(double), oldProtect2, &oldProtect2);
							}
						}
					}
				}
			}
		}
		break;
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
TCHAR *WoWManager::GetGameLocation() { return gameLocation; }

// Returns the version of the attached copy of WoW, 0 on failure
UINT WoWManager::GetGameVersion() { return gameVersion; }

// Checks whether or not the sky position has been patched.
// Returns: true if it's been patched
bool WoWManager::HasPatchedSkyPosition()
{
	switch( gameVersion )
	{
	case FINAL_WOTLK:
		{
			SIZE_T size = 0;
			SIZE_T sizeofEdit = 5;
			BYTE skyPositionPatchOrig[5] = { NULL };
			BYTE skyPositionPatchVerify[5] = ENGINE_SKY_POSITION_PATCH_VERIFY_12340;
			
			// Read the code at the address we're about to check
			if( ReadProcessMemory(hProcess, (baseAddress + ENGINE_SKY_POSITION_PATCH_ADDRESS_12340), &skyPositionPatchOrig, sizeof(BYTE)*sizeofEdit, &size) && size == sizeof(BYTE)*sizeofEdit )
			{
				// If the code matches what we know, it's not been patched.
				return (memcmp(skyPositionPatchOrig, skyPositionPatchVerify, sizeof(BYTE)*sizeofEdit) != 0);
			}
		}
		break;
	}

	return false;
}

// Edits the games code to stop the sky position from being updated every few minutes.
bool WoWManager::PatchSkyPosition()
{
	switch( gameVersion )
	{
	case FINAL_WOTLK:
		{
			SIZE_T size = 0;
			SIZE_T sizeofEdit = 5;
			BYTE skyPositionPatchOrig[5] = { NULL };
			BYTE skyPositionPatchVerify[5] = ENGINE_SKY_POSITION_PATCH_VERIFY_12340;
			BYTE skyPositionPatchData[5] = ENGINE_SKY_POSITION_PATCH_PATCH_12340;

			// Read the code at the address we're about to modify
			if( ReadProcessMemory(hProcess, (baseAddress + ENGINE_SKY_POSITION_PATCH_ADDRESS_12340), &skyPositionPatchOrig, sizeof(BYTE)*sizeofEdit, &size) && size == sizeof(BYTE)*sizeofEdit )
			{
				// Make sure the code read what matches what we know - we don't want to be writing to the wrong address
				if( memcmp(skyPositionPatchOrig, skyPositionPatchVerify, sizeof(BYTE)*sizeofEdit) == 0 )
				{
					DWORD oldProtect;
					// This code region doesn't have write permissions, so we need to change that before we put our new code in
					if( VirtualProtectEx(hProcess, (baseAddress + ENGINE_SKY_POSITION_PATCH_ADDRESS_12340), sizeofEdit, PAGE_EXECUTE_READWRITE, &oldProtect) != 0 )
					{
						// Now we change the code to never update the sky
						if( WriteProcessMemory(hProcess, (baseAddress + ENGINE_SKY_POSITION_PATCH_ADDRESS_12340), &skyPositionPatchData, sizeof(BYTE)*sizeofEdit, &size) && size == sizeof(BYTE)*sizeofEdit )
						{
							// Restore the old permissions
							VirtualProtectEx(hProcess, (baseAddress + ENGINE_SKY_POSITION_PATCH_ADDRESS_12340), sizeofEdit, oldProtect, &oldProtect);
							return true;
						}
					}
				}
			}
		}
		break;
	}

	return false;
}

// Edits the games code to allow the sky position to be updated every few minutes again.
bool WoWManager::DepatchSkyPosition()
{
	switch( gameVersion )
	{
	case FINAL_WOTLK:
		{
			SIZE_T size = 0;
			SIZE_T sizeofEdit = 5;
			BYTE skyPositionPatchOrig[5] = { NULL };
			BYTE skyPositionPatchVerify[5] = ENGINE_SKY_POSITION_PATCH_PATCH_12340;
			BYTE skyPositionPatchData[5] = ENGINE_SKY_POSITION_PATCH_VERIFY_12340;

			// Read the code at the address we're about to modify
			if( ReadProcessMemory(hProcess, (baseAddress + ENGINE_SKY_POSITION_PATCH_ADDRESS_12340), &skyPositionPatchOrig, sizeof(BYTE)*sizeofEdit, &size) && size == sizeof(BYTE)*sizeofEdit )
			{
				// Make sure the code read what matches what we know - we don't want to be writing to the wrong address
				if( memcmp(skyPositionPatchOrig, skyPositionPatchVerify, sizeof(BYTE)*sizeofEdit) == 0 )
				{
					DWORD oldProtect;
					// This code region doesn't have write permissions, so we need to change that before we put our new code in
					if( VirtualProtectEx(hProcess, (baseAddress + ENGINE_SKY_POSITION_PATCH_ADDRESS_12340), sizeofEdit, PAGE_EXECUTE_READWRITE, &oldProtect) != 0 )
					{
						// Now we change the code to update the sky again
						if( WriteProcessMemory(hProcess, (baseAddress + ENGINE_SKY_POSITION_PATCH_ADDRESS_12340), &skyPositionPatchData, sizeof(BYTE)*sizeofEdit, &size) && size == sizeof(BYTE)*sizeofEdit )
						{
							// Restore the old permissions
							VirtualProtectEx(hProcess, (baseAddress + ENGINE_SKY_POSITION_PATCH_ADDRESS_12340), sizeofEdit, oldProtect, &oldProtect);
							return true;
						}
					}
				}
			}
		}
		break;
	}

	return false;
}
