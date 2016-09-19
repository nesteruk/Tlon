#include "stdafx.h"
#include "Resource.h"

#include "../Compiler/headers.hpp"
#include "../Compiler/parser.hpp"
using namespace tlön;

#define MAX_LOADSTRING 100

HINSTANCE hInst;
HWND hInput, hOutput;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
WNDPROC oldEditProc;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                UpdateOutput();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
  _In_opt_ HINSTANCE hPrevInstance,
  _In_ LPWSTR    lpCmdLine,
  _In_ int       nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);

  LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
  LoadStringW(hInstance, IDC_TLNPAD, szWindowClass, MAX_LOADSTRING);
  MyRegisterClass(hInstance);

  if (!InitInstance(hInstance, nCmdShow))
  {
    return FALSE;
  }

  HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TLNPAD));
  MSG msg;

  while (GetMessage(&msg, nullptr, 0, 0))
  {
    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return static_cast<int>(msg.wParam);
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
  WNDCLASSEXW wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_TLNPAD));
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
  wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TLNPAD);
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

  return RegisterClassExW(&wcex);
}

LRESULT CALLBACK InputEditProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
  if (msg == WM_KEYDOWN
    && wp == 187 /* = */
    && GetKeyState(VK_CONTROL) & 0x8000)
  {
    SendMessage(hwnd, WM_CHAR, 8801 /* ≡ */, 0);
  }
  return CallWindowProc(oldEditProc, hwnd, msg, wp, lp);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
  hInst = hInstance; // Store instance handle in our global variable

  HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

  if (!hWnd) return FALSE;

  RECT rcClient;
  GetClientRect(hWnd, &rcClient);

  hInput = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""),
    WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_LEFT,
    0, 0, rcClient.right / 2, rcClient.bottom,
    hWnd, nullptr, hInstance, nullptr);

  if (!hInput) return FALSE;

  oldEditProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(hInput, 
    GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(InputEditProc)));

  hOutput = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""),
    WS_VISIBLE | WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VSCROLL
      | ES_MULTILINE | ES_LEFT 
      | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
    rcClient.right / 2, 0, rcClient.right / 2, rcClient.bottom,
    hWnd, nullptr, hInstance, nullptr);

  if (!hOutput) return FALSE;

  auto hFont = CreateFont(-48, 0, 0, 0, FW_DONTCARE, 0,
    0, 0, ANSI_CHARSET, 0, 0, 0, FIXED_PITCH, TEXT("Consolas"));
  SendMessage(hInput, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), 0);
  SendMessage(hOutput, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), 0);

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  SetWindowText(hInput, L"class Demo.ChangeMe(x,y:i16) {\r\n"
    L"  z:i32;\r\n"
    L"}\r\n\r\n"
    L"interface Demo.SomeInterface\r\n"
    L"{\r\n"
    L"  add := (a,b:i32, c:string) -> i32;\r\n"
    L"  \r\n"
    L"  tupledemo := (x:(i8,string)) -> (str,u8);\r\n"
    L"}\r\n");
  UpdateOutput();

  return TRUE;
}

void UpdateOutput()
{
  auto len = GetWindowTextLength(hInput) + 1;
  wstring input(len, 0);
  GetWindowText(hInput, &input[0], len);
  auto result = parse(input.begin(), input.end());
  SetWindowText(hOutput, result.c_str());
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
  case WM_COMMAND:
  {
    int wmId = LOWORD(wParam);
    auto hi = HIWORD(wParam);

    if (hi == EN_CHANGE && lParam == reinterpret_cast<LPARAM>(hInput))
    {
      UpdateOutput();
    }

    // Parse the menu selections:
    switch (wmId)
    {
    case IDM_ABOUT:
      DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
      break;
    case IDM_EXIT:
      DestroyWindow(hWnd);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
    }
  }
  break;
  case WM_SIZE:
  {
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    SetWindowPos(hInput, nullptr, 0, 0, rcClient.right / 2, rcClient.bottom, 0);
    SetWindowPos(hOutput, nullptr, rcClient.right / 2, 0, rcClient.right / 2, rcClient.bottom, 0);
  }
  break;
  case WM_PAINT:
  {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    EndPaint(hWnd, &ps);
  }
  break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  UNREFERENCED_PARAMETER(lParam);
  switch (message)
  {
  case WM_INITDIALOG:
    return static_cast<INT_PTR>(TRUE);

  case WM_COMMAND:
    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
    {
      EndDialog(hDlg, LOWORD(wParam));
      return static_cast<INT_PTR>(TRUE);
    }
    break;
  }
  return static_cast<INT_PTR>(FALSE);
}