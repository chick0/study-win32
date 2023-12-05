#include <Windows.h>
#include <stdio.h>
#include <tchar.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
VOID InitBitmap(const INT, HBITMAP*, LPBITMAP);

HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("더블 버퍼링");

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
	WndClass.hCursor = LoadCursor(NULL, IDC_IBEAM);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(
		// 생성 윈도우 클래스 이름
		lpszClass,
		// 윈도우 타이틀바 텍스트
		lpszClass,
		// 윈도우 스타일
		WS_OVERLAPPEDWINDOW,
		// 윈도우 x좌표
		CW_USEDEFAULT,
		// 윈도우 y좌표
		CW_USEDEFAULT,
		// 윈도우 가로 크기
		1280,
		// 윈도우 세로 크기
		720,
		// 부모 윈도우 핸들, 부모 윈도우가 없으면 NULL
		NULL,
		// 메뉴 핸들, 없으면 NULL
		(HMENU)NULL,
		// 프로그램 핸들
		hInstance,
		// 생성 윈도우 정보, 일반적으로 NULL
		NULL
	);

	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	static RECT rtClient;

	switch (iMessage) {
	case WM_CREATE:
		GetClientRect(hWnd, &rtClient);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_SIZE:
		GetClientRect(hWnd, &rtClient);
		break;

	case WM_LBUTTONDOWN:
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_PAINT: {
		HDC hdc, memDC;
		PAINTSTRUCT ps;
		HBITMAP hBit, hBitOld;
		RECT bufferRt;

		hdc = BeginPaint(hWnd, &ps);

		GetClientRect(hWnd, &bufferRt);

		memDC = CreateCompatibleDC(hdc);

		hBit = CreateCompatibleBitmap(hdc, bufferRt.right, bufferRt.bottom);
		hBitOld = (HBITMAP)SelectObject(memDC, hBit);

		PatBlt(memDC, 0, 0, bufferRt.right, bufferRt.bottom, WHITENESS);

		for (INT i = 0; i < 30; i++) {
			Rectangle(
				memDC,
				rand() % rtClient.right,
				rand() % rtClient.bottom,
				rand() % rtClient.right,
				rand() % rtClient.bottom
			);

			Sleep(30);
		}

		BitBlt(hdc, 0, 0, bufferRt.right, bufferRt.bottom, memDC, 0, 0, SRCCOPY);
		SelectObject(hdc, hBitOld);

		DeleteObject(hBit);
		DeleteDC(memDC);

		EndPaint(hWnd, &ps);
		break;
	}

	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}
