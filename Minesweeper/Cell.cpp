#include <windows.h>
#include <cstdlib>
#include "Cell.h"

TCHAR CELL_CLASSNAME[] = L"CellClass";

const int CELL_FONT_SIZE = 32;

COLORREF *COLORS = new COLORREF[9] { RGB(255, 0, 0), RGB(0, 0, 200), RGB(0, 150, 0), RGB(150, 0, 0), RGB(186,85,211),
RGB(0, 0, 0), RGB(128, 0, 0), RGB(100, 100, 100), RGB(63, 224, 208) };

void registerCellClass(HINSTANCE hInst) {
	WNDCLASSEX cell = {
		sizeof cell,
		0,
		CellProc,
		0,
		0,
		hInst,
		0,
		0,
		reinterpret_cast <HBRUSH> (static_cast <int> (COLOR_BTNFACE) + 1),
		0,
		CELL_CLASSNAME,
		0
	};

	if (!RegisterClassEx(&cell)) {
		MessageBox(NULL, L"ќшибка регистрации €чейки игры!", L"ќшибка!", MB_OK);
		return;
	}
}

LRESULT __stdcall CellProc(HWND window, unsigned message, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	HBRUSH hpen;
	LPWSTR text;
	HFONT font;
	COLORREF color;
	int i;

	LRESULT res = 0;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(window, &ps);
		GetClientRect(window, &rect);

		text = new wchar_t[2];
		GetWindowText(window, text, 2);

		i = _wtoi(text);
		
		if (i == 0) {
			color = RGB(210, 210, 210);
		}
		else {
			color = RGB(230, 230, 230);
		}

		hpen = CreateSolidBrush(color);
		SelectObject(hdc, hpen);
		Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
		DeleteObject(hpen);

		color = COLORS[i];
		font = CreateFont(CELL_FONT_SIZE, 0, 0, 0, 700, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"Arial");
		SetBkMode(hdc, TRANSPARENT);
		SelectObject(hdc, font);
		SetTextColor(hdc, color);

		DrawText(hdc, text, 1, &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);

		EndPaint(window, &ps);

		break;
	case WM_SETTEXT:
		res = DefWindowProc(window, message, wParam, lParam);

		InvalidateRect(window, NULL, TRUE);
		UpdateWindow(window);

		break;
	case WM_LBUTTONDOWN:
		SendMessage(GetParent(window),
			WM_COMMAND,
			GetDlgCtrlID(window),
			(LPARAM)window);

		break;
	case WM_RBUTTONDOWN:
		NMHDR nmh;
		SendMessage(GetParent(window),
			WM_NOTIFY,
			GetDlgCtrlID(window),
			(LPARAM)window);

		break;
	default:
		res = DefWindowProc(window, message, wParam, lParam);
	}


	return res;
}