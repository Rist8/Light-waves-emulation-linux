#include <SDL2/SDL.h>
#include <math.h>
#include <iostream>

struct Block {
	Block(double w = 1, double h = 0) :weight(w), speed(0), height(h) {}
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
				if (pow(i - x, 2) + pow(j - y, 2) <= r * r)
					wave[i][j].height = 10;
	}

	void ImpulseRectangle(int x, int y, int a, int b) {
		for (int i = x; i < x + a; ++i)
			for (int j = y; j < y + b; ++j)
				if (i == x || j == y || i == x + a - 1 || j == y + b - 1)
					wave[i][j].height = (1);
	}

	void ObjectFilledCircle(int x, int y, int r) {
		for (int i = x - r; i <= x + r; ++i)
			for (int j = y - r; j <= y + r; ++j)
				if (pow(i - x, 2) + pow(j - y, 2) <= r * r)
					wave[i][j].weight = (INFINITY);
	}

	void ObjectFilledRectangle(int x, int y, int a, int b) {
		for (int i = x; i < x + a; ++i)
			for (int j = y; j < y + b; ++j)
				wave[i][j].weight = (INFINITY);
	}

	void ObjectRectangle(int x, int y, int a, int b) {
		for (int i = x; i < x + a; ++i)
			for (int j = y; j < y + b; ++j)
				if (i == x || j == y || i == x + a - 1 || j == y + b - 1)
					wave[i][j].weight = (INFINITY);
	}

	void ImpulseDot(int x, int y) {
		wave[x][y].height = (10);
	}

	inline double operator()(const int x, const int y) { return wave[x][y].height; }
	inline double operator()(const int x, const int y, bool) { return wave[x][y].weight; }
private:
	void Init() {
		wave = new Block * [width + 2];
		for (int i = 0; i < width; ++i)
			wave[i] = new Block[height + 2];
		for (int j = 0; j < height; ++j)
			wave[0][j].weight = (INFINITY), wave[width - 1][j].weight = (INFINITY);
		for (int j = 0; j < width; ++j)
			wave[j][0].weight = (INFINITY), wave[j][height - 1].weight = (INFINITY);
	}

	void Block_UpdateSpeed(int x, int y) {
		static double left, up, right, down, sum;
		left = wave[x - 1][y].height;
		up = wave[x][y - 1].height;
		right = wave[x + 1][y].height;
		down = wave[x][y + 1].height;
		sum = (left + down + right + up);
		wave[x][y].speed += ((sum / 4 - wave[x][y].height) / wave[x][y].weight);
	}

	void Block_Update(int x, int y) {
		wave[x][y].height += wave[x][y].speed;
	}

	Block** wave;
	unsigned width, height;
};


int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		return 1;
	SDL_Window* window = SDL_CreateWindow("WAVRS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1920, 1080, SDL_WINDOW_FULLSCREEN);
	if (window == NULL)
		return 1;
	SDL_Surface* window_surface = SDL_GetWindowSurface(window);
	uint32_t* screen = (uint32_t*)window_surface->pixels;

	int width = window_surface->w;
	int height = window_surface->h;

	Wave map(width, height);
	map.ImpulseCircle(width / 2 - 200, height / 2, 100);
	map.ImpulseCircle(width / 2 + 200, height / 2, 100);
	//map.ImpulseDot(0, 0);
	//map.ImpulseDot(50, 30);
	SDL_Event event;
	
	while (1) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) { exit(0); }
		}
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				uint8_t index = std::min(abs((int)map(i, j)) * 100, 255);
				uint32_t pixel = SDL_MapRGBA(window_surface->format, index, index, index, 255);
				if (map(i, j, true) == INFINITY)
					pixel = SDL_MapRGBA(window_surface->format, 255, 0, 0, 255);
				screen[i + j * width] = pixel;
			}
		}
		SDL_UpdateWindowSurface(window);
		map.Update();
		//Sleep(10);
	}
}
