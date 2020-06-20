#include <iostream>

#include <Windows.h>
#include <gl/GL.h>

#pragma comment(lib, "Opengl32.lib")

HWND InitOpenGLWindow();
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void MainLoop(HWND hWnd);
void UpdateGL();

int main()
{
	HWND hWnd = InitOpenGLWindow();
	if (!hWnd)
		return -1;

	MainLoop(hWnd);
    return 0;
}

HWND InitOpenGLWindow()
{
	std::cout << "Initializing OpenGL Window." << std::endl;
	HINSTANCE hInst = GetModuleHandle(NULL);

	std::cout << "HINST: 0x" << std::hex << hInst << std::dec << std::endl;

	WNDCLASSEXW wcex;

	ZeroMemory(&wcex, sizeof(WNDCLASSEXW));

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInst;
	wcex.lpszClassName = L"WinOpenGL";
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClassExW(&wcex);

	HWND hWnd = CreateWindow(TEXT("WinOpenGL"), TEXT("Windows OpenGL Sample"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInst, NULL);

	std::cout << "HWND: 0x" << std::hex << hWnd << std::dec << std::endl;

	HDC hdc = GetDC(hWnd);
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;
	int iPixelFormat = ChoosePixelFormat(hdc, &pfd);
	if (!iPixelFormat)
	{
		std::cout << "ChoosePixelFormat Failed." << std::endl << "Error Code: " << GetLastError() << std::endl;
		DestroyWindow(hWnd);
		return NULL;
	}
	std::cout << "ChoosePixelFormat: " << iPixelFormat << std::endl;
	if (!SetPixelFormat(hdc, iPixelFormat, &pfd))
	{
		std::cout << "SetPixelFormat Failed." << std::endl << "Error Code: " << GetLastError() << std::endl;
		DestroyWindow(hWnd);
		return NULL;
	}
	HGLRC hrc = wglCreateContext(hdc);
	if (!hrc)
	{
		std::cout << "wglCreateContext Failed." << std::endl << "Error Code: " << GetLastError() << std::endl;
		DestroyWindow(hWnd);
		return NULL;
	}
	if (!wglMakeCurrent(hdc, hrc))
	{
		std::cout << "wglMakeCurrent Failed." << std::endl << "Error Code: " << GetLastError() << std::endl;
		wglDeleteContext(hrc);
		DestroyWindow(hWnd);
		return NULL;
	}

	std::cout << "GL_VENDOR: " << (char *)glGetString(GL_VENDOR) << std::endl;
	std::cout << "GL_RENDERER: " << (char*)glGetString(GL_RENDERER) << std::endl;
	std::cout << "GL_VERSION: " << (char*)glGetString(GL_VERSION) << std::endl;

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	return hWnd;
}

void MainLoop(HWND hWnd)
{
	MSG msg = {0};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		UpdateGL();
		SwapBuffers(GetDC(hWnd));
	}
}

void UpdateGL()
{
	glClearColor(0, 0, 0, 0);
	glClearDepth(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBegin(GL_TRIANGLES);
	glColor3f(1, 0, 0);
	glVertex3f(0, 0.5, 0);
	glColor3f(0, 1, 0);
	glVertex3f(0.5, -0.5, 0);
	glColor3f(0, 0, 1);
	glVertex3f(-0.5, -0.5, 0);
	glEnd();

	glFlush();
	glFinish();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);
		// Do Nothing.
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_SIZE:
		RECT rt;
		GetClientRect(hWnd, &rt);
		glViewport(0, 0, rt.right - rt.left, rt.bottom - rt.top);
		break;
	case WM_DESTROY:
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(wglGetCurrentContext());
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}