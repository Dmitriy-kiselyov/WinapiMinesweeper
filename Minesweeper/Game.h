#pragma once
#include <iostream>

class Game {
private:
	int** field;
	int _width;
	int _height;
	int mineCount;
	std::pair<int, int> *mines;

	void createField();
	void putMine(int y, int x);

public:
	Game(int width, int height, int mines);

	int getWidth();
	int getHeight();
	int getMineCount();
	std::pair<int, int>* getMines();
	int getCell(int y, int x);
	bool inField(int y, int x);
};
