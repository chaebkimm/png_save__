#include<stdio.h>
#include<stdlib.h>
#include"ico.h"


static FILE* file = NULL;
static unsigned int current_pos;
static struct icondirentry info;

int png_read_ico(const char* filename, struct icondirentry* info);

static void write(unsigned int val, int len) {
	current_pos += len;
	while (len--) {
		fputc(val & 0xff, file);
		val >>= 8;
	}
	printf("\nwrite: current_pos = %d\n", current_pos);
}

static void encode_ICONDIR() {

	printf("\nWrite according to ICONDIR structure \n");

	write(0, 2); // reserved
	write(1, 2); // .ico image
	write(1, 2); // number of images
}

static void encode_ICONDIRENTRY() {
	printf("\nWrite info of the image according to ICONDIRENTRY structure \n");

	write(info.width, 1); // image width
	write(info.height, 1); // image height
	write(info.n_color_plt, 1); // number of colors in palette
	write(0, 1); // reserved
	write(info.color_planes, 2); // if there is color planes
	write(info.depth, 2); // bits per pixel
	write(info.data_size, 4); // img size
	
	printf("\ncurrent_pos: %d\n", current_pos);
	info.data_pos = current_pos + 4;
	write(info.data_pos, 4); // img offset
}


static void ico_open(const char* filename) {
	errno_t error = fopen_s(&file, filename, "wb");

	if (error) {
		printf("File open error: error code %d", error);
		exit(1);
	}
	else {
		printf("File opened: %s\n", filename);
	}

	current_pos = 0;

	encode_ICONDIR();

	encode_ICONDIRENTRY();
}

static void ico_pngs(const char* png_filename) {
	FILE* png;
	errno_t error = fopen_s(&png, png_filename, "rb");

	if (error) {
		printf("File open error: error code %d", error);
		exit(1);
	}
	else {
		printf("File opened: %s\n", png_filename);
	}

	int val = 0;
	do {
		val = fgetc(png);
		write(val, 1);
	} while (val != EOF);

	fclose(png);
}

void check_info() {
	printf(
		"\ninfo: width %d, height %d, n_color_plt %d, color_planes %d, depth %d, data_size %d, data_pos %d\n",
		info.width,
		info.height,
		info.n_color_plt,
		info.color_planes,
		info.depth,
		info.data_size,
		info.data_pos
	);
}
extern int ico_write(const char* filename, const char* png_filename) {
	png_read_ico(png_filename, &info);
	check_info();
	ico_open(filename);
	ico_pngs(png_filename);
	fclose(file);
	return 1;
}