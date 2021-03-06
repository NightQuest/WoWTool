#include "PreCompiled.h"

// Proc for the main window (called whenever a message happens, pretty much)
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	// Called when the main windows gets created
	case WM_CREATE:
		return HandleMainWindowCreate(hwnd, msg, wParam, lParam);
		break;

	// Called when the main window is shown via ShowWindow()
	case WM_SHOWWINDOW:
		return HandleMainWindowShowWindow(hwnd, msg, wParam, lParam);
		break;

	// Called when the main window receives a user command (Eg: Right-click)
	case WM_COMMAND:
		return HandleMainWindowCommand(hwnd, msg, wParam, lParam);
		break;

	// Called when a horizontal scroll occurs in the main window
	case WM_HSCROLL:
		return HandleMainWindowHScroll(hwnd, msg, wParam, lParam);
		break;

	// Called when a SYSCOMMAND happens (Eg: Right clicking the title bar)
	case WM_SYSCOMMAND:
		return HandleMainWindowSysCommand(hwnd, msg, wParam, lParam);
		break;

	// Called before a menu is displayed, giving us time to modify it
	case WM_INITMENUPOPUP:
		return HandleMainWindowInitMenuPopup(hwnd, msg, wParam, lParam);
		break;

	// Called when the user clicks the 'X' on our main window usually
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	// Called when it's time to fully destroy our window, and exit the program
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return FALSE;
}

// Function to create our main window
LRESULT CALLBACK HandleMainWindowCreate(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Store hwndMain
	hwndMain = hwnd;

	// Initialize common controls so we can have modern looking buttons and such
	InitCommonControls();

	// Initialize the COM library so we can (if needed) open a file browsing dialog
	HRESULT hr = OleInitialize(NULL);
	if( hr != S_OK && hr != S_FALSE )
	{
		MessageBox(hwnd, _T("Failed to Initialize Shell!"), _T("Error!"), MB_ICONERROR | MB_OK);
		PostMessage(hwnd, WM_CLOSE, 0, 0);
	}

	// Create all the main UI elements
	hwndCommentatorModeGroupBox = CreateWindowEx(NULL,
		_T("Button"), _T("Commentator mode"), WS_CHILD | WS_VISIBLE | BS_TEXT | BS_GROUPBOX,
		10, 5, 110, 100, hwnd, (HMENU)HMENU_COMMENTATOR_GROUPBOX, NULL, NULL);

	hwndCommentatorCheckbox = CreateWindowEx(NULL,
		_T("Button"), _T("Enabled"), WS_CHILD | WS_VISIBLE | BS_TEXT | BS_AUTOCHECKBOX,
		20, 20, 60, 23, hwnd, (HMENU)HMENU_COMMENTATOR_CHECKBOX, NULL, NULL);

	hwndCommentatorCollisionCheckbox = CreateWindowEx(NULL,
		_T("Button"), _T("Collision"), WS_CHILD | WS_VISIBLE | BS_TEXT | BS_AUTOCHECKBOX,
		20, 40, 60, 23, hwnd, (HMENU)HMENU_COMMENTATOR_COLLISION_CHECKBOX, NULL, NULL);

	hwndCommentatorSpeedStatic = CreateWindowEx(NULL,
		_T("Static"), _T("Speed"), WS_CHILD | WS_VISIBLE,
		20, 65, 80, 15, hwnd, (HMENU)HMENU_COMMENTATOR_SPEED_STATIC, NULL, NULL);

	hwndCommentatorSpeedSlider = CreateWindowEx(NULL,
		TRACKBAR_CLASS, _T(""), WS_CHILD | WS_VISIBLE | TBS_NOTICKS | TBS_ENABLESELRANGE,
		13, 80, 100, 23, hwnd, (HMENU)HMENU_COMMENTATOR_SPEED_SLIDER, NULL, NULL);

	hwndTeleportForwardButton = CreateWindowEx(NULL,
		_T("Button"), _T("Teleport Forward"), WS_CHILD | WS_VISIBLE,
		175, 5, 100, 23, hwnd, (HMENU)HMENU_TELEPORT_FORWARD_BUTTON, NULL, NULL);

	hwndCameraFOVStatic = CreateWindowEx(NULL,
		_T("Static"), _T("Field of View"), WS_CHILD | WS_VISIBLE,
		175, 35, 110, 15, hwnd, (HMENU)HMENU_CAMERA_FOV_STATIC, NULL, NULL);

	hwndCameraFOVSlider = CreateWindowEx(NULL,
		TRACKBAR_CLASS, _T(""), WS_CHILD | WS_VISIBLE | TBS_NOTICKS | TBS_ENABLESELRANGE,
		170, 50, 100, 23, hwnd, (HMENU)HMENU_CAMERA_FOV_SLIDER, NULL, NULL);

	hwndWireframeCheckbox = CreateWindowEx(NULL,
		_T("Button"), _T("Wireframe"), WS_CHILD | WS_VISIBLE | BS_TEXT | BS_AUTOCHECKBOX,
		175, 95, 70, 23, hwnd, (HMENU)HMENU_RENDER_WIREFRAME_CHECKBOX, NULL, NULL);

	hwndEngineAnimationSpeedStatic = CreateWindowEx(NULL,
		_T("Static"), _T("Animation Speed"), WS_CHILD | WS_VISIBLE,
		165, 120, 150, 15, hwnd, (HMENU)HMENU_ENGINE_MODEL_ANIMATION_SPEED_STATIC, NULL, NULL);

	hwndEngineAnimationSpeedSlider = CreateWindowEx(NULL,
		TRACKBAR_CLASS, _T(""), WS_CHILD | WS_VISIBLE | TBS_NOTICKS | TBS_ENABLESELRANGE,
		170, 135, 100, 23, hwnd, (HMENU)HMENU_ENGINE_MODEL_ANIMATION_SPEED_SLIDER, NULL, NULL);

	hwndEngineSkyPositionCheckbox = CreateWindowEx(NULL,
		_T("Button"), _T("Sky Position"), WS_CHILD | WS_VISIBLE | BS_TEXT | BS_AUTOCHECKBOX,
		15, 110, 110, 15, hwnd, (HMENU)HMENU_ENGINE_SKY_POSITION_CHECKBOX, NULL, NULL);

	hwndEngineSkyPositionSlider = CreateWindowEx(NULL,
		TRACKBAR_CLASS, _T(""), WS_CHILD | WS_VISIBLE | TBS_NOTICKS | TBS_ENABLESELRANGE,
		7, 125, 100, 23, hwnd, (HMENU)HMENU_ENGINE_SKY_POSITION_SLIDER, NULL, NULL);

	hwndEngineNightSkyOpacityCheckbox = CreateWindowEx(NULL,
		_T("Button"), _T("Sky Position"), WS_CHILD | WS_VISIBLE | BS_TEXT | BS_AUTOCHECKBOX,
		15, 160, 125, 15, hwnd, (HMENU)HMENU_ENGINE_NIGHT_SKY_OPACITY_CHECKBOX, NULL, NULL);

	hwndEngineNightSkyOpacitySlider = CreateWindowEx(NULL,
		TRACKBAR_CLASS, _T(""), WS_CHILD | WS_VISIBLE | TBS_NOTICKS | TBS_ENABLESELRANGE,
		7, 175, 100, 23, hwnd, (HMENU)HMENU_ENGINE_NIGHT_SKY_OPACITY_SLIDER, NULL, NULL);

	hwndSnapPlayerToGroundNormalCheckbox = CreateWindowEx(NULL,
		_T("Button"), _T("Snap to Ground Normal"), WS_CHILD | WS_VISIBLE | BS_TEXT | BS_AUTOCHECKBOX,
		140, 175, 150, 15, hwnd, (HMENU)HMENU_ENGINE_SNAP_PLAYER_TO_GROUND_NORMAL_CHECKBOX, NULL, NULL);


	// Set the font of all the UI elements, so they won't have the default blocky look
	HFONT hfFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(hwndCommentatorModeGroupBox, WM_SETFONT, (WPARAM)hfFont, TRUE);
	SendMessage(hwndCommentatorCheckbox, WM_SETFONT, (WPARAM)hfFont, TRUE);
	SendMessage(hwndCommentatorCollisionCheckbox, WM_SETFONT, (WPARAM)hfFont, TRUE);
	SendMessage(hwndTeleportForwardButton, WM_SETFONT, (WPARAM)hfFont, TRUE);
	SendMessage(hwndCommentatorSpeedStatic, WM_SETFONT, (WPARAM)hfFont, TRUE);
	SendMessage(hwndCameraFOVStatic, WM_SETFONT, (WPARAM)hfFont, TRUE);
	SendMessage(hwndWireframeCheckbox, WM_SETFONT, (WPARAM)hfFont, TRUE);
	SendMessage(hwndEngineAnimationSpeedStatic, WM_SETFONT, (WPARAM)hfFont, TRUE);
	SendMessage(hwndEngineSkyPositionCheckbox, WM_SETFONT, (WPARAM)hfFont, TRUE);
	SendMessage(hwndEngineNightSkyOpacityCheckbox, WM_SETFONT, (WPARAM)hfFont, TRUE);
	SendMessage(hwndSnapPlayerToGroundNormalCheckbox, WM_SETFONT, (WPARAM)hfFont, TRUE);

	// Set the min/max of the field of view slider to 0/180
	// We use 100 times what the real value is on the max in order to allow decimal places
	SendMessage(hwndCameraFOVSlider, TBM_SETRANGEMIN, TRUE, (LPARAM)0l);
	SendMessage(hwndCameraFOVSlider, TBM_SETRANGEMAX, TRUE, (LPARAM)18000l);
    SendMessage(hwndCameraFOVSlider, TBM_SETPAGESIZE, 0, 100);
    SendMessage(hwndCameraFOVSlider, TBM_SETPOS, TRUE, 0);

	// Set the min/max of the Commentator speed slider to 1/100000
	// We use 100 times what the real value is on the max in order to allow decimal places
	SendMessage(hwndCommentatorSpeedSlider, TBM_SETRANGEMIN, TRUE, (LPARAM)1l);
	SendMessage(hwndCommentatorSpeedSlider, TBM_SETRANGEMAX, TRUE, (LPARAM)100000l);
    SendMessage(hwndCommentatorSpeedSlider, TBM_SETPAGESIZE, FALSE, 100);
    SendMessage(hwndCommentatorSpeedSlider, TBM_SETPOS, TRUE, 1);

	// Set the min/max of the Engine animation speed slider to 1/10000
	SendMessage(hwndEngineAnimationSpeedSlider, TBM_SETRANGEMIN, TRUE, (LPARAM)10l);
	SendMessage(hwndEngineAnimationSpeedSlider, TBM_SETRANGEMAX, TRUE, (LPARAM)10000l);
	SendMessage(hwndEngineAnimationSpeedSlider, TBM_SETPAGESIZE, FALSE, 100);
	SendMessage(hwndEngineAnimationSpeedSlider, TBM_SETPOS, TRUE, 100);

	// Set the min/max of the Engine sky position speed slider to 0/1440
	SendMessage(hwndEngineSkyPositionSlider, TBM_SETRANGEMIN, TRUE, (LPARAM)0l);
	SendMessage(hwndEngineSkyPositionSlider, TBM_SETRANGEMAX, TRUE, (LPARAM)1440l);
	SendMessage(hwndEngineSkyPositionSlider, TBM_SETPAGESIZE, FALSE, 60);
	SendMessage(hwndEngineSkyPositionSlider, TBM_SETPOS, TRUE, 0);

	// Set the min/max of the Engine sky position speed slider to 1/255
	SendMessage(hwndEngineNightSkyOpacitySlider, TBM_SETRANGEMIN, TRUE, (LPARAM)1l);
	SendMessage(hwndEngineNightSkyOpacitySlider, TBM_SETRANGEMAX, TRUE, (LPARAM)255l);
	SendMessage(hwndEngineNightSkyOpacitySlider, TBM_SETPAGESIZE, FALSE, 10);
	SendMessage(hwndEngineNightSkyOpacitySlider, TBM_SETPOS, TRUE, 0);

	// Add "Always on top" to the main windows titlebar right click menu
	HMENU hMenu = GetSystemMenu(hwnd, FALSE);
	AppendMenu(hMenu, MF_STRING, HMENU_MAIN_WINDOW_SYSMENU_TOPMOST, _T("Always on top"));

	// Easteregg (Except to those who look in a Debugger..)
	AppendMenu(hMenu, MF_STRING, HMENU_MAIN_WINDOW_SYSMENU_EASTEREGG, _T("Do a barrel roll!"));

	return FALSE;
}

// Function called when the main window is shown via ShowWindow()
LRESULT CALLBACK HandleMainWindowShowWindow(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
	HANDLE hProcess = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 pe32;
	DWORD dwPID = 0;

	// Create a list of all the processes running on the system (or try until we do)
	do
	{
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if( hProcessSnap != INVALID_HANDLE_VALUE )
			break;
	} while( GetLastError() == ERROR_BAD_LENGTH );

	if( hProcessSnap == INVALID_HANDLE_VALUE )
		return FALSE;

	pe32.dwSize = sizeof(PROCESSENTRY32);
	if( !Process32First(hProcessSnap, &pe32) )
		return FALSE;

	// Find "Wow.exe" and save the PID if found
	do
	{
		if( !_tcscmp(pe32.szExeFile, _T("Wow.exe")) )
		{
			dwPID = pe32.th32ProcessID;
			break;
		}
	} while( Process32Next(hProcessSnap, &pe32) );
	CloseHandle( hProcessSnap );

	// Attach WoWManager to Wow.exe
	if( !dwPID || !wm.Attach(dwPID) || !wm.GetEngine()->IsFullyLoadedInGame() )
	{
		MessageBox(NULL, _T("Cannot attach to WoW!\r\nPlease make sure that WoW is running and fully logged in, and you're running this tool as Administrator!"), _T("Error!"), MB_ICONERROR | MB_OK);
		PostMessage(hwnd, WM_CLOSE, 0, 0);
		return FALSE;
	}

	// Read the game's memory, and fill in the values for it
	SendMessage(hwndCommentatorCheckbox, BM_SETCHECK, (WPARAM)(wm.GetPlayer()->IsInCommentatorMode() ? BST_CHECKED : BST_UNCHECKED), NULL);
	SendMessage(hwndCommentatorCollisionCheckbox, BM_SETCHECK, (WPARAM)(wm.GetPlayer()->IsCommentatorCameraCollidable() ? BST_CHECKED : BST_UNCHECKED), NULL);
	SendMessage(hwndWireframeCheckbox, BM_SETCHECK, (WPARAM)(wm.GetEngine()->HasRenderingFlags(RENDER_FLAG_WIREFRAME) ? BST_CHECKED : BST_UNCHECKED), NULL);
	SendMessage(hwndEngineSkyPositionCheckbox, BM_SETCHECK, (WPARAM)(wm.HasPatchedSkyPosition() ? BST_CHECKED : BST_UNCHECKED), NULL);
	SendMessage(hwndEngineNightSkyOpacityCheckbox, BM_SETCHECK, (WPARAM)(wm.HasPatchedNightSkyOpacity() ? BST_CHECKED : BST_UNCHECKED), NULL);
	SendMessage(hwndSnapPlayerToGroundNormalCheckbox, BM_SETCHECK, (WPARAM)(wm.HasPatchedSnapPlayerToGroundNormal() ? BST_CHECKED : BST_UNCHECKED), NULL);

	float pos = wm.GetPlayer()->GetCommentatorCameraSpeed();
	SendMessage(hwndCommentatorSpeedSlider, TBM_SETPOS, TRUE, (LPARAM)(int)floor(pos*100.0f));
	TCHAR *tmp = new TCHAR[30];
	ZeroMemory(tmp, 30);
	_stprintf(tmp, _T("Speed: %.02f"), pos);
	SendMessage(hwndCommentatorSpeedStatic, WM_SETTEXT, NULL, (LPARAM)tmp);

	pos = wm.GetCamera()->GetFieldOfView();
	SendMessage(hwndCameraFOVSlider, TBM_SETPOS, TRUE, (LPARAM)(int)floor(pos*100.0f));
	ZeroMemory(tmp, 30);
	_stprintf(tmp, _T("Field of view: %.02f�"), pos);
	SendMessage(hwndCameraFOVStatic, WM_SETTEXT, NULL, (LPARAM)tmp);

	float pos2 = wm.GetEngine()->GetAnimationSpeed();
	SendMessage(hwndEngineAnimationSpeedSlider, TBM_SETPOS, TRUE, (LPARAM)(int)floor(pos2));
	ZeroMemory(tmp, 30);
	_stprintf(tmp, _T("Animation Speed: %.f"), pos2);
	SendMessage(hwndEngineAnimationSpeedStatic, WM_SETTEXT, NULL, (LPARAM)tmp);

	float pos3 = wm.GetEngine()->GetSkyPosition();
	SendMessage(hwndEngineSkyPositionSlider, TBM_SETPOS, TRUE, (LPARAM)(int)floor(pos3));
	ZeroMemory(tmp, 30);
	_stprintf(tmp, _T("Sky Position: %.02f"), pos3 * (24.f / 1440.f));
	SendMessage(hwndEngineSkyPositionCheckbox, WM_SETTEXT, NULL, (LPARAM)tmp);

	BYTE pos4 = wm.GetEngine()->GetNightSkyOpacity();
	SendMessage(hwndEngineNightSkyOpacitySlider, TBM_SETPOS, TRUE, (LPARAM)(int)pos4);
	ZeroMemory(tmp, 30);
	_stprintf(tmp, _T("Night Sky Opacity: %u"), pos4);
	SendMessage(hwndEngineNightSkyOpacityCheckbox, WM_SETTEXT, NULL, (LPARAM)tmp);

	delete[] tmp;
	
	if( !wm.HasPatchedSkyPosition() )
		EnableWindow(hwndEngineSkyPositionSlider, FALSE);
	if( !wm.HasPatchedNightSkyOpacity() )
		EnableWindow(hwndEngineNightSkyOpacitySlider, FALSE);

	return FALSE;
}

// Function called when the main window receives a system command (right click of the titlebar for instance)
LRESULT CALLBACK HandleMainWindowSysCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	// Main titlebar right click menu "Always on top"
	case HMENU_MAIN_WINDOW_SYSMENU_TOPMOST:
		{
			LONG dwStyle = GetWindowLong(hwndMain, GWL_EXSTYLE);
			if( dwStyle & WS_EX_TOPMOST )
			{
				// Grab the menu for the titlebar,
				// Set our custom item to unchecked,
				// And update the window via moving it nowhere
				HMENU hMenu = GetSystemMenu(hwndMain, FALSE);
				CheckMenuItem(hMenu, HMENU_MAIN_WINDOW_SYSMENU_TOPMOST, MF_BYCOMMAND|MF_UNCHECKED);
				SetWindowPos(hwndMain, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}
			else
			{
				// Grab the menu for the titlebar,
				// Set our custom item to checked,
				// And update the window via moving it nowhere
				HMENU hMenu = GetSystemMenu(hwndMain, FALSE);
				CheckMenuItem(hMenu, HMENU_MAIN_WINDOW_SYSMENU_TOPMOST, MF_BYCOMMAND|MF_CHECKED);
				SetWindowPos(hwndMain, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}
		}
		break;

	// Main titlebar right click menu "Do a barrel roll!"
	case HMENU_MAIN_WINDOW_SYSMENU_EASTEREGG:
		{
			float OriginalCameraRoll = wm.GetCamera()->GetRoll();
			for( int x = 0; x < 60; x++ )
			{
				wm.GetCamera()->SetRoll(wm.GetCamera()->GetRoll() + (360.0f/60.0f));
				Sleep(1000/60);
			}
			wm.GetCamera()->SetRoll(OriginalCameraRoll);
		}
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return FALSE;
}

// Function called when the main windows receives a command
LRESULT CALLBACK HandleMainWindowCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// LOWORD(wParam) is the target UI element
	switch(LOWORD(wParam))
	{
	case HMENU_COMMENTATOR_CHECKBOX:
		{
			if( !wm.IsAttached() )
			{
				MessageBox(NULL, _T("WoWManager is not attached to WoW!"), _T("Error!"), MB_ICONERROR|MB_OK);
				break;
			}

			// Toggle commentator mode
			if( !wm.GetPlayer()->SetCommentatorMode(SendMessage(hwndCommentatorCheckbox, BM_GETCHECK, (WPARAM)NULL, (LPARAM)NULL) == BST_CHECKED) )
				MessageBox(NULL, _T("Cannot Toggle Commentator Mode!"), _T("Error!"), MB_ICONERROR|MB_OK);
		}
		break;

	case HMENU_COMMENTATOR_COLLISION_CHECKBOX:
		{
			if( !wm.IsAttached() )
			{
				MessageBox(NULL, _T("WoWManager is not attached to WoW!"), _T("Error!"), MB_ICONERROR|MB_OK);
				break;
			}

			// Toggle commentator mode's camera collision
			if( !wm.GetPlayer()->SetCommentatorCameraCollision(SendMessage(hwndCommentatorCollisionCheckbox, BM_GETCHECK, (WPARAM)NULL, (LPARAM)NULL) == BST_CHECKED) )
				MessageBox(NULL, _T("Cannot Toggle Commentator Mode's Collision!"), _T("Error!"), MB_ICONERROR|MB_OK);
		}
		break;

	case HMENU_TELEPORT_FORWARD_BUTTON:
		{
			if( !wm.IsAttached() )
			{
				MessageBox(NULL, _T("WoWManager is not attached to WoW!"), _T("Error!"), MB_ICONERROR|MB_OK);
				break;
			}

			// Teleport forward by 10 units.
			Vec4 pos = wm.GetPlayer()->GetPosition();
			pos.X += 10.0f * cos(pos.O);
			pos.Y += 10.0f * sin(pos.O);
			if( !wm.GetPlayer()->SetPosition(pos) )
			{
				MessageBox(NULL, _T("Failed to set position!"), _T("Error!"), MB_ICONERROR|MB_OK);
				break;
			}
		}
		break;

	case HMENU_RENDER_WIREFRAME_CHECKBOX:
		{
			if( !wm.IsAttached() )
			{
				MessageBox(NULL, _T("WoWManager is not attached to WoW!"), _T("Error!"), MB_ICONERROR|MB_OK);
				break;
			}

			// Set the render flags to toggle wireframe
			if( SendMessage(hwndWireframeCheckbox, BM_GETCHECK, (WPARAM)NULL, (LPARAM)NULL) == BST_CHECKED )
			{
				if( !wm.GetEngine()->SetRenderingFlags(RENDER_FLAG_WIREFRAME) )
				{
					MessageBox(NULL, _T("Failed to toggle wireframe"), _T("Error!"), MB_ICONERROR|MB_OK);
					break;
				}
				
				if( !wm.GetEngine()->DrawSky(false) )
				{
					MessageBox(NULL, _T("Failed to toggle drawSky"), _T("Error!"), MB_ICONERROR|MB_OK);
					break;
				}
			}
			else
			{
				if( !wm.GetEngine()->RemoveRenderingFlags(RENDER_FLAG_WIREFRAME) )
				{
					MessageBox(NULL, _T("Failed to toggle wireframe"), _T("Error!"), MB_ICONERROR|MB_OK);
					break;
				}

				if( !wm.GetEngine()->DrawSky(true) )
				{
					MessageBox(NULL, _T("Failed to toggle drawSky"), _T("Error!"), MB_ICONERROR|MB_OK);
					break;
				}
			}
		}
		break;

	case HMENU_ENGINE_SKY_POSITION_CHECKBOX:
		{
			if( !wm.IsAttached() )
			{
				MessageBox(NULL, _T("WoWManager is not attached to WoW!"), _T("Error!"), MB_ICONERROR|MB_OK);
				break;
			}

			// Patch the sky position code
			if( SendMessage(hwndEngineSkyPositionCheckbox, BM_GETCHECK, (WPARAM)NULL, (LPARAM)NULL) == BST_CHECKED )
			{
				if( !wm.PatchSkyPosition() )
				{
					MessageBox(NULL, _T("Failed to enable sky position"), _T("Error!"), MB_ICONERROR|MB_OK);
					break;
				}
				EnableWindow(hwndEngineSkyPositionSlider, TRUE);
			}
			else
			{
				if( !wm.DepatchSkyPosition() )
				{
					MessageBox(NULL, _T("Failed to disable sky position"), _T("Error!"), MB_ICONERROR|MB_OK);
					break;
				}
				EnableWindow(hwndEngineSkyPositionSlider, FALSE);
			}
		}
		break;

	case HMENU_ENGINE_NIGHT_SKY_OPACITY_CHECKBOX:
		{
			if( !wm.IsAttached() )
			{
				MessageBox(NULL, _T("WoWManager is not attached to WoW!"), _T("Error!"), MB_ICONERROR|MB_OK);
				break;
			}

			if( SendMessage(hwndEngineNightSkyOpacityCheckbox, BM_GETCHECK, (WPARAM)NULL, (LPARAM)NULL) == BST_CHECKED )
			{
				if( !wm.PatchNightSkyOpacity() )
				{
					MessageBox(NULL, _T("Failed to enable night sky opacity"), _T("Error!"), MB_ICONERROR|MB_OK);
					break;
				}
				EnableWindow(hwndEngineNightSkyOpacitySlider, TRUE);
			}
			else
			{
				if( !wm.DepatchNightSkyOpacity() )
				{
					MessageBox(NULL, _T("Failed to disable night sky opacity"), _T("Error!"), MB_ICONERROR|MB_OK);
					break;
				}
				EnableWindow(hwndEngineNightSkyOpacitySlider, FALSE);
			}
		}
		break;

	case HMENU_ENGINE_SNAP_PLAYER_TO_GROUND_NORMAL_CHECKBOX:
		{
			if( !wm.IsAttached() )
			{
				MessageBox(NULL, _T("WoWManager is not attached to WoW!"), _T("Error!"), MB_ICONERROR|MB_OK);
				break;
			}

			if( SendMessage(hwndSnapPlayerToGroundNormalCheckbox, BM_GETCHECK, (WPARAM)NULL, (LPARAM)NULL) == BST_CHECKED )
			{
				if( !wm.PatchSnapPlayerToGroundNormal() )
				{
					MessageBox(NULL, _T("Failed to force player to snap to ground normal"), _T("Error!"), MB_ICONERROR|MB_OK);
					break;
				}
			}
			else
			{
				if( !wm.DepatchSnapPlayerToGroundNormal() )
				{
					MessageBox(NULL, _T("Failed to un-force player to snap to ground normal"), _T("Error!"), MB_ICONERROR|MB_OK);
					break;
				}
			}
		}
		break;
	}
	return FALSE;
}

// Function called when the user does a horizontal scroll
LRESULT CALLBACK HandleMainWindowHScroll(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	// Track when the user drags it with the mouse
	case SB_LINELEFT:
	case SB_LINERIGHT:
	// Track when the user presses page up/down
	case SB_PAGERIGHT:
	case SB_PAGELEFT:
	// Track when the user press left/right
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		{
			if( (HWND)lParam == hwndCameraFOVSlider )
			{
				if( !wm.IsAttached() )
				{
					MessageBox(NULL, _T("WoWManager is not attached to WoW!"), _T("Error!"), MB_ICONERROR|MB_OK);
					break;
				}

				// Set the field of view to the position of the slider and update the text to match
				float pos = (float)SendMessage(hwndCameraFOVSlider, TBM_GETPOS, 0, 0);
				wm.GetCamera()->SetFieldOfView(pos/100.0f);
				TCHAR *tmp = new TCHAR[30];
				ZeroMemory(tmp, 30);
				_stprintf(tmp, _T("Field of view: %.02f�"), pos/100.0f);
				SendMessage(hwndCameraFOVStatic, WM_SETTEXT, NULL, (LPARAM)tmp);
				delete[] tmp;
			}
			else if( (HWND)lParam == hwndCommentatorSpeedSlider )
			{
				if( !wm.IsAttached() )
				{
					MessageBox(NULL, _T("WoWManager is not attached to WoW!"), _T("Error!"), MB_ICONERROR|MB_OK);
					break;
				}

				// Set the Commentator speed to the position of the slider and update the text to match
				float pos = (float)SendMessage(hwndCommentatorSpeedSlider, TBM_GETPOS, 0, 0);
				wm.GetPlayer()->SetCommentatorCameraSpeed(pos/100.0f);
				TCHAR *tmp = new TCHAR[30];
				ZeroMemory(tmp, 30);
				_stprintf(tmp, _T("Speed: %.02f"), pos/100.0f);
				SendMessage(hwndCommentatorSpeedStatic, WM_SETTEXT, NULL, (LPARAM)tmp);
				delete[] tmp;
			}
			else if( (HWND)lParam == hwndEngineAnimationSpeedSlider )
			{
				if( !wm.IsAttached() )
				{
					MessageBox(NULL, _T("WoWManager is not attached to WoW!"), _T("Error!"), MB_ICONERROR|MB_OK);
					break;
				}

				// Set the animation speed of models to the position of the slider and update the text to match
				float pos = (float)SendMessage(hwndEngineAnimationSpeedSlider, TBM_GETPOS, 0, 0);
				if( !wm.GetEngine()->SetAnimationSpeed(pos) )
					MessageBox(NULL, _T("Error"), _T(""), MB_ICONERROR|MB_OK);
				TCHAR *tmp = new TCHAR[30];
				ZeroMemory(tmp, 30);
				_stprintf(tmp, _T("Animation Speed: %.f"), pos);
				SendMessage(hwndEngineAnimationSpeedStatic, WM_SETTEXT, NULL, (LPARAM)tmp);
				delete[] tmp;
			}
			else if( (HWND)lParam == hwndEngineSkyPositionSlider )
			{
				if( !wm.IsAttached() )
				{
					MessageBox(NULL, _T("WoWManager is not attached to WoW!"), _T("Error!"), MB_ICONERROR|MB_OK);
					break;
				}

				// Set the sky position to the position of the slider and update the text to match
				float pos = (float)SendMessage(hwndEngineSkyPositionSlider, TBM_GETPOS, 0, 0);
				if( !wm.GetEngine()->SetSkyPosition(pos) )
					MessageBox(NULL, _T("Error"), _T(""), MB_ICONERROR|MB_OK);
				TCHAR *tmp = new TCHAR[30];
				ZeroMemory(tmp, 30);
				_stprintf(tmp, _T("Sky Position: %.02f"), pos * (24.f / 1440.f));
				SendMessage(hwndEngineSkyPositionCheckbox, WM_SETTEXT, NULL, (LPARAM)tmp);
				delete[] tmp;
			}
			else if( (HWND)lParam == hwndEngineNightSkyOpacitySlider )
			{
				if( !wm.IsAttached() )
				{
					MessageBox(NULL, _T("WoWManager is not attached to WoW!"), _T("Error!"), MB_ICONERROR|MB_OK);
					break;
				}

				// Set the opacity of the night sky to the position of the slider and update the text to match
				BYTE pos = (BYTE)SendMessage(hwndEngineNightSkyOpacitySlider, TBM_GETPOS, 0, 0);
				if( !wm.GetEngine()->SetNightSkyOpacity(pos) )
					MessageBox(NULL, _T("Error"), _T(""), MB_ICONERROR|MB_OK);
				TCHAR *tmp = new TCHAR[30];
				ZeroMemory(tmp, 30);
				_stprintf(tmp, _T("Night Sky Opacity: %u"), pos);
				SendMessage(hwndEngineNightSkyOpacityCheckbox, WM_SETTEXT, NULL, (LPARAM)tmp);
				delete[] tmp;

			}
		}
		break;
	}
	return FALSE;
}

// Function called before a menu is shown, allowing us time to modify it
LRESULT CALLBACK HandleMainWindowInitMenuPopup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HMENU hMenu = GetSystemMenu(hwndMain, FALSE);
	if( (HMENU)wParam == hMenu )
	{
		RemoveMenu(hMenu, HMENU_MAIN_WINDOW_SYSMENU_EASTEREGG, MF_BYCOMMAND);
		if( (GetKeyState(VK_CONTROL) & 0x8000) ) // 0x8000 is 1000000000000000 in binary, which is the high-order bit of 1 - signifying that Ctrl is pressed
		{
			AppendMenu(hMenu, MF_STRING, HMENU_MAIN_WINDOW_SYSMENU_EASTEREGG, _T("Do a barrel roll!"));
		}
	}

	return FALSE;
}