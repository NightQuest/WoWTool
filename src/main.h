#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS
#define WIN32_MEAN_AND_LEAN
#define _USE_MATH_DEFINES
#include <windows.h>
#include <commctrl.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <iostream>
#include <math.h>

#include "resource.h"
#include "Defines.h"
#include "Camera.h"
#include "Player.h"
#include "WoWManager.h"

BOOL InitApplication(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HandleMainWindowCreate(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HandleMainWindowShowWindow(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HandleMainWindowCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HandleMainWindowHScroll(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);