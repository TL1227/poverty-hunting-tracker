#ifndef UNICODE
#define UNICODE
#endif 

#include <string>
#include <map>
#include <windows.h>

#include "hunt.h"

using namespace std;

HWND MainWindow;

Vault _vault{};
Wallet _wallet{};

HFONT GetMainFont(const wchar_t* fontName);
HFONT MainFont = GetMainFont(L"Calbri");

bool CanEditWinningsControls = true;

#define IPC_WINNINGS_TEXTB 1000
#define IPC_WINNINGS_LABEL 1001

#define IPC_VAULT_LABEL 1002
#define IPC_VAULT_VALUE 1003

#define IPC_WALLET_LABEL 1004
#define IPC_WALLET_VALUE 1005

#define IPC_SPLIT_BUTTON 1006

#define IPC_BASKET_LABEL 1007
#define IPC_BASKET_TEXTB 1008

#define IPC_SPEND_BUTTON 1009

#define IPC_NEWHUNT_BUTTON 1010

int xBorder = 10;
int yBorder = 10;

int columns[] = { 10, 90, 170, 250, 330 };

std::map<HWND, WNDPROC> EditProcs;
std::map<HWND, WNDPROC> LabelProcs;

HBITMAP hBackground = NULL;

HFONT GetMainFont(const wchar_t* fontName)
{
    return CreateFont(
        -MulDiv(12, GetDeviceCaps(GetDC(NULL), LOGPIXELSY), 72),
        0, 0, 0,
        FW_NORMAL,
        FALSE, FALSE, FALSE,
        ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_SWISS,
        fontName
    );
}

HWND CreateControl(const LPCWSTR type, long long controlId, const LPCWSTR text, DWORD styles, int x, int y, int width, int length, HWND parentHwnd)
{
    HWND control = CreateWindowEx( 0, type, text, styles, x, y, width, length, parentHwnd, (HMENU)controlId,
        (HINSTANCE)GetWindowLongPtr(parentHwnd, GWLP_HINSTANCE),
        NULL
    );

    SendMessage(control, WM_SETFONT, (WPARAM)MainFont, TRUE);

    return control;
}

void HandleBasket()
{
	int basketTotal = GetDlgItemInt(MainWindow, IPC_BASKET_TEXTB, NULL, 1);

	if (_wallet.SpendMoney(basketTotal))
	{
        HWND hwnd = GetDlgItem(MainWindow, IPC_WALLET_VALUE);
        UpdateWindow(hwnd);
		SetDlgItemInt(MainWindow, IPC_WALLET_VALUE, _wallet.Total, 1);
	}

	SetDlgItemText(MainWindow, IPC_BASKET_TEXTB, L"");
}

void HandleWinnings()
{
	int winnings = GetDlgItemInt(MainWindow, IPC_WINNINGS_TEXTB, NULL, 1);

    if (winnings == 0)
    {
		SetDlgItemText(MainWindow, IPC_WINNINGS_TEXTB, L"");
        return;
    }
    else
    {
		_wallet.AddWinnings(winnings);
		SetDlgItemInt(MainWindow, IPC_WALLET_VALUE, _wallet.Total, 1);

		_vault.AddWinnings(winnings);
		SetDlgItemText(MainWindow, IPC_VAULT_VALUE, to_wstring(_vault.Total).c_str());

        EnableWindow(GetDlgItem(MainWindow, IPC_SPLIT_BUTTON), FALSE);
        EnableWindow(GetDlgItem(MainWindow, IPC_WINNINGS_TEXTB), FALSE);

        HWND basket = GetDlgItem(MainWindow, IPC_BASKET_TEXTB);
        SetFocus(basket);
    }
}

//Callbacks
LRESULT CALLBACK TextBoxProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_GETDLGCODE:
            return DLGC_WANTALLKEYS;

        case WM_KEYDOWN: {
            if (wParam == VK_RETURN)
            {
                int controlId = GetDlgCtrlID(hwnd);

                if (controlId == IPC_WINNINGS_TEXTB)
                {
                    HandleWinnings();
                }
                if (controlId == IPC_BASKET_TEXTB)
                {
                    HandleBasket();
                }

                break;
            }
        }
    }
    return CallWindowProc(EditProcs[hwnd], hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE: {

        int RowSize = 50;

        DWORD textBoxStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP;
        DWORD labelStyle = WS_CHILD | WS_VISIBLE | SS_CENTER |SS_CENTERIMAGE;
        DWORD buttonStyle = WS_CHILD | WS_VISIBLE | BS_FLAT; 

        CreateControl(L"STATIC", IPC_WINNINGS_LABEL, L"Winnings", labelStyle, columns[0], yBorder, 70, 25, hwnd);

        HWND textBoxHandle = CreateControl(L"EDIT", IPC_WINNINGS_TEXTB, L"", textBoxStyle, columns[1], yBorder, 70, 25, hwnd);
        EditProcs[textBoxHandle] = (WNDPROC)SetWindowLongPtr(textBoxHandle, GWLP_WNDPROC, (LONG_PTR)TextBoxProc);
        SetFocus(textBoxHandle);

        CreateControl(L"BUTTON", IPC_SPLIT_BUTTON, L"Split", buttonStyle, columns[2], yBorder, 70, 25, hwnd);

        yBorder += RowSize;

        CreateControl(L"STATIC", IPC_VAULT_LABEL, L"Vault", labelStyle, columns[0], yBorder, 70, 25, hwnd);
        CreateControl(L"STATIC", IPC_VAULT_VALUE, L"0", labelStyle, columns[1], yBorder, 70, 25, hwnd);

        yBorder += RowSize;

        CreateControl(L"STATIC", IPC_WALLET_LABEL, L"Wallet", labelStyle, columns[0], yBorder, 70, 25, hwnd);
        CreateControl(L"STATIC", IPC_WALLET_VALUE, L"0", labelStyle, columns[1], yBorder, 70, 25, hwnd);

        yBorder += RowSize;

        CreateControl(L"STATIC", IPC_BASKET_LABEL, L"Basket", labelStyle, columns[0], yBorder, 70, 25, hwnd);

        textBoxHandle = CreateControl(L"EDIT", IPC_BASKET_TEXTB, L"", textBoxStyle, columns[1], yBorder, 70, 25, hwnd);
        EditProcs[textBoxHandle] = (WNDPROC)SetWindowLongPtr(textBoxHandle, GWLP_WNDPROC, (LONG_PTR)TextBoxProc);

        CreateControl(L"BUTTON", IPC_SPEND_BUTTON, L"Spend", buttonStyle, columns[2], yBorder, 70, 25, hwnd);

        yBorder += RowSize;

        CreateControl(L"BUTTON", IPC_NEWHUNT_BUTTON, L"Next Hunt", buttonStyle, columns[3], yBorder, 80, 25, hwnd);

        hBackground = (HBITMAP)LoadImage(NULL, L"bg.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

        return 0;
    }

    case BN_CLICKED: {
        PostQuitMessage(0);
        return 0;
    }
    case WM_CTLCOLORSTATIC: {
        UpdateWindow(hwnd);
        HDC hdc = (HDC)wParam;
        SetTextColor(hdc, RGB(255, 255, 255)); // White text
        SetBkMode(hdc, TRANSPARENT);

        return (INT_PTR)CreateSolidBrush(RGB(20, 20, 20));
    }
    case WM_COMMAND: {

        if (wParam == IPC_SPLIT_BUTTON)
        {
            HandleWinnings();
        }
        else if (wParam == IPC_SPEND_BUTTON)
        {
            HandleBasket();
        }
        else if (wParam == IPC_NEWHUNT_BUTTON)
        {
            EnableWindow(GetDlgItem(MainWindow, IPC_SPLIT_BUTTON), TRUE);
            EnableWindow(GetDlgItem(MainWindow, IPC_WINNINGS_TEXTB), TRUE);
            SetDlgItemText(MainWindow, IPC_WINNINGS_TEXTB, L"");
        }
        return 0;
    }

    case WM_DESTROY: {
        PostQuitMessage(0);
        return 0;
    }

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            if (hBackground)
            {
                HDC hMemDC = CreateCompatibleDC(hdc);
                HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hBackground);

                // Get bitmap dimensions
                BITMAP bmp;
                GetObject(hBackground, sizeof(BITMAP), &bmp);

                RECT rc;
                GetClientRect(hwnd, &rc);
                StretchBlt(hdc, 0, 0, rc.right, rc.bottom,
                    hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

                SelectObject(hMemDC, hOldBmp);
                DeleteDC(hMemDC);
            }

            EndPaint(hwnd, &ps);
        }
        return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


int WINAPI wWinMain(HINSTANCE windowHandle, HINSTANCE xxx, PWSTR cmdLine, int cmdShow)
{
    // Set the font for the edit control

    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = windowHandle;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.
    MainWindow = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Poverty Huntan",
        WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, 430, 600,
        NULL,
        NULL,
        windowHandle,
        NULL
    );

    if (MainWindow == NULL)
    {
        return 0;
    }

    ShowWindow(MainWindow, cmdShow);

    int answer = MessageBox(MainWindow, L"Hello Gamer, would you like to buy the Battle Pass?", L"BATTLEPA$$", MB_YESNO | MB_ICONQUESTION);

    if (answer == IDNO)
        answer = MessageBox(MainWindow, L"Let's try that again. Would you LIKE, TO BUY, the Battle Pass?", L"BATTLEPA$$", MB_OK | MB_SYSTEMMODAL);

    if (answer == IDNO)

    MessageBox(MainWindow, L"Thank you! $99.99 has been withdrawn from your steam wallet.", L"BATTLEPA$$", MB_OK);


    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        if (!IsDialogMessage(MainWindow, &msg)) 
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return 0;
}
