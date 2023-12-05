#include <Windows.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("UI 싫어!!!!!!!!!!");

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
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, (HMENU)NULL, hInstance, NULL);
    ShowWindow(hWnd, nCmdShow);

    while (GetMessage(&Message, NULL, 0, 0)) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }

    return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
    HDC hdc;
    PAINTSTRUCT ps;

    switch (iMessage) {
    case WM_LBUTTONDOWN:
        if (DialogBox(g_hInst, MAKEINTRESOURCE(IDD_BASEINFO), hWnd, DialogProc) == IDOK) {
            InvalidateRect(hWnd, NULL, TRUE);
        }
     
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);

        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

INT_PTR CALLBACK DialogProc(HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam) {
    LPCWSTR gender_list[] = {
        TEXT("남자"),
        TEXT("여자"),
    };

    LPCWSTR hobby_list[] = {
        TEXT("독서"),
        TEXT("음악감상"),
        TEXT("인터넷"),
        TEXT("게임"),
    };

    switch (message) {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_BTN_COPY:
        {
            TCHAR before[128];
            GetDlgItemText(hDialog, IDC_CPY_INPUT, before, 128);
            SetDlgItemText(hDialog, IDC_CPY_RESULT, before);
        }
            break;

        case IDC_BTN_CLEAR:
            SetDlgItemText(hDialog, IDC_CPY_INPUT, TEXT(""));
            SetDlgItemText(hDialog, IDC_CPY_RESULT, TEXT(""));
            break;

        case IDCANCEL:
            EndDialog(hDialog, 0);
            break;

        }
    }

    return (INT_PTR)FALSE;
}
