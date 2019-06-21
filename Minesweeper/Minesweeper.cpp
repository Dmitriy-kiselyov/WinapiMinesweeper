﻿#include <windows.h>
#include "Minesweeper.h"
#include "Game.h"
#include "Cell.h"
#include <iostream>
#include <string>
#include <map>

TCHAR MINESWEEPER_CLASSNAME[] = L"Minesweeper";
const int RESET_ID = 1000;

std::map<int, HWND> cells;
HWND mainWindow;
Game game(1, 1, 0);
int** visited; // 0 - пусто, 1 - открыта, 2 - стоит флаг
bool gameIsOver;
int flagCount;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void handleReset();
void handleCellClick(int code);
void handleCellRightClick(int code);
void removeEmptyCells(int y, int x);
void gameOver();
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

	game = Game(8, 8, 5);
	gameIsOver = false;
	flagCount = 0;

	visited = new int*[game.getHeight()];
	for (int i = 0; i < game.getWidth(); i++) {
		visited[i] = new int[game.getWidth()];
		std::fill(visited[i], visited[i] + game.getWidth(), 0);
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

			HWND button = CreateWindowExA(
				0,
				"CellClass",
				"",
				WS_CHILD | WS_VISIBLE,
				x, y,
				cellSize, cellSize,
				hMainWnd,
				(HMENU)id,
				GetModuleHandle(0),
				0
			);

			cells.insert(std::pair<int, HWND>(id, button));
		}
	}

	//draw reset button
	int resetWidth = game.getWidth() * (gapX + cellSize) - gapX;
	int resetTop = marginTop + game.getHeight() * (gapY + cellSize) + cellSize;

	CreateWindowA(
		"button",
		"Новая игра",
		WS_CHILD | WS_VISIBLE | BS_FLAT | BS_PUSHBUTTON,
		marginLeft, resetTop,
		resetWidth, cellSize,
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

	switch (uMsg) {
	case WM_PAINT: // если нужно нарисовать, то:
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
			handleCellClick(code);
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

void handleReset() {
	for (std::map<int, HWND>::iterator it = cells.begin(); it != cells.end(); ++it) {
		HWND button = it->second;
		DestroyWindow(button);
	}

	drawMinesweeperControls(mainWindow);
	UpdateWindow(mainWindow);
}

void handleCellClick(int code) {
	if (gameIsOver) {
		return;
	}

	std::pair<int, int> yx = parseComponentCode(code);

	if (visited[yx.first][yx.second] != 0) {
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
}

void removeEmptyCells(int y, int x) {
	visited[y][x] = 1;

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