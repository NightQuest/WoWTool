#include "PreCompiled.h"

// Main program function - called when the program starts
BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow)
{
	MSG Msg;
	BOOL bRet;

	// Register the window class
	if( !InitApplication(hInstance) )
		return FALSE;

	// Create the main window
	if (!InitInstance(hInstance, nCmdShow))
		return FALSE;

	// The main message loop
	while( (bRet = GetMessage(&Msg, NULL, 0, 0)) != 0 && bRet != -1 )
	{
		TranslateMessage( &Msg );
		DispatchMessage( &Msg );
	}

	return Msg.wParam;
}

// Initialize the window classes
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

// Initialize an instance of the program (windows and such)
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	int width = 265;
	int height = 200;

	// Create the main window
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

	// Show the window
	ShowWindow( hwndMain, nCmdShow );
	UpdateWindow( hwndMain );
	return TRUE;
}
