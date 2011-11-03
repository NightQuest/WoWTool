#include "main.h"

HWND hwndMain, hwndSpectateCheckbox, hwndSpectateCollisionCheckbox;
WoWManager *wm;

BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow)
{
	MSG Msg;

	if( !InitApplication(hInstance) )
		return FALSE;

	if (!InitInstance(hInstance, nCmdShow))
		return FALSE;

	// Create a new instance of the WoWManager, which will also give us debug permissions
	wm = new WoWManager();
	if( !wm )
	{
		MessageBox( NULL, _T("Cannot initialize WoWManager.\r\nPlease make sure you're running as Administrator."), _T("Error!"), MB_ICONERROR | MB_OK );
		return 0;
	}

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
	int height = 100;
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

	hwndSpectateCheckbox = CreateWindowEx(NULL,
		_T("Button"), _T("Spectate"), WS_CHILD | WS_VISIBLE | BS_TEXT | BS_AUTOCHECKBOX,
		10, 5, 70, 23, hwnd, (HMENU)111, NULL, NULL);

	hwndSpectateCollisionCheckbox = CreateWindowEx(NULL,
		_T("Button"), _T("Collision"), WS_CHILD | WS_VISIBLE | BS_TEXT | BS_AUTOCHECKBOX,
		10, 30, 70, 23, hwnd, (HMENU)112, NULL, NULL);

	HFONT hfFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(hwndSpectateCheckbox, WM_SETFONT, (WPARAM)hfFont, TRUE);
	SendMessage(hwndSpectateCollisionCheckbox, WM_SETFONT, (WPARAM)hfFont, TRUE);

	return FALSE;
}

LRESULT CALLBACK HandleMainWindowShowWindow(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Find Wow.exe in memory
	HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
	HANDLE hProcess = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 pe32;
	DWORD dwPID = 0;

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
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
		MessageBox(NULL, _T("Cannot attach to WoW!\r\nPlease make sure WoW is running and fully logged in, and you're running as Administrator!"), _T("Error!"), MB_ICONERROR | MB_OK);
		PostMessage(hwnd, WM_CLOSE, 0, 0);
		return FALSE;
	}

	// Read the game's memory, and fill in the values for it
	SendMessage(hwndSpectateCheckbox, BM_SETCHECK, (WPARAM)(wm->GetPlayer()->IsSpectating() ? BST_CHECKED : BST_UNCHECKED), NULL);
	SendMessage(hwndSpectateCollisionCheckbox, BM_SETCHECK, (WPARAM)(wm->GetPlayer()->IsSpectateModeCollidable() ? BST_CHECKED : BST_UNCHECKED), NULL);

	return FALSE;
}

LRESULT CALLBACK HandleMainWindowCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case 111:
		{
			if( !wm )
			{
				MessageBox(NULL, _T("WoWManager is NULL!"), _T("Error!"), MB_ICONERROR|MB_OK);
				break;
			}

			// Toggle spectate mode
			if( !wm->GetPlayer()->SetSpectateMode(!wm->GetPlayer()->IsSpectating()) )
				MessageBox(NULL, _T("Cannot Toggle Spectate Mode!"), _T("Error!"), MB_ICONERROR|MB_OK);
		}
		break;

	case 112:
		{
			if( !wm )
			{
				MessageBox(NULL, _T("WoWManager is NULL!"), _T("Error!"), MB_ICONERROR|MB_OK);
				break;
			}

			// Toggle spectate mode's camera collision
			if( !wm->GetPlayer()->SetSpectateCollision(!wm->GetPlayer()->IsSpectateModeCollidable()) )
				MessageBox(NULL, _T("Cannot Toggle Spectate Mode's Collision!"), _T("Error!"), MB_ICONERROR|MB_OK);
		}
		break;
	}
	return FALSE;
}