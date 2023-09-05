#include <stdio.h>
#include <iostream>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <linux/input.h>
#include <thread>

class Block {
public:
	Block() :weight(1), speed(0), height(0) {}
	Block(double w, double h) :weight(w), speed(0), height(h) {}
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
				if (pow(i - x, 2) + pow(j - y, 2) <= r * r) wave[i][j].set_height(1 - ((pow(i - x, 2) + pow(j - y, 2)) / (r * r)));
	}
	double operator()(int x, int y) { return wave[x][y].get_height(); }
private:
	void Init() {
		wave = new Block * [width + 2];
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

//may return 0 when not able to detect
const auto processor_count = std::thread::hardware_concurrency();

#define X_SIZE 1920
#define Y_SIZE 1080
#define SCREEN_SIZE X_SIZE * Y_SIZE
#define BUF_SIZE SCREEN_SIZE * sizeof(uint32_t)

typedef struct __attribute((packed))__ {
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t a;
} Pixel;

typedef union {
	Pixel pixel;
	uint32_t raw;
} PixelUnion;

int main(int argc, char** argv)
{
	int fd = open("/dev/fb0", O_RDWR);
	char* screen = (char*)(mmap(NULL, BUF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
	uint32_t* ptr = (uint32_t*)screen;
	uint32_t ptr1[BUF_SIZE / 4];
	PixelUnion p = { 0 };
	p.pixel.r = p.pixel.g = p.pixel.b = 0x80;
	p.pixel.a = 0xFF;
	Wave map(X_SIZE, Y_SIZE);
	map.Circle(100, 30, 10);
	map.Circle(140, 30, 10);
	int frames = 0;
	while (1) {
		for (int i = 0; i < X_SIZE; i++) {
			for (int j = 0; j < Y_SIZE; j++) {
				int index = map(i, j) / 0.1;
				p.pixel.r = p.pixel.g = p.pixel.b = (0x80 + index * 10);
				ptr1[i + j * X_SIZE] = p.raw;
			}
		}
		memcpy(ptr, ptr1, BUF_SIZE);
		map.Update();
		++frames;
	}
}

