#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
VOID InitBitmap(const INT, HBITMAP*, LPBITMAP);

HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("더블 버퍼링 움직이는 나비");

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
	// 비트맵 핸들 값
	static HBITMAP Background;
	static HBITMAP Character;
	static HBITMAP Mask;
	// 나비 캐릭터 크기 값
	static BITMAP bm;
	// 클라이언트 크기 값
	static RECT client;

	// 나비 좌표값
	static struct Point2D points[120];
	static INT NextAccessPoints;

	HDC hdc;
	PAINTSTRUCT ps;

	switch (iMessage) {
	case WM_CREATE:
		// 비트맵 로딩
		InitBitmap(IDB_BITMAP_BG, &Background, NULL);
		InitBitmap(IDB_BITMAP_BF, &Character, &bm);
		InitBitmap(IDB_BITMAP_BF_MASK, &Mask, NULL);

		// 좌표 값 초기화 (-1, -1)
		memset(points, -1, sizeof(points));
		NextAccessPoints = 0;

		SetTimer(hWnd, 1, 3, NULL);
		GetClientRect(hWnd, &client);
		break;

	case WM_DESTROY:
		DeleteObject(Background);
		DeleteObject(Character);
		DeleteObject(Mask);
		KillTimer(hWnd, 1);
		PostQuitMessage(0);
		return 0;

	case WM_SIZE:
		GetClientRect(hWnd, &client);
		break;

	case WM_LBUTTONDOWN:
	{
		// 클릭 지점에 중앙에 나비가 생성되도록 좌표 지정
		INT startX = LOWORD(lParam) - (bm.bmWidth / 2);
		INT startY = HIWORD(lParam) - (bm.bmHeight / 2);

		// 화면에 반짝거리지 않도록 나비가 그려지는 구간을 구해
		// invalidate rect를 진행합니다.
		RECT rect = { startX, startY, startX + bm.bmWidth, startY + bm.bmHeight };

		points[NextAccessPoints].x = startX;
		points[NextAccessPoints].y = startY;

		NextAccessPoints += 1;

		// 다음에 설정할 나비 좌표가 points 배열을 넘었다면
		// 0번 인덱스로 초기화합니다.
		if (NextAccessPoints >= sizeof(points) / sizeof(struct Point2D)) {
			NextAccessPoints = 0;
		}

		InvalidateRect(hWnd, &rect, TRUE);
	}
	break;

	case WM_PAINT: {
		hdc = BeginPaint(hWnd, &ps);

		HDC bufDC = CreateCompatibleDC(hdc);
	
		HBITMAP hBit = CreateCompatibleBitmap(bufDC, client.right, client.bottom);
		HBITMAP hBitOld = (HBITMAP)SelectObject(bufDC, hBit);

		// 배경화면 그리기
		// - memdc 자동 삭제 모드
		// - 클라이언트 크기에 stretch
		DrawBitmap(bufDC, NULL, 0, 0, Background, &client, SRCCOPY);

		for (INT i = 0; i < sizeof(points) / sizeof(struct Point2D); i++) {
			struct Point2D* ptr = &points[i];

			if (ptr->x > 0) {
				HDC MemDC = CreateCompatibleDC(hdc);
		
				// 나비 그리기
				// - memdc 수동 삭제 모드
				// - stretch 없이 그리기
				// - AND 또는 OR 모드로 그리기
				DrawBitmap(bufDC, MemDC, ptr->x, ptr->y, Mask, NULL, SRCAND);
				DrawBitmap(bufDC, MemDC, ptr->x, ptr->y, Character, NULL, SRCPAINT);

				DeleteDC(MemDC);
			}
		}

		BitBlt(hdc, 0, 0, client.right, client.bottom, bufDC, 0, 0, SRCCOPY);

		// 기존 비트맵 선택 및 버퍼 DC 삭제
		DeleteObject(SelectObject(bufDC, hBitOld));
		DeleteDC(bufDC);

		EndPaint(hWnd, &ps);
		break;
	}

	case WM_TIMER:
		for (INT i = 0; i < sizeof(points) / sizeof(struct Point2D); i++) {
			struct Point2D* ptr = &points[i];

			ptr->x += rand() % (client.right / 3);

			if (ptr->x >= client.right) {
				ptr->x = 0;
			}

			ptr->y += rand() % (client.bottom / 3);

			if (ptr->y >= client.bottom) {
				ptr->y = 0;
			}
		}

		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}
