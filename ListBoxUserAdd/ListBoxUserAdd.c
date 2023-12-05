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
		DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_USER_UI), hWnd, DialogProc);
		break;
	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

typedef enum _Gender {
	G = 0,
	B = 1,
} Gender;

TCHAR GetGender(Gender g) {
	if (g == G) {
		return "여";
	}
	else if (g == B) {
		return "남";
	}
	else {
		return "기타";
	}
}

INT_PTR CALLBACK DialogProc(HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam) {
	static HWND hCombo;
	static INT selectIndex;
	TCHAR name[20];
	Gender gender;
	TCHAR phone[32];
	INT year = 0;

	TCHAR line[200];

	switch (message) {
	case WM_INITDIALOG:
		hCombo = GetDlgItem(hDialog, IDC_COMBO_YEAR);
		CheckRadioButton(hDialog, IDC_GENDER_G, IDC_GENDER_B, IDC_GENDER_G);

		for (INT i = 1970; i < 2010; i++) {
			TCHAR birthY[32];

			wsprintf(birthY, _T("%d"), i);
			SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)birthY);
		}

		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BUTTON_ADD:
			GetDlgItemText(hDialog, IDC_EDIT_NAME, name, 20);
			GetDlgItemText(hDialog, IDC_EDIT_PHONE, phone, 32);
			GetDlgItemInt(hDialog, IDC_COMBO_YEAR, year, 32);

			_stprintf_s(line, _T("%s; %s; %s; %d년"), name, phone, TEXT("기타"), year);

			if (lstrcmp(line, TEXT(""))) {
				SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)name);
				
				// 입력 초기화
				SetDlgItemText(hDialog, IDC_EDIT_NAME, TEXT(""));
				SetDlgItemText(hDialog, IDC_EDIT_PHONE, TEXT(""));
			}

			break;

		case IDC_BUTTON_DELETE:
			SendMessage(hCombo, CB_DELETESTRING, selectIndex, 0);
			break;

		case IDC_LIST1:
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
