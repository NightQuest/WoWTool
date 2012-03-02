#include "PreCompiled.h"

// Proc for the main window (called whenever a message happens, pretty much)
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	// Called when the main windows gets created
	case WM_CREATE:
		HandleMainWindowCreate(hwnd, msg, wParam, lParam);
		break;

	// Called when the main window is shown via ShowWindow()
	case WM_SHOWWINDOW:
		HandleMainWindowShowWindow(hwnd, msg, wParam, lParam);
		break;

	// Called when the main window recieves a user command (Eg: Right-click)
	case WM_COMMAND:
		HandleMainWindowCommand(hwnd, msg, wParam, lParam);
		break;

	// Called when a horizontal scroll occurs in the main window
	case WM_HSCROLL:
		HandleMainWindowHScroll(hwnd, msg, wParam, lParam);
		break;

	// Called when a SYSCOMMAND happens (Eg: Right clicking the title bar)
	case WM_SYSCOMMAND:
		{
			// Main titlebar right click menu
			if( wParam == HMENU_MAIN_WINDOW_SYSMENU )
			{
				LONG dwStyle = GetWindowLong(hwndMain, GWL_EXSTYLE);
				if( dwStyle & WS_EX_TOPMOST )
				{
					// Grab the menu for the titlebar,
					// Set our custom item to unchecked,
					// And update the window via moving it nowhere
					HMENU hMenu = GetSystemMenu(hwndMain, FALSE);
					CheckMenuItem(hMenu, HMENU_MAIN_WINDOW_SYSMENU, MF_BYCOMMAND|MF_UNCHECKED);
					SetWindowPos(hwndMain, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				}
				else
				{
					// Grab the menu for the titlebar,
					// Set our custom item to checked,
					// And update the window via moving it nowhere
					HMENU hMenu = GetSystemMenu(hwndMain, FALSE);
					CheckMenuItem(hMenu, HMENU_MAIN_WINDOW_SYSMENU, MF_BYCOMMAND|MF_CHECKED);
					SetWindowPos(hwndMain, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				}
				return FALSE;
			}
			else
				DefWindowProc(hwnd, msg, wParam, lParam);
		}
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
	hwndCommentatorCheckbox = CreateWindowEx(NULL,
		_T("Button"), _T("Commentator Mode"), WS_CHILD | WS_VISIBLE | BS_TEXT | BS_AUTOCHECKBOX,
		10, 5, 120, 23, hwnd, (HMENU)HMENU_COMMENTATOR_CHECKBOX, NULL, NULL);

	hwndCommentatorCollisionCheckbox = CreateWindowEx(NULL,
		_T("Button"), _T("Collision"), WS_CHILD | WS_VISIBLE | BS_TEXT | BS_AUTOCHECKBOX,
		10, 30, 70, 23, hwnd, (HMENU)HMENU_COMMENTATOR_COLLISION_CHECKBOX, NULL, NULL);

	hwndTeleportForwardButton = CreateWindowEx(NULL,
		_T("Button"), _T("Teleport Forward"), WS_CHILD | WS_VISIBLE,
		140, 5, 100, 23, hwnd, (HMENU)HMENU_TELEPORT_FORWARD_BUTTON, NULL, NULL);

	hwndCommentatorSpeedStatic = CreateWindowEx(NULL,
		_T("Static"), _T("Speed"), WS_CHILD | WS_VISIBLE,
		10, 55, 110, 15, hwnd, (HMENU)HMENU_COMMENTATOR_SPEED_STATIC, NULL, NULL);

	hwndCommentatorSpeedSlider = CreateWindowEx(NULL,
		TRACKBAR_CLASS, _T(""), WS_CHILD | WS_VISIBLE | TBS_NOTICKS | TBS_ENABLESELRANGE,
		3, 70, 100, 23, hwnd, (HMENU)HMENU_COMMENTATOR_SPEED_SLIDER, NULL, NULL);

	hwndCameraFOVStatic = CreateWindowEx(NULL,
		_T("Static"), _T("Field of View"), WS_CHILD | WS_VISIBLE,
		140, 55, 110, 15, hwnd, (HMENU)HMENU_CAMERA_FOV_STATIC, NULL, NULL);

	hwndCameraFOVSlider = CreateWindowEx(NULL,
		TRACKBAR_CLASS, _T(""), WS_CHILD | WS_VISIBLE | TBS_NOTICKS | TBS_ENABLESELRANGE,
		135, 70, 100, 23, hwnd, (HMENU)HMENU_CAMERA_FOV_SLIDER, NULL, NULL);

	hwndWireframeCheckbox = CreateWindowEx(NULL,
		_T("Button"), _T("Wireframe"), WS_CHILD | WS_VISIBLE | BS_TEXT | BS_AUTOCHECKBOX,
		140, 95, 70, 23, hwnd, (HMENU)HMENU_RENDER_WIREFRAME_CHECKBOX, NULL, NULL);


	// Set the font of all the UI elements, so they won't have the default blocky look
	HFONT hfFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(hwndCommentatorCheckbox, WM_SETFONT, (WPARAM)hfFont, TRUE);
	SendMessage(hwndCommentatorCollisionCheckbox, WM_SETFONT, (WPARAM)hfFont, TRUE);
	SendMessage(hwndTeleportForwardButton, WM_SETFONT, (WPARAM)hfFont, TRUE);
	SendMessage(hwndCommentatorSpeedStatic, WM_SETFONT, (WPARAM)hfFont, TRUE);
	SendMessage(hwndCameraFOVStatic, WM_SETFONT, (WPARAM)hfFont, TRUE);
	SendMessage(hwndWireframeCheckbox, WM_SETFONT, (WPARAM)hfFont, TRUE);

	// Set the min/max of the Commentator speed slider to 1/100000
	// We use 100 times what the real value is on the max in order to allow decimal places
	SendMessage(hwndCommentatorSpeedSlider, TBM_SETRANGEMIN, TRUE, (LPARAM)1l);
	SendMessage(hwndCommentatorSpeedSlider, TBM_SETRANGEMAX, TRUE, (LPARAM)100000l);
    SendMessage(hwndCommentatorSpeedSlider, TBM_SETPAGESIZE, FALSE, 100);
    SendMessage(hwndCommentatorSpeedSlider, TBM_SETPOS, TRUE, 1);

	// Set the min/max of the field of view slider to 0/180
	// We use 100 times what the real value is on the max in order to allow decimal places
	SendMessage(hwndCameraFOVSlider, TBM_SETRANGEMIN, TRUE, (LPARAM)0l);
	SendMessage(hwndCameraFOVSlider, TBM_SETRANGEMAX, TRUE, (LPARAM)18000l);
    SendMessage(hwndCameraFOVSlider, TBM_SETPAGESIZE, 0, 100);
    SendMessage(hwndCameraFOVSlider, TBM_SETPOS, TRUE, 0);

	// Add "Always on top" to the main windows titlebar right click menu
	HMENU hMenu = GetSystemMenu(hwnd, FALSE);
	AppendMenu(hMenu, MF_STRING, HMENU_MAIN_WINDOW_SYSMENU, _T("Always on top"));

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
	if( !dwPID || !wm.Attach(dwPID) )
	{
		MessageBox(NULL, _T("Cannot attach to WoW!\r\nPlease make sure that WoW is running and fully logged in, and you're running this tool as Administrator!"), _T("Error!"), MB_ICONERROR | MB_OK);
		PostMessage(hwnd, WM_CLOSE, 0, 0);
		return FALSE;
	}

	// Read the game's memory, and fill in the values for it
	SendMessage(hwndCommentatorCheckbox, BM_SETCHECK, (WPARAM)(wm.GetPlayer()->IsInCommentatorMode() ? BST_CHECKED : BST_UNCHECKED), NULL);
	SendMessage(hwndCommentatorCollisionCheckbox, BM_SETCHECK, (WPARAM)(wm.GetPlayer()->IsCommentatorCameraCollidable() ? BST_CHECKED : BST_UNCHECKED), NULL);
	float pos = wm.GetPlayer()->GetCommentatorCameraSpeed();
	SendMessage(hwndCommentatorSpeedSlider, TBM_SETPOS, TRUE, (LPARAM)(int)floor(pos*100.0f));
	TCHAR *tmp = new TCHAR[30];
	ZeroMemory(tmp, 30);
	_stprintf(tmp, _T("Speed: %.02f"), pos);
	SendMessage(hwndCommentatorSpeedStatic, WM_SETTEXT, NULL, (LPARAM)tmp);

	pos = wm.GetCamera()->GetFieldOfView();
	SendMessage(hwndCameraFOVSlider, TBM_SETPOS, TRUE, (LPARAM)(int)floor(pos*100.0f));
	ZeroMemory(tmp, 30);
	_stprintf(tmp, _T("Field of view: %.02fº"), pos);
	SendMessage(hwndCameraFOVStatic, WM_SETTEXT, NULL, (LPARAM)tmp);
	delete[] tmp;

	SendMessage(hwndWireframeCheckbox, BM_SETCHECK, (WPARAM)((wm.GetRenderingFlags() & RENDER_FLAG_WIREFRAME) ? BST_CHECKED : BST_UNCHECKED), NULL);

	return FALSE;
}

// Function called when the main windows recieves a command
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

			DWORD flags = wm.GetRenderingFlags();
			if( SendMessage(hwndWireframeCheckbox, BM_GETCHECK, (WPARAM)NULL, (LPARAM)NULL) == BST_CHECKED )
				flags |= RENDER_FLAG_WIREFRAME;
			else
				flags &= ~RENDER_FLAG_WIREFRAME;

			if( !wm.SetRenderingFlags(flags) )
			{
				MessageBox(NULL, _T("Failed to toggle wireframe"), _T("Error!"), MB_ICONERROR|MB_OK);
				break;
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
				_stprintf(tmp, _T("Field of view: %.02fº"), pos/100.0f);
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
		}
		break;
	}
	return FALSE;
}