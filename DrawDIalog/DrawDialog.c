#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("���� �׸���");

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
		800,
		// ������ ���� ũ��
		600,
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
	switch (iMessage) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_LBUTTONDOWN:
		DialogBox(g_hInst, MAKEINTRESOURCE(IDD_SELECT_DRAW), hWnd, DialogProc);
		break;
	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

typedef enum _DrawType {
	NONE = -1,
	SQUARE = 0,
	CIRCLE = 1,
} DrawType;

INT_PTR CALLBACK DialogProc(HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam) {
	static DrawType drawType;
	PAINTSTRUCT ps;
	HDC dc;

	switch (message) {
	case WM_INITDIALOG:
		drawType = NONE;
		CheckRadioButton(hDialog, IDC_SQUARE, IDC_CIRCLE, NULL);
		return (INT_PTR)TRUE;

	case WM_PAINT:
		dc = BeginPaint(hDialog, &ps);

		if (drawType == SQUARE) {
			Rectangle(dc, 50, 50, 150, 150);
		}
		else if (drawType == CIRCLE) {
			Ellipse(dc, 50, 50, 150, 150);
		}

		EndPaint(hDialog, &ps);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_SQUARE:
			drawType = SQUARE;
			InvalidateRect(hDialog, NULL, TRUE);
			break;

		case IDC_CIRCLE:
			drawType = CIRCLE;
			InvalidateRect(hDialog, NULL, TRUE);
			break;

		case IDCANCEL:
			EndDialog(hDialog, 0);
			break;
		}

		break;
	}

	return (INT_PTR)FALSE;
}
