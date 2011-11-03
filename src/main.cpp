#include "main.h"

HWND hwndMain, hwndSpectateCheckbox, hwndSpectateCollisionCheckbox;
WoWManager *wm;

BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX	wc;
	MSG			Msg;

	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = MainWindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(hInstance, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = _T("WoWCToolApp");
	wc.hIconSm       = (HICON)LoadImage(hInstance, IDI_APPLICATION, IMAGE_ICON, 16, 16, 0);

	if( !RegisterClassEx(&wc) )
	{
		MessageBox( NULL, _T("Window Registration Failed!"), _T("Error!"), MB_ICONERROR | MB_OK );
		return 0;
	}

	int width = 250;
	int height = 100;
	hwndMain = CreateWindowEx( NULL,
		_T("WoWCToolApp"), _T("WoW Camera Tool"),
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		((GetSystemMetrics(SM_CXSCREEN)/2)-(width/2)), ((GetSystemMetrics(SM_CYSCREEN)/2)-(height/2)), // center
		width, height,
		NULL, NULL, hInstance, NULL );

	if( !hwndMain )
	{
		MessageBox( NULL, _T("Window Creation Failed!"), _T("Error!"), MB_ICONERROR | MB_OK );
		return 0;
	}

	// Create a new instance of the WoWManager, which will also give us debug permissions
	wm = new WoWManager();

	ShowWindow( hwndMain, nCmdShow );
	UpdateWindow( hwndMain );
	BOOL bRet;
	while( (bRet = GetMessage(&Msg, NULL, 0, 0)) != 0 )
	{
		if( bRet == -1 )
			break;

		TranslateMessage( &Msg );
		DispatchMessage( &Msg );
	}

	return Msg.wParam;
}

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_CREATE:
		{
			hwndSpectateCheckbox = CreateWindowEx(NULL,
				_T("Button"), _T("Spectate"), WS_CHILD | WS_VISIBLE | BS_TEXT | BS_AUTOCHECKBOX,
				10, 5, 70, 23, hwnd, (HMENU)111, NULL, NULL);

			hwndSpectateCollisionCheckbox = CreateWindowEx(NULL,
				_T("Button"), _T("Collision"), WS_CHILD | WS_VISIBLE | BS_TEXT | BS_AUTOCHECKBOX,
				10, 30, 70, 23, hwnd, (HMENU)112, NULL, NULL);

			HFONT hfFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
			SendMessage(hwndSpectateCheckbox, WM_SETFONT, (WPARAM)hfFont, TRUE);
			SendMessage(hwndSpectateCollisionCheckbox, WM_SETFONT, (WPARAM)hfFont, TRUE);
		}
		break;

	case WM_SHOWWINDOW:
		{
			// Find Wow.exe in memory
			HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
			HANDLE hProcess = INVALID_HANDLE_VALUE;
			PROCESSENTRY32 pe32;

			hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if( hProcessSnap == INVALID_HANDLE_VALUE )
				break;

			pe32.dwSize = sizeof(PROCESSENTRY32);
			if( !Process32First(hProcessSnap, &pe32) )
				break;

			do
			{
				if( !_tcscmp(pe32.szExeFile, _T("Wow.exe")) )
					break;
			} while( Process32Next(hProcessSnap, &pe32) );
			CloseHandle( hProcessSnap );

			// Attach WoWManager to Wow.exe
			if( !wm->Attach(pe32.th32ProcessID) )
			{
				delete wm;
				break;
			}

			// Read the game's memory, and fill in the values for it
			SendMessage(hwndSpectateCheckbox, BM_SETCHECK, (WPARAM)(wm->GetPlayer()->IsSpectating() ? BST_CHECKED : BST_UNCHECKED), NULL);
			SendMessage(hwndSpectateCollisionCheckbox, BM_SETCHECK, (WPARAM)(wm->GetPlayer()->IsSpectateModeCollidable() ? BST_CHECKED : BST_UNCHECKED), NULL);
		}
		break;

	case WM_COMMAND:
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
