#include <stdio.h>
#include <math.h>
#include <functional>
#include <conio.h>
#include <windows.h>
#include <iostream>
#include <thread>

class Block {
public:
	Block():weight(1),speed(0),height(0){}
	Block(double w, double h):weight(w),speed(0),height(h){}
	void Update() {
		height += speed;
	}
	void set_weight(double w) { weight = w; }
	double get_weight() { return weight; }
	void set_height(double h) { height = h; }
	double get_height() { return height; }
	void add_speed(double s) { speed += s; }
	double get_speed() { return speed; }
private:
	double weight, speed, height;
};

class Wave {
public:
	Wave() :width(50), height(50) { Init(); }
	Wave(unsigned w, unsigned h) :width(w), height(h) { Init(); }
	~Wave() { delete[] wave; }
	void Update() {
		for (int i = 1; i < width - 1; ++i)
			for (int j = 1; j < height - 1; ++j)
				Block_UpdateSpeed(i, j);
		for (int i = 1; i < width - 1; ++i)
			for (int j = 1; j < height - 1; ++j)
				Block_Update(i, j);
	}
	void Circle(int x, int y, int r) {
		for (int i = x - r; i <= x + r; ++i)
			for (int j = y - r; j <= y + r; ++j)
				if (pow(i - x,2) + pow(j - y, 2) <= r * r) wave[i][j].set_height(1 - ((pow(i - x, 2) + pow(j - y, 2)) / (r * r)));
	}
	void Dot(int x, int y) {
		wave[x][y].set_height(10);
	}
	double operator()(int x, int y) { return wave[x][y].get_height(); }
private:
	void Init() {
		wave = new Block*[width + 2];
		for (int i = 0; i < width; ++i)
			wave[i] = new Block[height + 2];
		for (int j = 0; j < height; ++j)
			wave[0][j].set_weight(9999), wave[width - 1][j].set_weight(9999);
		for (int j = 0; j < width; ++j)
			wave[j][0].set_weight(9999), wave[j][height - 1].set_weight(9999);
	}
	void Block_UpdateSpeed(int x, int y) {
		double left = wave[x - 1][y].get_height();
		double up = wave[x][y - 1].get_height();
		double right = wave[x + 1][y].get_height();
		double down = wave[x][y + 1].get_height();
		double sum = (left + down + right + up);
		wave[x][y].add_speed((sum / 4 - wave[x][y].get_height()) / wave[x][y].get_weight());
	}
	void Block_Update(int x, int y) {
		wave[x][y].Update();
	}
	Block** wave;
	unsigned width, height;
};

void SetWindow(int Width, int Height)
{
	_COORD coord;
	coord.X = Width;
	coord.Y = Height;
	_SMALL_RECT Rect;
	Rect.Top = 0;
	Rect.Left = 0;
	Rect.Bottom = Height - 1;
	Rect.Right = Width - 1;
	HANDLE Handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleScreenBufferSize(Handle, coord);
	SetConsoleWindowInfo(Handle, TRUE, &Rect);
}

int main() {
	int width = 120 * 2;
	int height = 30 * 2;
	SetWindow(width, height);
	float aspect = (float)width / height;
	float pixelAspect = 11.0f / 24.0f;
	aspect *= pixelAspect;
	char gradient[] = " .:!/r(l1Z4H9W8$@";
	char invgradient[] = "@$8W9H4Z1l(r/!:. ";
	int gradientSize = std::size(gradient) - 2;

	wchar_t* screen = new wchar_t[width * height];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	Wave map(width,height);
	map.Circle(100, 30, 10);
	map.Circle(140, 30, 10);
	int frames = 0;
	while (1) {
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				int index = map(i, j) / 0.1;
				char pixel = gradient[min(max(index + 2, 1), 16)];
				screen[i + j * width] = pixel;
			}
		}
		screen[width * height - 1] = '\0';
		WriteConsoleOutputCharacterW(hConsole, screen, width * height, { 0, 0 }, &dwBytesWritten);
		map.Update();
		Sleep(10);
		++frames;
	}
}