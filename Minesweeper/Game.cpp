#include <random>
#include <iostream>
#include "Game.h"

Game::Game(int width, int height, int mines) {
	_width = width;
	_height = height;
	mineCount = mines;

	createField();
}

//PUBLIC 

int Game::getWidth() {
	return _width;
}

int Game::getHeight() {
	return _height;
}

int Game::getMineCount() {
	return mineCount;
}

int Game::getFeild(int y, int x) {
	return field[y][x];
}

std::pair<int, int>* Game::getMines() {
	return mines;
}

// PRIVATE

void Game::createField() {
	field = new int*[_height];
	for (int i = 0; i < _height; i++) {
		field[i] = new int[_width];
		std::fill(field[i], field[i] + _width, 0);
	}

	mines = new std::pair<int, int>[mineCount];
	for (int t = 0; t < mineCount; t++) {
		int y = rand() % _height;
		int x = rand() % _width;
		if (field[y][x] != -1) {
			putMine(y, x);
			mines[t] = std::pair<int, int>(y, x);
		}
		else {
			t--;
		}
	}
}

void Game::putMine(int y, int x) {
	field[y][x] = -1;
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if (inField(y + i, x + j) && field[y + i][x + j] != -1)
				field[y + i][x + j]++;
		}
	}
}

bool Game::inField(int y, int x) {
	return y >= 0 && y < _height && x >= 0 && x < _width;
}