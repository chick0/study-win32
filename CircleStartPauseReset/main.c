#include <Windows.h>
#include "resource.h"

#define TIMER_OFF  -1
#define TIMER_ID   1
#define TIMER_TICK 100

#define SIZE 50
#define SPEED 20

#define LEFT  -1
#define RIGHT 1

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("¿ø¿ø");

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) {
    HWND hWnd;
    MSG Message;
    WNDCLASS WndClass;
    g_hInst = hInstance;
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;
    WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WndClass.hIcon = LoadIcon(NULL, IDI_INFORMATION);
    WndClass.hInstance = hInstance;
    WndClass.lpfnWndProc = WndProc;
    WndClass.lpszClassName = lpszClass;
    WndClass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
    WndClass.style = CS_HREDRAW | CS_VREDRAW;
    RegisterClass(&WndClass);

    hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 400,
        NULL, (HMENU)NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);

    while (GetMessage(&Message, NULL, 0, 0)) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }
    return (int)Message.wParam;
}

typedef struct _Point2D {
    INT x;
    INT y;
} Point2D ;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
    HDC hdc;
    PAINTSTRUCT ps;

    static HMENU hMenu, hSubMenu;

    static RECT clientSize;

    static INT timerId;
    static Point2D position;
    static INT direction;

    switch (iMessage) {
    case WM_CREATE:
        hMenu = GetMenu(hWnd);
        hSubMenu = GetSubMenu(hMenu, 0);
        GetClientRect(hWnd, &clientSize);
        position.x = 0;
        position.y = 0;
        timerId = TIMER_OFF;
        direction = RIGHT;

        EnableMenuItem(hSubMenu, ID_START, MF_ENABLED);
        EnableMenuItem(hSubMenu, ID_PAUSE, MF_GRAYED);
        EnableMenuItem(hSubMenu, ID_RESET, MF_GRAYED);
        break;

    case WM_SIZE:
        GetClientRect(hWnd, &clientSize);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_START:
            timerId = SetTimer(hWnd, TIMER_ID, TIMER_TICK, NULL);

            EnableMenuItem(hSubMenu, ID_START, MF_GRAYED);
            EnableMenuItem(hSubMenu, ID_PAUSE, MF_ENABLED);
            EnableMenuItem(hSubMenu, ID_RESET, MF_ENABLED);
            break;

        case ID_PAUSE:
            KillTimer(hWnd, timerId);
            timerId = TIMER_OFF;

            EnableMenuItem(hSubMenu, ID_START, MF_ENABLED);
            EnableMenuItem(hSubMenu, ID_PAUSE, MF_GRAYED);
            EnableMenuItem(hSubMenu, ID_RESET, MF_ENABLED);
            break;

        case ID_RESET:
            position.x = 0;
            position.y = 0;

            EnableMenuItem(hSubMenu, ID_START, MF_ENABLED);
            EnableMenuItem(hSubMenu, ID_PAUSE, MF_GRAYED);
            EnableMenuItem(hSubMenu, ID_RESET, MF_GRAYED);
            break;
        }

        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        Ellipse(hdc, position.x, position.y, position.x + SIZE, position.y + SIZE);
        EndPaint(hWnd, &ps);
        break;

    case WM_TIMER:
        position.x += SPEED * direction;

        if (position.x < 0) {
            position.x = 0;
            direction = RIGHT;
        }
        else if (position.x > clientSize.right - SIZE) {
            position.x = clientSize.right - SIZE;
            direction = LEFT;
        }

        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_DESTROY:
        if (timerId != TIMER_OFF) {
            KillTimer(hWnd, timerId);
        }

        PostQuitMessage(0);
        break;
    }

    return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}