#include<stdio.h>
#include<stdlib.h>
#include"ico.h"
static FILE* file = NULL;
static unsigned int current_pos;

static unsigned int n_img;

static struct icondirentry* icons_info;

static void read(unsigned char* buf, int offset, unsigned int len) {
	if (!file) exit(1);
	while (len-- > 0) {
		int c = fgetc(file);
		current_pos++;

		if (c < 0 || c > 0xff) {
			printf("Error in read(): fgetc got %d\n", c);
			exit(1);
		}

		buf[offset++] = (unsigned char) (c & 0xff);
	}
}

static void print_char(unsigned char c) {
	if (c >= 'a' && c <= 'z') {
		printf("%c", c);
	}
	else if (c >= 'A' && c <= 'Z') {
		printf("%c", c);
	}
	else {
		printf("%02x", c);
	}
	printf(" ");
}

static void print(unsigned char* str, int len) {
	while (len--) {
		print_char(*(str++));
	}
	printf("\n");
}


static unsigned int decode(unsigned char** buf, int len) {
	unsigned int ret = 0;
	unsigned int pos = 0;
	while (len--) {
		ret |= **buf << pos;
		pos += 8;
		(*buf)++;
	}
	return ret;
}

static void print_value(int* offset, int size, int value, const char* msg) {
	printf("Offset: %2d   Size: %2d   Value: %2d   Meaning: %s \n", *offset, size, value, msg);
	offset += size;
}

static void print_value_hex(int* offset, int size, int value, const char* msg) {
	printf("Offset: %2d   Size: %2d   Value: %2x   Meaning: %s \n", *offset, size, value, msg);
	offset += size;
}


static void decode_ICONDIR() {

	printf("\nRead according to ICONDIR structure \n");

	unsigned char buf[6] = { 0 };
	read(buf, 0, 6);

	unsigned char* it = buf;
	int offset = 0;
	
	int reserved = decode(&it, 2);
	print_value(&offset, 2, reserved, "should be 0");

	int img_type = decode(&it, 2);
	print_value(&offset, 2, img_type, "1 is icon(.ICO) image");

	n_img = (unsigned int) decode(&it, 2);
	print_value(&offset, 2, n_img, "number of images");

	icons_info = (struct icondirentry*)malloc(n_img * sizeof(struct icondirentry));
}


static int decode_byte_0_is_256(unsigned char **it) {
	int ret = decode(it, 1);

	if (ret) return ret;
	else return 256;
}

static void decode_ICONDIRENTRY() {
	printf("\nRead info of %d images according to ICONDIRENTRY structure \n", n_img);

	unsigned char buf[16] = { 0 };

	for (int i = 0; i < n_img; i++) {
		printf("\nImage #%d\n", i);
		struct icondirentry* info = icons_info + i;

		read(buf, 0, 16);
		unsigned char* it = buf;

		int offset = 0;

		info->width = decode_byte_0_is_256(&it);
		print_value(&offset, 1, info->width, "image width");

		info->height = decode_byte_0_is_256(&it);
		print_value(&offset, 1, info->height, "image height");

		info->n_color_plt = decode(&it, 1);
		print_value(&offset, 1, info->n_color_plt, "number of colors in palette");

		int reserved = decode(&it, 1);
		print_value(&offset, 1, info->n_color_plt, "reserved: should be 0");

		info->color_planes = decode(&it, 2);
		print_value(&offset, 2, info->color_planes, "if there is color planes");

		info->depth = decode(&it, 2);
		print_value(&offset, 2, info->depth, "bits per pixel");

		info->data_size = decode(&it, 4);
		print_value_hex(&offset, 4, info->data_size, "size of img data");

		info->data_pos = decode(&it, 4);
		print_value_hex(&offset, 4, info->data_pos, "offset of img data");

	}

}

static void ico_open(const char* filename) {
	errno_t error = fopen_s(&file, filename, "rb");

	if (error) {
		printf("File open error: error code %d", error);
		exit(1);
	}
	else {
		printf("File opened: %s\n", filename);
	}


	decode_ICONDIR();

	decode_ICONDIRENTRY();
}


static int check_signature(unsigned char* sig, unsigned char* buf, int len) {
	while (len--) {
		if (*(sig++) != *(buf++)) {
			printf("check png signature: failed\n");
			return 1;
		}
	}
	printf("png signature checked\n");
	return 0;
}

static void str_add_number(char* str, int n, int offset, int max_size) {
	do {
		if (n < 0) {
			printf("Error in str_add_number: n is negative\n");
			return;
		}

		int digit = n % 10;
		str[offset] = '0' + digit;
		offset += 1;

		if (n < 10) return;
		else n /= 10;

		max_size -= 1;
	} while (max_size > 0);

	printf("Error in str_add_number: max size reached\n");
}

static void fput_int(int i, FILE* img) {
	fputc(i & 0xff, img);
	fputc((i >>= 8) & 0xff, img);
	fputc((i >>= 8) & 0xff, img);
	fputc((i >>= 8) & 0xff, img);
}

static void check_and_write(FILE** img, unsigned int* write_pos, int i) {

	const unsigned char PNG_SIGNITURE[8] = { 137, 'P', 'N', 'G', 13, 10, 26, 10 };

	unsigned char buf[8] = { 0 };

	read(buf, 0, 8);
	print(buf, 8);

	char filename[20] = "ico___.___";
	str_add_number(filename, i, 3, 2);

	int is_bmp = check_signature(PNG_SIGNITURE, buf, 8);

	if (is_bmp) {
		printf("bmp file.. skip\n");
		return;
	}
	else {
		printf("png file..\n");
		filename[7] = 'p';
		filename[8] = 'n';
		filename[9] = 'g';
	}

	errno_t error = fopen_s(img, filename, "wb");

	if (error) {
		printf("File open error: error code %d", error);
		exit(1);
	}
	else {
		printf("file opened: %s\n", filename);
	}

	for (int i = 0; i < 8; i++) {
		fputc(buf[i], *img);
	}
	*write_pos += 8;
}

static void write_and_close(FILE** img, unsigned int* write_pos, const unsigned int end_pos) {

	while (*write_pos < end_pos) {
		unsigned char buf;
		read(&buf, 0, 1);
		fputc((int)buf, *img);
		(*write_pos)++;
	}

	fclose(*img);
}


static void ico_pngs() {
	printf("\nWriting png files\n");
	
	for (int i = 0; i < n_img; i++) {

		printf("Image #%d\n ", i);

		unsigned int data_pos = icons_info[i].data_pos;
		unsigned int data_size = icons_info[i].data_size;

		if (data_pos != current_pos) {
			printf("Error: offset of image #%d is %x, but curront offset is %x", i, data_pos, current_pos);
			exit(1);
		}


		FILE* img;
		unsigned int write_pos = 0;

		check_and_write(&img, &write_pos, i);

		write_and_close(&img, &write_pos, data_size);
	}
}

extern int ico_read(const char* filename) {
	ico_open(filename);
	ico_pngs();
	fclose(file);
	return 1;
}