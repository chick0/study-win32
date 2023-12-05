#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("��Ʈ�� �ݺ�");

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
		// ���� ������ Ŭ���� �̸�
		lpszClass,
		// ������ Ÿ��Ʋ�� �ؽ�Ʈ
		lpszClass,
		// ������ ��Ÿ��
		WS_OVERLAPPEDWINDOW,
		// ������ x��ǥ
		CW_USEDEFAULT,
		// ������ y��ǥ
		CW_USEDEFAULT,
		// ������ ���� ũ��
		1280,
		// ������ ���� ũ��
		720,
		// �θ� ������ �ڵ�, �θ� �����찡 ������ NULL
		NULL,
		// �޴� �ڵ�, ������ NULL
		(HMENU)NULL,
		// ���α׷� �ڵ�
		hInstance,
		// ���� ������ ����, �Ϲ������� NULL
		NULL
	);

	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	return (int)Message.wParam;
}

VOID InitBitmap(const INT bitmapId, HBITMAP *lpHbit, LPBITMAP lpBit) {
	*lpHbit = LoadBitmap(g_hInst, MAKEINTRESOURCE(bitmapId));
	GetObject(*lpHbit, sizeof(BITMAP), lpBit);
}

VOID DrawBitmap(HDC hdc, INT x, INT y, HBITMAP hBit, LPRECT rect) {
	HDC MemDC;
	HBITMAP old;
	BITMAP bm;

	MemDC = CreateCompatibleDC(hdc);
	old = (HBITMAP)SelectObject(MemDC, hBit);

	GetObject(hBit, sizeof(BITMAP), &bm);

	if (rect == NULL) {
		BitBlt(hdc, x, y, bm.bmWidth, bm.bmHeight, MemDC, 0, 0, SRCCOPY);
	}
	else {
		StretchBlt(hdc, 0, 0, rect->right, rect->bottom, MemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
	}

	SelectObject(MemDC, old);
	DeleteDC(MemDC);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP cat;
	static BITMAP bm;
	static RECT client;

	HDC hdc;
	PAINTSTRUCT ps;

	switch (iMessage) {
	case WM_CREATE:
		InitBitmap(IDB_BITMAP_CAT, &cat, &bm);
		GetClientRect(hWnd, &client);
		break;

	case WM_DESTROY:
		DeleteObject(cat);
		PostQuitMessage(0);
		return 0;

	case WM_SIZE:
		GetClientRect(hWnd, &client);
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		for (INT i = 0; i <= client.right / bm.bmWidth; i++) {
			for (INT j = 0; j <= client.bottom / bm.bmHeight; j++) {
				DrawBitmap(hdc, i * bm.bmWidth, j * bm.bmHeight, cat, NULL);
			}
		}

		EndPaint(hWnd, &ps);
		break;
	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}
