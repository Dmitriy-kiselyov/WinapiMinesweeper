#include <windows.h>
#include "Minesweeper.h"

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