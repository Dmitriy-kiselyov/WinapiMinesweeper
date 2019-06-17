#include <windows.h>
#include "Minesweeper.h"

// ����������� �������:
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
	HWND hMainWnd; // ������ ���������� �������� ������
	MSG msg; // ����� ��������� ��������� MSG ��� ��������� ���������
	
	WNDCLASSEX wc = createMinesweeperWindow(hInst);

	if (!RegisterClassEx(&wc)) {
		// � ������ ���������� ����������� ������:
		MessageBox(NULL, L"�� ���������� ���������������� �����!", L"������", MB_OK);
		return NULL; // ����������, �������������, ������� �� WinMain
	}

	// �������, ��������� ������:
	hMainWnd = CreateWindow(
		MINESWEEPER_CLASSNAME, // ��� ������
		L"����", // ��� ������ (�� ��� ������)
		WS_OVERLAPPEDWINDOW, // ������ ����������� ������
		CW_USEDEFAULT, // ������� ������ �� ��� �
		NULL, // ������� ������ �� ��� � (��� ������ � �, �� ������ �� �����)
		CW_USEDEFAULT, // ������ ������
		NULL, // ������ ������ (��� ������ � ������, �� ������ �� �����)
		(HWND)NULL, // ���������� ������������� ����
		NULL, // ���������� ����
		HINSTANCE(hInst), // ���������� ���������� ����������
		NULL // ������ �� ������� �� WndProc
	);

	if (!hMainWnd) {
		// � ������ ������������� �������� ������ (�������� ��������� � ��):
		MessageBox(NULL, L"�� ���������� ������� ����!", L"������", MB_OK);
		return NULL;
	}

	ShowWindow(hMainWnd, nCmdShow); // ���������� ������
	UpdateWindow(hMainWnd); // ��������� ������

	while (GetMessage(&msg, NULL, NULL, NULL)) { // ��������� ��������� �� �������, ���������� ���������, ��
		TranslateMessage(&msg); // �������������� ���������
		DispatchMessage(&msg); // ������� ��������� ������� ��
	}

	return msg.wParam; // ���������� ��� ������ �� ����������
}