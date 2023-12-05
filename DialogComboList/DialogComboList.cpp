#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("단순 가입 탈퇴");

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
		800,
		// 윈도우 세로 크기
		600,
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
	switch (iMessage) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_LBUTTONDOWN:
		DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_INSERT_DELETE), hWnd, DialogProc);
		break;
	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

INT_PTR CALLBACK DialogProc(HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam) {
	static HWND hCombo;
	static INT selectIndex;
	TCHAR name[20];

	switch (message) {
	case WM_INITDIALOG:
		hCombo = GetDlgItem(hDialog, IDC_COMBO_LIST);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_INSERT:
			GetDlgItemText(hDialog, IDC_NAME, name, 20);

			if (lstrcmp(name, TEXT(""))) {
				SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)name);
				SetDlgItemText(hDialog, IDC_NAME, TEXT(""));
			}

			break;

		case IDC_DELETE:
			SendMessage(hCombo, CB_DELETESTRING, selectIndex, 0);
			break;

		case IDC_COMBO_LIST:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				selectIndex = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
			}
			break;

		case IDCANCEL:
			EndDialog(hDialog, 0);
			break;
		}

		break;
	}

	return (INT_PTR)FALSE;
}
