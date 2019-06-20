#include <windows.h>
#include "Minesweeper.h"
#include "Game.h"
#include <iostream>
#include <string>
#include <map>

TCHAR MINESWEEPER_CLASSNAME[] = L"Minesweeper";
const int RESET_ID = 1000;

std::map<int, HWND> cells;
HWND mainWindow;
Game game(1, 1, 0);
bool** visited;
bool gameIsOver;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawCell(LPDRAWITEMSTRUCT item);

void handleReset();
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
	mainWindow = hMainWnd;

	game = Game(8, 8, 10);
	gameIsOver = false;

	visited = new bool*[game.getHeight()];
	for (int i = 0; i < game.getWidth(); i++) {
		visited[i] = new bool[game.getWidth()];
		std::fill(visited[i], visited[i] + game.getWidth(), false);
	}

	cells = std::map<int, HWND>();

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
				WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
				x, y,
				cellSize, cellSize,
				hMainWnd,
				(HMENU)id,
				NULL,
				NULL
			);

			cells.insert(std::pair<int, HWND>(id, button));
		}
	}

	//draw reset button
	int resetWidth = game.getWidth() * (gapX + cellSize) - gapX;
	int resetTop = marginTop + game.getHeight() * (gapY + cellSize) + cellSize;

	CreateWindowA(
		"button",
		"����� ����",
		WS_CHILD | WS_VISIBLE | BS_FLAT | BS_PUSHBUTTON,
		marginLeft, resetTop,
		resetWidth, cellSize,
		hMainWnd,
		(HMENU)RESET_ID,
		NULL,
		NULL
	);
}

// �������� ������� ��������� ���������
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int code = LOWORD(wParam);
	std::map<int, HWND>::iterator it;

	LPDRAWITEMSTRUCT item;

	switch (uMsg) {
	case WM_DRAWITEM:
		item = (LPDRAWITEMSTRUCT)lParam;

		if (item->CtlID <= 100) {
			DrawCell(item);
		}
	case WM_PAINT: // ���� ����� ����������, ��:
		break;
	case WM_DESTROY: // ���� ������ ���������, ��:
		PostQuitMessage(NULL); // ���������� WinMain() ��������� WM_QUIT
		break;
	case WM_COMMAND:
		switch (code) {
		case RESET_ID:
			handleReset();
			break;
		default:
			handleFieldClick(code);
		}

		return 0;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam); // ���� ������� ������
	}

	return NULL; // ���������� ��������
}

void DrawCell(LPDRAWITEMSTRUCT item)
{
	HDC hdc = item->hDC;
	HWND hwnd = item->hwndItem;
	RECT rect = item->rcItem;

	HBRUSH hpen = CreateSolidBrush(RGB(230, 230, 230));
	SelectObject(hdc, hpen);
	Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
	DeleteObject(hpen);

	LPWSTR text = new wchar_t[1];
	GetWindowText(hwnd, text, 1);

	HFONT font = CreateFont(30, 0, 0, 0, 700, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"Arial");
	SetBkMode(hdc, TRANSPARENT);
	SelectObject(hdc, font);

	DrawText(hdc, text, 1, &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
}

void handleReset() {
	for (std::map<int, HWND>::iterator it = cells.begin(); it != cells.end(); ++it) {
		HWND button = it->second;
		DestroyWindow(button);
	}

	drawMinesweeperControls(mainWindow);
	UpdateWindow(mainWindow);
}

void handleFieldClick(int code) {
	if (gameIsOver) {
		return;
	}

	std::pair<int, int> yx = parseComponentCode(code);

	if (visited[yx.first][yx.second]) {
		return;
	}

	HWND button = cells.at(code);

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
	HWND button = cells.at(code);
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
		HWND button = cells.at(code);

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