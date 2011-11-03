#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>

#include "WoWCamera.h"
#include "WoWPlayer.h"
#include "WoWManager.h"

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);