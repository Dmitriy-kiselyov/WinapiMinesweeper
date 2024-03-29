﻿#include <windows.h>
#include "Minesweeper.h"
#include "Game.h"
#include "Cell.h"
#include <iostream>
#include <string>
#include <map>

TCHAR MINESWEEPER_CLASSNAME[] = L"Minesweeper";
const int RESET_ID = 12345;
const int CODE_BASE = 100;

const int GAME_SIZE = 12;
const int BOMB_COUNT = 20;

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 800;

const int GRID_H = GAME_SIZE + 4;
const int MARGIN_V = 40;
const int CELL_SIZE = (SCREEN_HEIGHT - MARGIN_V * 2) / GRID_H;
const int FIELD_SIZE = CELL_SIZE * GAME_SIZE;
const int MARGIN_HOR = (SCREEN_WIDTH - FIELD_SIZE) / 2;

const int BOMB_FONT_SIZE = 25;
const int RESULT_FONT_SIZE = 60;

std::map<int, HWND> cells;
HWND mainWindow;
RECT labelResultRect;
RECT bombCountRect;

Game game(1, 1, 0);
int** visited; // 0 - пусто, 1 - открыта, 2 - стоит флаг
int gameIsOver; // 0 - нет, 1 - победа, 2 - поражение
int flagCount;
int visitedCount;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void renderGameResultLabel(HDC hdc);
void renderBombCountLabel(HDC hdc);

void handleReset();
void handleCellClick(int code, bool recursive);
void handleCellRightClick(int code);
void removeEmptyCells(int y, int x);
void visitCell(int y, int x);
void gameLost();
void gameWon();
int getComponentCode(int y, int x);
std::pair<int, int> parseComponentCode(int code);

WNDCLASSEX createMinesweeperWindow(HINSTANCE hInst) {
	registerCellClass(hInst);

	WNDCLASSEX wc; // создаём экземпляр, для обращения к членам класса WNDCLASSEX
	wc.cbSize = sizeof(wc); // размер структуры (в байтах)
	wc.style = CS_HREDRAW | CS_VREDRAW; // стиль класса окошка
	wc.lpfnWndProc = WndProc; // указатель на пользовательскую функцию
	wc.lpszMenuName = NULL; // указатель на имя меню (у нас его нет)
	wc.lpszClassName = MINESWEEPER_CLASSNAME; // указатель на имя класса
	wc.cbWndExtra = NULL; // число освобождаемых байтов в конце структуры
	wc.cbClsExtra = NULL; // число освобождаемых байтов при создании экземпляра приложения
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO); // декриптор пиктограммы
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO); // дескриптор маленькой пиктограммы (в трэе)
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); // дескриптор курсора
	wc.hbrBackground = CreateSolidBrush(RGB(50, 50, 50));; // дескриптор кисти для закраски фона окна
	wc.hInstance = hInst; // указатель на строку, содержащую имя меню, применяемого для класса

	return wc;
}

void drawMinesweeperControls(HWND hMainWnd) {
	mainWindow = hMainWnd;

	game = Game(GAME_SIZE, GAME_SIZE, BOMB_COUNT);
	gameIsOver = 0;
	flagCount = 0;
	visitedCount = 0;

	visited = new int*[game.getHeight()];
	for (int i = 0; i < game.getWidth(); i++) {
		visited[i] = new int[game.getWidth()];
		std::fill(visited[i], visited[i] + game.getWidth(), 0);
	}

	cells = std::map<int, HWND>();

	// draw buttons
	int marginTop = MARGIN_V + CELL_SIZE * 2;

	for (int i = 0; i < game.getHeight(); i++) {
		for (int j = 0; j < game.getWidth(); j++) {
			int x = MARGIN_HOR + j * CELL_SIZE;
			int y = marginTop + i * CELL_SIZE;
			int id = getComponentCode(i, j);

			HWND button = CreateWindowExA(
				0,
				"CellClass",
				"",
				WS_CHILD | WS_VISIBLE,
				x, y,
				CELL_SIZE, CELL_SIZE,
				hMainWnd,
				(HMENU)id,
				GetModuleHandle(0),
				0
			);

			cells.insert(std::pair<int, HWND>(id, button));
		}
	}

	//draw reset button
	int resetWidth = FIELD_SIZE / 2;
	int resetTop = MARGIN_V + CELL_SIZE * 2 + FIELD_SIZE + CELL_SIZE;

	CreateWindowA(
		"button",
		"Новая игра",
		WS_CHILD | WS_VISIBLE | BS_FLAT | BS_PUSHBUTTON,
		MARGIN_HOR, resetTop,
		resetWidth, CELL_SIZE,
		hMainWnd,
		(HMENU)RESET_ID,
		NULL,
		NULL
	);
}

// Прототип функции обработки сообщений
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	int code = LOWORD(wParam);
	std::map<int, HWND>::iterator it;
	std::string s;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (uMsg) {
	case WM_PAINT: // если нужно нарисовать, то:
		hdc = BeginPaint(hWnd, &ps);

		renderGameResultLabel(hdc);
		renderBombCountLabel(hdc);

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY: // если окошко закрылось, то:
		PostQuitMessage(NULL); // отправляем WinMain() сообщение WM_QUIT
		break;
	case WM_COMMAND:
		switch (code) {
		case RESET_ID:
			handleReset();
			break;
		default:
			handleCellClick(code, true);
		}

		return 0;
	case WM_NOTIFY: // Типа правая кнопка мыши
		handleCellRightClick(code);

		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam); // если закрыли окошко
	}

	return NULL; // возвращаем значение
}

void renderGameResultLabel(HDC hdc) {
	int totalWidth = game.getWidth() * CELL_SIZE;
	labelResultRect.left = MARGIN_HOR;
	labelResultRect.top = MARGIN_V;
	labelResultRect.right = MARGIN_HOR + totalWidth;
	labelResultRect.bottom = MARGIN_V + CELL_SIZE;

	if (!gameIsOver) {
		return;
	}

	COLORREF color;
	LPCSTR text;
	int textLength;
	if (gameIsOver == 1) {
		color = RGB(0, 200, 0);
		text = "Победа!";
		textLength = 7;
	}
	else {
		color = RGB(200, 0, 0);
		text = "Поражение!";
		textLength = 10;
	}

	HFONT font = CreateFont(RESULT_FONT_SIZE, 0, 0, 0, 700, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"Arial");
	SetBkMode(hdc, TRANSPARENT);
	SelectObject(hdc, font);
	SetTextColor(hdc, color);

	DrawTextA(
		hdc,
		text,
		textLength,
		&labelResultRect,
		DT_CENTER | DT_SINGLELINE | DT_VCENTER
	);
}

void renderBombCountLabel(HDC hdc) {
	int left = max(game.getMineCount() - flagCount, 0);

	bombCountRect.left = MARGIN_HOR + FIELD_SIZE / 2;
	bombCountRect.top = MARGIN_V + CELL_SIZE * 2 + FIELD_SIZE + CELL_SIZE;
	bombCountRect.right = SCREEN_WIDTH - MARGIN_HOR;
	bombCountRect.bottom = SCREEN_HEIGHT - MARGIN_V;

	HFONT font = CreateFont(BOMB_FONT_SIZE, 0, 0, 0, 300, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"Arial");
	SetBkMode(hdc, TRANSPARENT);
	SelectObject(hdc, font);
	SetTextColor(hdc, RGB(200, 200, 200));

	std::string text = "Мин осталось: " + std::to_string(left);

	DrawTextA(
		hdc,
		text.c_str(),
		text.length(),
		&bombCountRect,
		DT_RIGHT | DT_SINGLELINE | DT_VCENTER
	);
}

void handleReset() {
	for (std::map<int, HWND>::iterator it = cells.begin(); it != cells.end(); ++it) {
		HWND button = it->second;
		DestroyWindow(button);
	}

	drawMinesweeperControls(mainWindow);
	InvalidateRect(mainWindow, NULL, TRUE);
	UpdateWindow(mainWindow);
}

void handleCellClick(int code, bool recurcive=true) {
	if (gameIsOver) {
		return;
	}

	std::pair<int, int> yx = parseComponentCode(code);
	int y = yx.first;
	int x = yx.second;

	if (visited[y][x] == 1 && recurcive) {
		int flagsAround = 0;
		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				if ((i != 0 || j != 0) && game.inField(y + i, x + j) && visited[y + i][x + j] == 2) {
					flagsAround++;
				}
			}
		}

		if (flagsAround != game.getCell(y, x)) {
			return;
		}

		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				if ((i != 0 || j != 0) && game.inField(y + i, x + j)) {
					handleCellClick(getComponentCode(y + i, x + j), false);
				}
			}
		}
	}

	if (visited[y][x] != 0) {
		return;
	}

	HWND button = cells.at(code);
	int value = game.getCell(y, x);

	if (value == -1) {
		gameLost();

		return;
	}

	if (value == 0) {
		removeEmptyCells(y, x);

		return;
	}
	else {
		visitCell(y, x);
	}

	std::string text = std::to_string(value);
	SetWindowTextA(button, text.c_str());
}

void visitCell(int y, int x) {
	if (visited[y][x] == 1) {
		return;
	}

	if (visited[y][x] == 2) {
		flagCount--;
		InvalidateRect(mainWindow, &bombCountRect, TRUE);
	}

	visited[y][x] = 1;
	visitedCount++;

	if (game.getWidth() * game.getHeight() == visitedCount + game.getMineCount()) {
		gameWon();
	}
}

void handleCellRightClick(int code) {
	if (gameIsOver) {
		return;
	}

	std::pair<int, int> yx = parseComponentCode(code);
	int visit = visited[yx.first][yx.second];

	if (visit == 1) {
		return;
	}

	HWND button = cells.at(code);
	int value = game.getCell(yx.first, yx.second);

	if (visit == 0) {
		visited[yx.first][yx.second] = 2;
		flagCount++;

		SetWindowText(button, L"⚑");
	}
	else { // 2
		visited[yx.first][yx.second] = 0;
		flagCount--;

		SetWindowTextA(button, "");
	}

	InvalidateRect(mainWindow, &bombCountRect, TRUE);
}

void removeEmptyCells(int y, int x) {
	visitCell(y, x);

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
			if (game.inField(y + i, x + j) && visited[y + i][x + j] != 1) {
				removeEmptyCells(y + i, x + j);
			}
		}
	}
}

void gameLost() {
	std::pair<int, int>* mines = game.getMines();

	for (int i = 0; i < game.getMineCount(); i++) {
		int code = getComponentCode(mines[i].first, mines[i].second);
		HWND button = cells.at(code);

		SetWindowText(button, L"✖");
	}

	gameIsOver = 2;

	InvalidateRect(mainWindow, &labelResultRect, TRUE);
}

void gameWon() {
	gameIsOver = 1;

	InvalidateRect(mainWindow, &labelResultRect, TRUE);
}

int getComponentCode(int y, int x) {
	return y * CODE_BASE + x;
}

std::pair<int, int> parseComponentCode(int code) {
	int y = code / CODE_BASE;
	int x = code % CODE_BASE;

	return std::pair<int, int>(y, x);
}