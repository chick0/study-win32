#include <Windows.h>
#include <tchar.h>

#define MAX_COUNTER 10

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("����̿� ��");

#pragma region WinMain
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
#pragma endregion

void PrintCat(const HDC* hdc, const INT *x, const INT *y) {
	TextOut(*hdc, *x, *y, TEXT("�����"), 3);
}

void PrintMouse(const HDC* hdc, const INT *x, const INT *y) {
	TextOut(*hdc, *x, *y, TEXT("��"), 1);
}

void PrintCatched(const HDC* hdc, const INT* x, const INT* y) {
	TextOut(*hdc, *x, *y, TEXT("��Ҵ�!"), 4);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;

	static INT catX;
	static INT catY;
	static BOOL isCatCatched;

	static INT mouseX;
	static INT mouseY;
	static BOOL isMouseMoving;

	static INT weightX;
	static INT weightY;

	static INT TimerID;
	static INT Counter;

	switch (iMessage) {
	case WM_DESTROY:
		PostQuitMessage(0);

		if (TimerID > 0) {
			KillTimer(hWnd, TimerID);
		}

		return 0;

	case WM_CREATE:
		catX = 100;
		catY = 100;
		isCatCatched = FALSE;

		mouseX = -1;
		mouseY = -1;

		TimerID = -1;

		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		PrintCat(&hdc, &catX, &catY);

		if (mouseX > 0 && mouseY > 0) {
			PrintMouse(&hdc, &mouseX, &mouseY);
		}

		if (isCatCatched == TRUE) {
			PrintCatched(&hdc, &catX, &catY);
		}


		EndPaint(hWnd, &ps);
		return 0;

	case WM_LBUTTONDOWN:
		isMouseMoving = TRUE;
		isCatCatched = FALSE;

		mouseX = LOWORD(lParam);
		mouseY = HIWORD(lParam);
		Counter = MAX_COUNTER;
		InvalidateRect(hWnd, NULL, TRUE);

		if (TimerID < 0) {
			TimerID = 1;
			SetTimer(hWnd, TimerID, 500, NULL);
		}

		break;

	case WM_LBUTTONUP:
		isMouseMoving = FALSE;
		mouseX = -1;

		if (TimerID > 0) {
			KillTimer(hWnd, TimerID);
			TimerID = -1;
		}

		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_MOUSEMOVE:
		if (isMouseMoving) {
			mouseX = LOWORD(lParam);
			mouseY = HIWORD(lParam);
			Counter = MAX_COUNTER;
			InvalidateRect(hWnd, NULL, TRUE);

			if (TimerID < 0) {
				TimerID = 1;
				SetTimer(hWnd, TimerID, 500, NULL);
			}
		}
		else {
			KillTimer(hWnd, TimerID);
			TimerID = -1;
		}

		break;

	case WM_TIMER:
		if (Counter <= 1) {
			KillTimer(hWnd, TimerID);
			TimerID = -1;
			mouseX = -1;

			isCatCatched = TRUE;
		}
		else {
			catX += (mouseX - catX) / Counter;
			catY += (mouseY - catY) / Counter;
			Counter -= 1;
		}

		InvalidateRect(hWnd, NULL, TRUE);
		break;

	}

	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}
