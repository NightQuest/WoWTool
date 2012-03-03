#pragma once

// Generic includes
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

// WoW stuff
#include "Defines.h"
#include "Defines_8606.h"
#include "Defines_12340.h"
#include "Camera.h"
#include "Player.h"
#include "WoWManager.h"

// Windows stuff
#include "resource.h"
#include "MainWindow.h"
#include "main.h"

enum WindowIDs
{
	// Main Window stuff
	HMENU_MAIN_WINDOW_SYSMENU_TOPMOST = 20,
	HMENU_MAIN_WINDOW_SYSMENU_EASTEREGG = 21,
	HMENU_MAIN_WINDOW = 110,
	HMENU_COMMENTATOR_GROUPBOX,
	HMENU_COMMENTATOR_CHECKBOX,
	HMENU_COMMENTATOR_COLLISION_CHECKBOX,
	HMENU_COMMENTATOR_SPEED_STATIC,
	HMENU_COMMENTATOR_SPEED_SLIDER,
	HMENU_TELEPORT_FORWARD_BUTTON,
	HMENU_CAMERA_FOV_STATIC,
	HMENU_CAMERA_FOV_SLIDER,
	HMENU_RENDER_WIREFRAME_CHECKBOX,
};

// Global static variables
static HWND		hwndMain, hwndCommentatorModeGroupBox, hwndCommentatorCheckbox, hwndCommentatorCollisionCheckbox,
				hwndTeleportForwardButton,	hwndCameraFOVStatic, hwndCameraFOVSlider, hwndCommentatorSpeedStatic,
				hwndCommentatorSpeedSlider,	hwndWireframeCheckbox;
static WoWManager	wm;
