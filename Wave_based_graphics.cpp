#include <math.h>
#include <windows.h>
#include <iostream>

class Block {
public:
	Block(double w = 1, double h = 0):weight(w),speed(0),height(h){}
	void Update() { height += speed; }
	
	double weight, speed, height;

};

class Wave {
public:
	Wave(unsigned w = 50, unsigned h = 50) :width(w), height(h) { Init(); }

	~Wave() { 
		for (int i = 0; i < width; ++i)
			delete[] wave[i];
		delete[] wave; 
	}

	void Update() {
		for (int i = 1; i < width - 1; ++i)
			for (int j = 1; j < height - 1; ++j)
				Block_UpdateSpeed(i, j);
		for (int i = 1; i < width - 1; ++i)
			for (int j = 1; j < height - 1; ++j)
				Block_Update(i, j);
	}

	void ImpulseCircle(int x, int y, int r) {
		for (int i = x - r; i <= x + r; ++i)
			for (int j = y - r; j <= y + r; ++j)
				if (pow(i - x,2) + pow(j - y, 2) <= r * r)
					wave[i][j].height=(1 - ((pow(i - x, 2) + pow(j - y, 2)) / (r * r)));
	}

	void ImpulseRectangle(int x, int y, int a, int b) {
		for (int i = x; i < x + a; ++i)
			for (int j = y; j < y + b; ++j)
				if (i == x || j == y || i == x + a - 1 || j == y + b - 1)
					wave[i][j].height=(1);
	}

	void ObjectFilledCircle(int x, int y, int r) {
		for (int i = x - r; i <= x + r; ++i)
			for (int j = y - r; j <= y + r; ++j)
				if (pow(i - x,2) + pow(j - y, 2) <= r * r)
					wave[i][j].weight=(9999);
	}

	void ObjectFilledRectangle(int x, int y, int a, int b) {
		for (int i = x; i < x + a; ++i)
			for (int j = y; j < y + b; ++j)
				wave[i][j].weight=(9999);
	}

	void ObjectRectangle(int x, int y, int a, int b) {
		for (int i = x; i < x + a; ++i)
			for (int j = y; j < y + b; ++j)
				if(i == x || j == y || i == x + a - 1 || j == y + b - 1)
					wave[i][j].weight=(9999);
	}

	void ImpulseDot(int x, int y) {
		wave[x][y].height=(10);
	}

	double operator()(int x, int y) { return wave[x][y].height; }
	double operator()(int x, int y, bool weight) { return wave[x][y].weight; }
private:
	void Init() {
		wave = new Block*[width + 2];
		for (int i = 0; i < width; ++i)
			wave[i] = new Block[height + 2];
		for (int j = 0; j < height; ++j)
			wave[0][j].weight=(9999), wave[width - 1][j].weight=(9999);
		for (int j = 0; j < width; ++j)
			wave[j][0].weight=(9999), wave[j][height - 1].weight=(9999);
	}

	void Block_UpdateSpeed(int x, int y) {
		static double left, up, right, down, sum;
		left = wave[x - 1][y].height;
		up = wave[x][y - 1].height;
		right = wave[x + 1][y].height;
		down = wave[x][y + 1].height;
		sum = (left + down + right + up);
		wave[x][y].speed+=((sum / 4 - wave[x][y].height) / wave[x][y].weight);
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
	int gradientSize = std::size(gradient);

	wchar_t* screen = new wchar_t[width * height];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
		0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	Wave map(width,height);
	map.ImpulseCircle(100, 30, 10);
	map.ImpulseCircle(140, 30, 10);
	//map.ImpulseDot(50, 20);
	//map.ImpulseDot(50, 30);
	while (1) {
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				int index = map(i, j) / 0.1;
				char pixel = gradient[min(max(abs(index), 0), 16)];
				if (map(i, j, 1) == 9999)
					pixel = '*';
				screen[i + j * width] = pixel;
			}
		}
		screen[width * height - 1] = '\0';
		WriteConsoleOutputCharacterW(hConsole, screen, width * height, { 0, 0 }, &dwBytesWritten);
		map.Update();
		//Sleep(10);
	}
}
