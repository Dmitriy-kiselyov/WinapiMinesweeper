#pragma once
#include <windows.h>

TCHAR CELL_CLASSNAME[];

LRESULT __stdcall CellProc(HWND window, unsigned message, WPARAM wParam, LPARAM lParam);
void registerCellClass(HINSTANCE hInst);
