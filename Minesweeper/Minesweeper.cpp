#include <windows.h>
#include "Minesweeper.h"
#include "Game.h"
#include <iostream>
#include <string>
#include <map>

TCHAR MINESWEEPER_CLASSNAME[] = L"Minesweeper";
std::map<int, HWND> components;
Game game(8, 8, 5);
bool** visited;
bool gameIsOver = false;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void handleFieldClick(int code);
void removeEmptyCells(int y, int x);
void gameOver();
int getComponentCode(int y, int x);
std::pair<int, int> parseComponentCode(int code);

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
	visited = new bool*[game.getHeight()];
	for (int i = 0; i < game.getWidth(); i++) {
		visited[i] = new bool[game.getWidth()];
		std::fill(visited[i], visited[i] + game.getWidth(), false);
	}

	components = std::map<int, HWND>();

	// draw buttons
	int marginTop = 150;
	int marginLeft = 400;
	int gapX = 2;
	int gapY = 2;
	int cellSize = 45;

	for (int i = 0; i < game.getHeight(); i++) {
		for (int j = 0; j < game.getWidth(); j++) {
			int x = marginLeft + j * gapX + j * cellSize;
			int y = marginTop + i * gapY + i * cellSize;
			int id = getComponentCode(i, j);

			HWND button = CreateWindowA(
				"button",
				"",
				WS_CHILD | WS_VISIBLE | BS_FLAT | BS_PUSHBUTTON,
				x, y,
				cellSize, cellSize,
				hMainWnd,
				(HMENU)id,
				NULL,
				NULL
			);

			components.insert(std::pair<int, HWND>(id, button));
		}
	}
}

// �������� ������� ��������� ���������
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int code = LOWORD(wParam);

	switch (uMsg) {
	case WM_PAINT: // ���� ����� ����������, ��:
		break;
	case WM_DESTROY: // ���� ������ ���������, ��:
		PostQuitMessage(NULL); // ���������� WinMain() ��������� WM_QUIT
		break;
	case WM_COMMAND:
		handleFieldClick(code);

		return 0;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam); // ���� ������� ������
	}

	return NULL; // ���������� ��������
}

void handleFieldClick(int code) {
	if (gameIsOver) {
		return;
	}

	std::pair<int, int> yx = parseComponentCode(code);

	if (visited[yx.first][yx.second]) {
		return;
	}

	HWND button = components.at(code);

	int value = game.getCell(yx.first, yx.second);

	if (value == -1) {
		gameOver();

		return;
	}

	if (value == 0) {
		removeEmptyCells(yx.first, yx.second);

		return;
	}

	std::string text = std::to_string(value);
	SetWindowTextA(button, text.c_str());
}

void removeEmptyCells(int y, int x) {
	visited[y][x] = true;


	int code = getComponentCode(y, x);
	HWND button = components.at(code);
	int cell = game.getCell(y, x);

	if (cell != 0) {
		std::string text = std::to_string(cell);
		SetWindowTextA(button, text.c_str());

		return;
	}

	DestroyWindow(button);

	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if (game.inField(y + i, x + j) && !visited[y + i][x + j]) {
				removeEmptyCells(y + i, x + j);
			}
		}
	}
}

void gameOver() {
	std::pair<int, int>* mines = game.getMines();

	for (int i = 0; i < game.getMineCount(); i++) {
		int code = getComponentCode(mines[i].first, mines[i].second);
		HWND button = components.at(code);

		SetWindowTextA(button, "X");
	}

	gameIsOver = true;
}

int getComponentCode(int y, int x) {
	return y * 10 + x;
}

std::pair<int, int> parseComponentCode(int code) {
	int y = code / 10;
	int x = code % 10;

	return std::pair<int, int>(y, x);
}