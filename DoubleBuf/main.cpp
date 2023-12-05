#include <Windows.h>
#include <stdio.h>
#include <tchar.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
VOID InitBitmap(const INT, HBITMAP*, LPBITMAP);

HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("���� ���۸�");

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
