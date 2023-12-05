#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
VOID InitBitmap(const INT, HBITMAP*, LPBITMAP);

HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("��Ʈ�� ����ũ");

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

VOID InitBitmap(const INT bitmapId, HBITMAP* lpHbit, LPBITMAP lpBit) {
	*lpHbit = LoadBitmap(g_hInst, MAKEINTRESOURCE(bitmapId));

	if (lpBit != NULL) {
		GetObject(*lpHbit, sizeof(BITMAP), lpBit);
	}
}

VOID DrawBitmap(const HDC hdc, const HDC MemDC, const INT x, const INT y, const HBITMAP hBit, LPCRECT rect, const DWORD drawMode) {
	HBITMAP old;
	BITMAP bm;
	
	BOOL manualDelete = FALSE;
	HDC mdc = MemDC;

	GetObject(hBit, sizeof(BITMAP), &bm);

	if (MemDC == NULL) {
		manualDelete = TRUE;
		mdc = CreateCompatibleDC(hdc);
	}

	old = (HBITMAP)SelectObject(mdc, hBit);

	if (rect == NULL) {
		BitBlt(hdc, x, y, bm.bmWidth, bm.bmHeight, mdc, 0, 0, drawMode);
	}
	else {
		StretchBlt(hdc, 0, 0, rect->right, rect->bottom, mdc, 0, 0, bm.bmWidth, bm.bmHeight, drawMode);
	}

	SelectObject(mdc, old);

	if (manualDelete == TRUE) {
		DeleteDC(mdc);
	}
}

struct Point2D {
	INT x;
	INT y;
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	// ��Ʈ�� �ڵ� ��
	static HBITMAP Background;
	static HBITMAP Character;
	static HBITMAP Mask;
	// ���� ĳ���� ũ�� ��
	static BITMAP bm;
	// Ŭ���̾�Ʈ ũ�� ��
	static RECT client;

	// ���� ��ǥ��
	static struct Point2D points[50];
	static INT NextAccessPoints;

	HDC hdc;
	PAINTSTRUCT ps;

	switch (iMessage) {
	case WM_CREATE:
		// ��Ʈ�� �ε�
		InitBitmap(IDB_BITMAP_BG,      &Background, NULL);
		InitBitmap(IDB_BITMAP_BF,      &Character,  &bm);
		InitBitmap(IDB_BITMAP_BF_MASK, &Mask,       NULL);

		// ��ǥ �� �ʱ�ȭ (-1, -1)
		memset(points, -1, sizeof(points));
		NextAccessPoints = 0;

		GetClientRect(hWnd, &client);
		break;

	case WM_DESTROY:
		DeleteObject(Background);
		DeleteObject(Character);
		DeleteObject(Mask);
		PostQuitMessage(0);
		return 0;

	case WM_SIZE:
		GetClientRect(hWnd, &client);
		break;

	case WM_LBUTTONDOWN:
		{
			// Ŭ�� ������ �߾ӿ� ���� �����ǵ��� ��ǥ ����
			INT startX = LOWORD(lParam) - (bm.bmWidth / 2);
			INT startY = HIWORD(lParam) - (bm.bmHeight / 2);

			// ȭ�鿡 ��¦�Ÿ��� �ʵ��� ���� �׷����� ������ ����
			// invalidate rect�� �����մϴ�.
			RECT rect = { startX, startY, startX + bm.bmWidth, startY + bm.bmHeight };

			points[NextAccessPoints].x = startX;
			points[NextAccessPoints].y = startY;

			NextAccessPoints += 1;

			// ������ ������ ���� ��ǥ�� points �迭�� �Ѿ��ٸ�
			// 0�� �ε����� �ʱ�ȭ�մϴ�.
			if (NextAccessPoints >= sizeof(points) / sizeof(struct Point2D)) {
				NextAccessPoints = 0;
			}

			InvalidateRect(hWnd, &rect, TRUE);
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		// ���ȭ�� �׸���
		// - memdc �ڵ� ���� ���
		// - Ŭ���̾�Ʈ ũ�⿡ stretch
		DrawBitmap(hdc, NULL, 0, 0, Background, &client, SRCCOPY);

		for (INT i = 0; i < sizeof(points) / sizeof(struct Point2D); i++) {
			struct Point2D* ptr = &points[i];

			if (ptr->x > 0) {
				HDC MemDC = CreateCompatibleDC(hdc);
	
				// ���� �׸���
				// - memdc ���� ���� ���
				// - stretch ���� �׸���
				// - AND �Ǵ� OR ���� �׸���
				DrawBitmap(hdc, MemDC, ptr->x, ptr->y, Mask,      NULL, SRCAND);
				DrawBitmap(hdc, MemDC, ptr->x, ptr->y, Character, NULL, SRCPAINT);
			
				DeleteDC(MemDC);
			}
		}

		EndPaint(hWnd, &ps);
		break;
	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}
