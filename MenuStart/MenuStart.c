#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <math.h>
#include "resource.h"
#include "resource1.h"

#define BSIZE 40

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("wi");

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
	WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
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

typedef struct _Point {
	INT x;
	INT y;
} Point;

double LengthPts(const Point *p1, const Point *p2)
{
	return (sqrt((float)((p2->x - p1->x) * (p2->x - p1->x) + (p2->y - p1->y) * (p2->y - p1->y))));
}

BOOL InCircle(const Point *circle, const Point click)
{
	if (LengthPts(circle, &click) < BSIZE) return TRUE;
	else return FALSE;
}

void UpdateMenu(HMENU* hSubMenu, BOOL* selection, BOOL* copy) {
	EnableMenuItem(*hSubMenu, ID_EDIT_COPY, *selection ? MF_ENABLED : MF_GRAYED);
	EnableMenuItem(*hSubMenu, ID_EDIT_PASTE, *copy ? MF_ENABLED : MF_GRAYED);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	static HMENU hMenu;
	static HMENU hSubMenu;
	static RECT rectView;
	static BOOL selection;
	static BOOL copy;
	static POINT points[10];
	static INT SelectedPoint;
	static INT PointIndex;

	Point clickPos = { -1, -1 };

	switch (iMessage) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_CREATE:
		hMenu = GetMenu(hWnd);
		hSubMenu = GetSubMenu(hMenu, 1);
		selection = FALSE;
		copy = FALSE;

		memset(points, -1, sizeof(points));
		points[0].x = 50;
		points[0].y = 50;

		PointIndex = 0;
		SelectedPoint = -1;

		break;

	case WM_LBUTTONDOWN:
		for (int i = 0; i < sizeof(points) / sizeof(Point); i++) {
			clickPos.x = LOWORD(lParam);
			clickPos.y = HIWORD(lParam);

			if (InCircle(&points[i], clickPos)) {
				selection = TRUE;
				SelectedPoint = i;
				InvalidateRect(hWnd, NULL, TRUE);
				break;
			}
		}

		break;

	case WM_RBUTTONDOWN:
		if (copy == TRUE && selection == TRUE) {
			PointIndex += 1;

			if (PointIndex >= sizeof(points) / sizeof(Point)) {
				PointIndex = 0;
			}

			points[PointIndex].x = LOWORD(lParam);
			points[PointIndex].y = HIWORD(lParam);

			selection = FALSE;
			copy = FALSE;

			InvalidateRect(hWnd, NULL, TRUE);
		}

		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {

		case ID_FILE_NEW:
			MessageBox(hWnd, TEXT("새 파일?"), TEXT("새 파일 선택"), MB_OKCANCEL);
			break;

		case ID_FILE_OPEN:
		{
			OPENFILENAME OFN;
			TCHAR str[300] = TEXT("");
			TCHAR lpstrFile[MAX_PATH] = TEXT("");
			TCHAR lpstrFileTitle[MAX_PATH] = TEXT("");

			memset(&OFN, 0, sizeof(OPENFILENAME));
			OFN.lStructSize = sizeof(OPENFILENAME);
			OFN.hwndOwner = hWnd;
			OFN.lpstrFilter = TEXT("모든 파일\0*.*\0텍스트 파일\0*.txt\0");
			OFN.lpstrFile = lpstrFile;
			OFN.nMaxFile = MAX_PATH;
			OFN.lpstrFileTitle = lpstrFileTitle;
			OFN.nMaxFileTitle = MAX_PATH;
			OFN.lpstrInitialDir = TEXT(".");

			if (GetOpenFileName(&OFN) != 0) {
				wsprintf(str, TEXT("%s 파일을 열겠습니까?"), OFN.lpstrFileTitle);
				MessageBox(hWnd, str, TEXT("열기 선택"), MB_OK);
			}
		}
			break;

		case ID_EXIT:
		{
			INT answer = MessageBox(hWnd, TEXT("파일 저장 끝?"), TEXT("끝내기"), MB_YESNOCANCEL);

			if (answer == IDYES || answer == IDNO) {
				PostQuitMessage(0);
			}
		}
			break;
		
		case ID_EDIT_COPY:
			copy = TRUE;
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case ID_EDIT_PASTE:
			copy = FALSE;
			selection = FALSE;
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		}

		break;

	case WM_PAINT:
		UpdateMenu(&hSubMenu, &selection, &copy);

		hdc = BeginPaint(hWnd, &ps);
		
		for (int i = 0; i < sizeof(points) / sizeof(Point); i++) {
			Point* p = &points[i];

			if (p->x > 0) {
				if (selection && SelectedPoint == i) {
					Rectangle(hdc, p->x - BSIZE, p->y - BSIZE, p->x + BSIZE, p->y + BSIZE);
				}

				Ellipse(hdc, p->x - BSIZE, p->y - BSIZE, p->x + BSIZE, p->y + BSIZE);
			}
		}

		EndPaint(hWnd, &ps);
		return 0;
	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}