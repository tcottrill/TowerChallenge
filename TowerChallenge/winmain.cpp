
// Includes
#define NOMINMAX
#include <windows.h>
#include "game.h"
#include "gl_basics.h"
#include "os_basic.h"
#include "framework.h"
#include "log.h"
#include "rawinput.h"
#include <cstdlib> 

using namespace std;

//Globals
HWND hWnd;
int SCREEN_W = 640;
int SCREEN_H = 940;

// Function Declarations
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


BOOL CenterWindow(HWND hwndWindow)
{
	HWND hwndParent;
	RECT rectWindow, rectParent;

	// Make the window relative to its parent
	if ((hwndParent = GetParent(hwndWindow)) != NULL)
	{
		GetWindowRect(hwndWindow, &rectWindow);
		GetWindowRect(hwndParent, &rectParent);

		int nWidth = rectWindow.right - rectWindow.left;
		int nHeight = rectWindow.bottom - rectWindow.top;

		int nX = ((rectParent.right - rectParent.left) - nWidth) / 2 + rectParent.left;
		int nY = ((rectParent.bottom - rectParent.top) - nHeight) / 2 + rectParent.top;

		int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

		// Make sure that the dialog box never moves outside of the screen
		if (nX < 0) nX = 0;
		if (nY < 0) nY = 0;
		if (nX + nWidth > nScreenWidth) nX = nScreenWidth - nWidth;
		if (nY + nHeight > nScreenHeight) nY = nScreenHeight - nHeight;

		MoveWindow(hwndWindow, nX, nY, nWidth, nHeight, FALSE);

		return TRUE;
	}

	return FALSE;
}


//========================================================================
// Popup a Windows Error Message, Allegro Style
//========================================================================
void allegro_message(const char* title, const char* message)
{
	MessageBoxA(NULL, message, title, MB_ICONEXCLAMATION | MB_OK);
}

//========================================================================
// Return the Window Handle
//========================================================================
HWND win_get_window(void)
{
	return hWnd;
}


int KeyCheck(int keynum)
{
	static int keys[256];

	static int hasrun = 0;
	int i;

	if (hasrun == 0)
	{
		for (i = 0; i < 256; i++)
		{
			keys[i] = 0;
		}
		hasrun = 1;
	}
	if (!keys[keynum] && key[keynum]) // Return True if not in que
	{
		keys[keynum] = 1;
		return 1;
	}
	else if (keys[keynum] && !key[keynum]) // Return False if in que
		keys[keynum] = 0;
	return 0;
}


bool AttachDebugConsole(void)
{
	if (FreeConsole())
	{
		if (!AllocConsole())
			return false;
	}
	else if (!AllocConsole())
		return false;
	SetForegroundWindow(hWnd);
	SetConsoleTitleA("Debug Console");
	const SMALL_RECT windowSize = { 0,0,79,24 };
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	// You chould check here first to see if the console is too big for the window size.
	if (!SetConsoleWindowInfo(hConsole, TRUE, &windowSize))
	{
		wrlog("SetConsoleWindowInfo Failed - Error (%d)\n", GetLastError());
	}
	printf("Debug window started\n");
	FILE* pConOutput = nullptr;
	return  (freopen_s(&pConOutput, "CONOUT$", "w", stdout) == 0);
}



// WinMain
BOOL EnableCloseButton(const HWND hwnd, const BOOL bState)
{
	HMENU   hMenu;
	UINT    dwExtra;

	if (hwnd == NULL) return FALSE;
	if ((hMenu = GetSystemMenu(hwnd, FALSE)) == NULL) return FALSE;
	dwExtra = bState ? MF_ENABLED : (MF_DISABLED | MF_GRAYED);
	return EnableMenuItem(hMenu, SC_CLOSE, MF_BYCOMMAND | dwExtra) != -1;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int iCmdShow)
{
	WNDCLASS wc;
	MSG msg;
	BOOL quit = FALSE;

	// Register window class
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "GLSample";
	RegisterClass(&wc);

	// This has to be set BEFORE Window creation on Windows 10. This enables DPI Scaling support on Windows 10.
	// BOOL REZ = SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);

	// Create main window
	hWnd = CreateWindow("GLSample", "OpenGL Sample", WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE, 0, 0, SCREEN_W, SCREEN_H, NULL, NULL, hInstance, NULL);

	///////////////// Initialize everything here //////////////////////////////
	LogOpen("testlog.txt");
	wrlog("Opening Log");

	// RESIZE WINDOW TO GL SIZE!
	ClientResize(SCREEN_W, SCREEN_H);
	CreateGLContext();
	ViewOrtho(SCREEN_W, SCREEN_H);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	HRESULT i = RawInput_Initialize(hWnd);
	// Enable Debug Window
	// AttachDebugConsole();
	// Enable vSync
	SetvSync(1);

	init_game();

	// Program main loop
	while (!quit)
	{
		// Check for messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// handle or dispatch messages
			if (msg.message == WM_QUIT)
			{
				quit = TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

			//Check if we want to exit;
			if (KeyCheck(KEY_ESC)) { quit = TRUE; }

			run_game();

			glSwap();
		}
	}

	end_game();

	// shutdown OpenGL
	DeleteGLContext();

	wrlog("Closing Log");
	LogClose();
	// Destroy the window explicitly
	DestroyWindow(hWnd);
	return 0;//return (int)msg.wParam;
}

// Window Procedure

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{

	case WM_CREATE:
		return 0;

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_INPUT: {return RawInput_ProcessInput(hWnd, wParam, lParam); return 0; }

	case WM_DESTROY:
		return 0;

	case WM_SYSCOMMAND:
	{
		switch (wParam & 0xfff0)
		{
		case SC_SCREENSAVE:
		case SC_MONITORPOWER:
		{
			return 0;
		}
		/*
		case SC_CLOSE:
		{
			//I can add a close hook here to trap close button
			quit = 1;
			PostQuitMessage(0);
			break;
		}
		*/
		if (SC_MINIMIZE)
		{
			//Application is minimized
			return 0;
		}
		break;

		// User trying to access application menu using ALT?
		case SC_KEYMENU:
			return 0;
		}
		DefWindowProc(hWnd, message, wParam, lParam);
	}
	case WM_KEYDOWN:
		switch (wParam)
		{

		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;

		}
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
