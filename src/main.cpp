#include "main.h"

enum WindowIDs
{
	HMENU_MAIN_WINDOW_SYSMENU = 20,
	HMENU_MAIN_WINDOW = 110,
	HMENU_COMMENTATOR_CHECKBOX,
	HMENU_COMMENTATOR_COLLISION_CHECKBOX,
	HMENU_TELEPORT_FORWARD_BUTTON,
	HMENU_CAMERA_FOV_STATIC,
	HMENU_CAMERA_FOV_SLIDER,
};

HWND	hwndMain, hwndCommentatorCheckbox, hwndCommentatorCollisionCheckbox, hwndTeleportForwardButton,
		hwndCameraFOVStatic, hwndCameraFOVSlider;
WoWManager *wm;

BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow)
{
	MSG Msg;

	if( !InitApplication(hInstance) )
		return FALSE;

	if (!InitInstance(hInstance, nCmdShow))
		return FALSE;

	BOOL bRet;
	while( (bRet = GetMessage(&Msg, NULL, 0, 0)) != 0 && bRet != -1 )
	{
		TranslateMessage( &Msg );
		DispatchMessage( &Msg );
	}

	return Msg.wParam;
}

BOOL InitApplication(HINSTANCE hInstance)
{
	WNDCLASSEX	wcx;

	wcx.cbSize        = sizeof(WNDCLASSEX);
	wcx.style         = 0;
	wcx.lpfnWndProc   = MainWindowProc;
	wcx.cbClsExtra    = 0;
	wcx.cbWndExtra    = 0;
	wcx.hInstance     = hInstance;
	wcx.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAIN_ICON));
	wcx.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcx.lpszMenuName  = NULL;
	wcx.lpszClassName = _T("WoWToolApp");
	wcx.hIconSm       = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_MAIN_ICON), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);

	return RegisterClassEx(&wcx);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	int width = 250;
	int height = 200;
	hwndMain = CreateWindowEx(
		NULL,
		_T("WoWToolApp"),
		_T("WoW Tool"),
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		((GetSystemMetrics(SM_CXSCREEN)/2)-(width/2)),	// X center
		((GetSystemMetrics(SM_CYSCREEN)/2)-(height/2)),	// Y center
		width,
		height,
		NULL,
		NULL,
		hInstance,
		NULL );

	if( !hwndMain )
		return FALSE;

	// Create a new instance of the WoWManager, which will also give us debug permissions
	wm = new WoWManager();
	if( !wm )
	{
		MessageBox( NULL, _T("Cannot initialize WoWManager.\r\nPlease make sure you're running as Administrator."), _T("Error!"), MB_ICONERROR | MB_OK );
		return 0;
	}

	ShowWindow( hwndMain, nCmdShow );
	UpdateWindow( hwndMain );
	return TRUE;
}

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_CREATE:
		HandleMainWindowCreate(hwnd, msg, wParam, lParam);
		break;

	case WM_SHOWWINDOW:
		HandleMainWindowShowWindow(hwnd, msg, wParam, lParam);
		break;

	case WM_COMMAND:
		HandleMainWindowCommand(hwnd, msg, wParam, lParam);
		break;

	case WM_HSCROLL:
		HandleMainWindowHScroll(hwnd, msg, wParam, lParam);
		break;

	case WM_SYSCOMMAND:
		{
			if( wParam == HMENU_MAIN_WINDOW_SYSMENU )
			{
				LONG dwStyle = GetWindowLong(hwndMain, GWL_EXSTYLE);
				if( dwStyle & WS_EX_TOPMOST )
				{
					HMENU hMenu = GetSystemMenu(hwndMain, FALSE);
					CheckMenuItem(hMenu, HMENU_MAIN_WINDOW_SYSMENU, MF_BYCOMMAND|MF_UNCHECKED);
					SetWindowPos(hwndMain, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				}
				else
				{
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

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return FALSE;
}

LRESULT CALLBACK HandleMainWindowCreate(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	InitCommonControls();
	HRESULT hr = OleInitialize(NULL);
	if( hr != S_OK && hr != S_FALSE )
	{
		MessageBox(hwnd, _T("Failed to Initialize Shell!"), _T("Error!"), MB_ICONERROR | MB_OK);
		PostMessage(hwnd, WM_CLOSE, 0, 0);
	}

	hwndCommentatorCheckbox = CreateWindowEx(NULL,
		_T("Button"), _T("Commentator Mode"), WS_CHILD | WS_VISIBLE | BS_TEXT | BS_AUTOCHECKBOX,
		10, 5, 120, 23, hwnd, (HMENU)HMENU_COMMENTATOR_CHECKBOX, NULL, NULL);

	hwndCommentatorCollisionCheckbox = CreateWindowEx(NULL,
		_T("Button"), _T("Collision"), WS_CHILD | WS_VISIBLE | BS_TEXT | BS_AUTOCHECKBOX,
		10, 30, 70, 23, hwnd, (HMENU)HMENU_COMMENTATOR_COLLISION_CHECKBOX, NULL, NULL);

	hwndTeleportForwardButton = CreateWindowEx(NULL,
		_T("Button"), _T("Teleport Forward"), WS_CHILD | WS_VISIBLE,
		140, 5, 100, 23, hwnd, (HMENU)HMENU_TELEPORT_FORWARD_BUTTON, NULL, NULL);

	hwndCameraFOVStatic = CreateWindowEx(NULL,
		_T("Static"), _T("Field of View"), WS_CHILD | WS_VISIBLE,
		10, 55, 100, 23, hwnd, (HMENU)HMENU_CAMERA_FOV_STATIC, NULL, NULL);

	hwndCameraFOVSlider = CreateWindowEx(NULL,
		TRACKBAR_CLASS, _T(""), WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_ENABLESELRANGE,
		5, 70, 100, 23, hwnd, (HMENU)HMENU_CAMERA_FOV_SLIDER, NULL, NULL);


	HFONT hfFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(hwndCommentatorCheckbox, WM_SETFONT, (WPARAM)hfFont, TRUE);
	SendMessage(hwndCommentatorCollisionCheckbox, WM_SETFONT, (WPARAM)hfFont, TRUE);
	SendMessage(hwndTeleportForwardButton, WM_SETFONT, (WPARAM)hfFont, TRUE);
	SendMessage(hwndCameraFOVStatic, WM_SETFONT, (WPARAM)hfFont, TRUE);

	SendMessage(hwndCameraFOVSlider, TBM_SETRANGE, TRUE, MAKELONG(0, 180));
    SendMessage(hwndCameraFOVSlider, TBM_SETPAGESIZE, 0, 1);
    SendMessage(hwndCameraFOVSlider, TBM_SETSEL, FALSE, MAKELONG(0, 180));
    SendMessage(hwndCameraFOVSlider, TBM_SETPOS, TRUE, 0);

	HMENU hMenu = GetSystemMenu(hwnd, FALSE);
	AppendMenu(hMenu, MF_STRING, HMENU_MAIN_WINDOW_SYSMENU, _T("Always on top"));

	return FALSE;
}

LRESULT CALLBACK HandleMainWindowShowWindow(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Find Wow.exe in memory
	HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
	HANDLE hProcess = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 pe32;
	DWORD dwPID = 0;

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
	if( !dwPID || !wm->Attach(dwPID) )
	{
		delete wm;
		MessageBox(NULL, _T("Cannot attach to WoW!\r\nPlease make sure that WoW is running and fully logged in, and you're running this tool as Administrator!"), _T("Error!"), MB_ICONERROR | MB_OK);
		PostMessage(hwnd, WM_CLOSE, 0, 0);
		return FALSE;
	}

	// Read the game's memory, and fill in the values for it
	SendMessage(hwndCommentatorCheckbox, BM_SETCHECK, (WPARAM)(wm->GetPlayer()->IsInCommentatorMode() ? BST_CHECKED : BST_UNCHECKED), NULL);
	SendMessage(hwndCommentatorCollisionCheckbox, BM_SETCHECK, (WPARAM)(wm->GetPlayer()->IsCommentatorCameraCollidable() ? BST_CHECKED : BST_UNCHECKED), NULL);
	int pos = (int)wm->GetCamera()->GetFieldOfView();
	SendMessage(hwndCameraFOVSlider, TBM_SETPOS, TRUE, (LPARAM)pos);

	return FALSE;
}

LRESULT CALLBACK HandleMainWindowCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case HMENU_COMMENTATOR_CHECKBOX:
		{
			if( !wm )
			{
				MessageBox(NULL, _T("WoWManager is NULL!"), _T("Error!"), MB_ICONERROR|MB_OK);
				break;
			}

			// Toggle commentator mode
			if( !wm->GetPlayer()->SetCommentatorMode(SendMessage(hwndCommentatorCheckbox, BM_GETCHECK, (WPARAM)NULL, (LPARAM)NULL) == BST_CHECKED) )
				MessageBox(NULL, _T("Cannot Toggle Commentator Mode!"), _T("Error!"), MB_ICONERROR|MB_OK);
		}
		break;

	case HMENU_COMMENTATOR_COLLISION_CHECKBOX:
		{
			if( !wm )
			{
				MessageBox(NULL, _T("WoWManager is NULL!"), _T("Error!"), MB_ICONERROR|MB_OK);
				break;
			}

			// Toggle commentator mode's camera collision
			if( !wm->GetPlayer()->SetCommentatorCameraCollision(SendMessage(hwndCommentatorCollisionCheckbox, BM_GETCHECK, (WPARAM)NULL, (LPARAM)NULL) == BST_CHECKED) )
				MessageBox(NULL, _T("Cannot Toggle Commentator Mode's Collision!"), _T("Error!"), MB_ICONERROR|MB_OK);
		}
		break;

	case HMENU_TELEPORT_FORWARD_BUTTON:
		{
			if( !wm )
			{
				MessageBox(NULL, _T("WoWManager is NULL!"), _T("Error!"), MB_ICONERROR|MB_OK);
				break;
			}

			// Teleport forward by 10 units.
			Vec4 pos = wm->GetPlayer()->GetPosition();
			pos.X += 10.0f * cos(pos.O);
			pos.Y += 10.0f * sin(pos.O);
			if( !wm->GetPlayer()->SetPosition(pos) )
			{
				MessageBox(NULL, _T("Failed to set position!"), _T("Error!"), MB_ICONERROR|MB_OK);
				break;
			}
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK HandleMainWindowHScroll(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case SB_LINELEFT:
	case SB_LINERIGHT:
	case SB_PAGERIGHT:
	case SB_PAGELEFT:
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		{
			if( (HWND)lParam == hwndCameraFOVSlider )
			{
				if( !wm )
				{
					MessageBox(NULL, _T("WoWManager is NULL!"), _T("Error!"), MB_ICONERROR|MB_OK);
					break;
				}

				float pos = (float)SendMessage(hwndCameraFOVSlider, TBM_GETPOS, 0, 0);
				wm->GetCamera()->SetFieldOfView(pos);
			}
		}
		break;
	}
	return FALSE;
}