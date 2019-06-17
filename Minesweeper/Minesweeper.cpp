#include <windows.h>
#include "Minesweeper.h"

TCHAR MINESWEEPER_CLASSNAME[] = L"Minesweeper";

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

WNDCLASSEX createMinesweeperWindow(HINSTANCE hInst) {
	WNDCLASSEX wc; // создаЄм экземпл€р, дл€ обращени€ к членам класса WNDCLASSEX
	wc.cbSize = sizeof(wc); // размер структуры (в байтах)
	wc.style = CS_HREDRAW | CS_VREDRAW; // стиль класса окошка
	wc.lpfnWndProc = WndProc; // указатель на пользовательскую функцию
	wc.lpszMenuName = NULL; // указатель на им€ меню (у нас его нет)
	wc.lpszClassName = MINESWEEPER_CLASSNAME; // указатель на им€ класса
	wc.cbWndExtra = NULL; // число освобождаемых байтов в конце структуры
	wc.cbClsExtra = NULL; // число освобождаемых байтов при создании экземпл€ра приложени€
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO); // декриптор пиктограммы
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO); // дескриптор маленькой пиктограммы (в трэе)
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); // дескриптор курсора
	wc.hbrBackground = CreateSolidBrush(RGB(50, 50, 50));; // дескриптор кисти дл€ закраски фона окна
	wc.hInstance = hInst; // указатель на строку, содержащую им€ меню, примен€емого дл€ класса

	return wc;
}

// ѕрототип функции обработки сообщений
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_PAINT: // если нужно нарисовать, то:
		break;
	case WM_DESTROY: // если окошко закрылось, то:
		PostQuitMessage(NULL); // отправл€ем WinMain() сообщение WM_QUIT
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam); // если закрыли окошко
	}

	return NULL; // возвращаем значение
}