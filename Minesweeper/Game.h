#pragma once

class Game {
private:
	int** field;
	int _width;
	int _height;
	int _mines;

	void createField();
	void putMine(int y, int x);
	bool inField(int y, int x);

public:
	Game(int width, int height, int mines);

	int getWidth();
	int getHeight();
	int getMineCount();
	int getFeild(int y, int x);
};
