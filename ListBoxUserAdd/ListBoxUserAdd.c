#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("�ܼ� ���� Ż��");

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
		return "��";
	}
	else if (g == B) {
		return "��";
	}
	else {
		return "��Ÿ";
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

			_stprintf_s(line, _T("%s; %s; %s; %d��"), name, phone, TEXT("��Ÿ"), year);

			if (lstrcmp(line, TEXT(""))) {
				SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)name);
				
				// �Է� �ʱ�ȭ
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
