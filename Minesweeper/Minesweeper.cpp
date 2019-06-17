#include <windows.h>
#include "Minesweeper.h"
#include "Game.h"
#include <iostream>
#include <string>

TCHAR MINESWEEPER_CLASSNAME[] = L"Minesweeper";

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

WNDCLASSEX createMinesweeperWindow(HINSTANCE hInst) {
	WNDCLASSEX wc; // ������ ���������, ��� ��������� � ������ ������ WNDCLASSEX
	wc.cbSize = sizeof(wc); // ������ ��������� (� ������)
	wc.style = CS_HREDRAW | CS_VREDRAW; // ����� ������ ������
	wc.lpfnWndProc = WndProc; // ��������� �� ���������������� �������
	wc.lpszMenuName = NULL; // ��������� �� ��� ���� (� ��� ��� ���)
	wc.lpszClassName = MINESWEEPER_CLASSNAME; // ��������� �� ��� ������
	wc.cbWndExtra = NULL; // ����� ������������� ������ � ����� ���������
	wc.cbClsExtra = NULL; // ����� ������������� ������ ��� �������� ���������� ����������
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO); // ��������� �����������
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO); // ���������� ��������� ����������� (� ����)
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); // ���������� �������
	wc.hbrBackground = CreateSolidBrush(RGB(50, 50, 50));; // ���������� ����� ��� �������� ���� ����
	wc.hInstance = hInst; // ��������� �� ������, ���������� ��� ����, ������������ ��� ������

	return wc;
}

void drawMinesweeperControls(HWND hMainWnd) {
	// draw buttons
	int marginTop = 150;
	int marginLeft = 400;
	int gapX = 2;
	int gapY = 2;
	int cellSize = 45;

	Game game(8, 8, 10);

	for (int i = 0; i < game.getHeight(); i++) {
		for (int j = 0; j < game.getWidth(); j++) {
			int x = marginLeft + j * gapX + j * cellSize;
			int y = marginTop + i * gapY + i * cellSize;
			int id = i * 1000 + y;

			std::string s = std::to_string(game.getFeild(i, j));

			CreateWindowA(
				"button",
				s.c_str(),
				WS_CHILD | WS_VISIBLE | BS_FLAT | BS_PUSHBUTTON,
				x, y,
				cellSize, cellSize,
				hMainWnd,
				(HMENU)id,
				NULL,
				NULL
			);
		}
	}
}

// �������� ������� ��������� ���������
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_PAINT: // ���� ����� ����������, ��:
		break;
	case WM_DESTROY: // ���� ������ ���������, ��:
		PostQuitMessage(NULL); // ���������� WinMain() ��������� WM_QUIT
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam); // ���� ������� ������
	}

	return NULL; // ���������� ��������
}