#pragma once

#include <windows.h>

TCHAR MINESWEEPER_CLASSNAME[];

WNDCLASSEX createMinesweeperWindow(HINSTANCE hInst);
void drawMinesweeperControls(HWND hMainWnd);