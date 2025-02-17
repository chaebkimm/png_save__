#include<stdio.h>
#include<stdlib.h>
#include"ico.h"

static FILE* img = NULL;
static unsigned int current_pos;

static void read(unsigned char* buf, int offset, unsigned int len) {
	while (len-- > 0) {
		buf[offset++] = fgetc(img);
		current_pos++;
	}
	printf("\nread: current_pos = %d\n", current_pos);
}

static unsigned char read1() {
	current_pos++;
	printf("\nread: current_pos = %d\n", current_pos);

	return (unsigned char)fgetc(img);
}

static int check_signature() {
	const unsigned char sig[8] = { 137, 'P', 'N', 'G', 13, 10, 26, 10 };

	for (int i = 0; i < 8; i++) {
		if (sig[i] != read1()) {
			printf("check png signature: failed\n");
			return 1;
		}
	}
	printf("png signature checked\n");
	return 0;
}

static void png_open(const char* filename) {
	errno_t error = fopen_s(&img, filename, "rb");
	current_pos = 0;
	int error_signature = check_signature();
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

static unsigned int decode_int(unsigned char* buf) {
	int len = 4;
	int ret = 0;
	while (len--) {
		ret = (ret << 8) | *(buf++);
	}
	return ret;
}

static void png_header() {
	const unsigned int n_bits = 13;
	unsigned char buf[5] = { 0 };

	read(buf, 0, 4);
	printf("\nChunk size: %d", decode_int(buf));

	read(buf, 0, 4);
	printf("\nChunk type: %s", buf);

	read(buf, 0, 4);
	printf("\nImg width: %d", decode_int(buf));

	read(buf, 0, 4);
	printf("\nImg height: %d", decode_int(buf));

	printf("\nBit depth: %d", read1());
	printf("\nColor type: %d", read1());
	printf("\nCompression method: %d", read1());
	printf("\nFilter method: %d", read1());
	printf("\nInterlace method: %d", read1());

	read(buf, 0, 4);
	printf("\nCRC: %x\n", decode_int(buf));
}

static int compare_char_ignore_case(char f, char s) {
	if (f >= 'a' && f <= 'z') {
		f -= 'a' - 'A';
	}
	if (s >= 'a' && s <= 'z') {
		s -= 'a' - 'A';
	}
	return s - f;
}

static int compare_type(unsigned char* buf, const char* type) {
	for (int i = 0; i < 4; i++) {
		if (compare_char_ignore_case((char)buf[i], type[i])) {
			return i + 1;
		}
	}
	return 0;
}

static void png_header_ico(struct icondirentry* info) {
	const unsigned int n_bits = 13;
	unsigned char buf[5] = { 0 };

	read(buf, 0, 4);
	if (decode_int(buf) != n_bits) {
		printf("Error: wrong IHDR size: %d\n", decode_int(buf));
	}

	read(buf, 0, 4);
	if (compare_type(buf, "IHDR")) {
		printf("Error: IHDR type expected\n");
	}

	read(buf, 0, 4);
	info->width = decode_int(buf);

	read(buf, 0, 4);
	info->height = decode_int(buf);

	info->depth = read1();

	int color_type = read1();
	if (color_type == 3) {
		info->color_planes = 1;
	}
	else if (color_type == 6) {
		info->color_planes = 1;
		info->depth = info->depth * 4;
	}
	else if (color_type == 2) {
		info->color_planes = 1;
		info->depth = info->depth * 3;
	}
	else {
		info->color_planes = 0;
	}
	printf("\nCompression method: %d", read1());
	printf("\nFilter method: %d", read1());
	printf("\nInterlace method: %d", read1());

	read(buf, 0, 4);
	printf("\nCRC: %x\n", decode_int(buf));
}


static void png_data() {
	unsigned char len[4] = { 0 };
	unsigned char type[5] = { 0 };
	unsigned char crc[4] = { 0 };
	
	while (compare_type(type, "IEND")) {
		read(len, 0, 4);
		int length = decode_int(len);
		printf("\n\nChunk length: %d\n", length);

		read(type, 0, 4);
		printf("Chunk type: %s\n", type);
		
		int pos = 0;
		while (pos < length) {
			read1();
			pos++;
		}

		read(crc, 0, 4);
		int crc_value = decode_int(crc);
		printf("CRC: %x\n", crc_value);
	}
}

static void png_data_ico(struct icondirentry* info) {
	unsigned char len[4] = { 0 };
	unsigned char type[5] = { 0 };
	unsigned char crc[4] = { 0 };
	printf("\npng_data_ico\n");
	while (1) {
		read(len, 0, 4);
		int length = decode_int(len);
		printf("\n\nChunk length: %d\n", length);

		read(type, 0, 4);
		printf("Chunk type: %s\n", type);

		int pos = 0;
		while (pos < length) {
			read1();
			pos++;
		}

		read(crc, 0, 4);
		int crc_value = decode_int(crc);
		printf("CRC: %x\n", crc_value);

		if (compare_type(type, "PLTE") == 0) {
			info->n_color_plt = length / 3;
		}
		if (compare_type(type, "IEND") == 0) {
			info->data_size = current_pos;
			return;
		}
	}
}

static void png_end() {
	unsigned char type[4] = "IEND";
	unsigned int data_offset = 0;
	unsigned long crc = get_crc(type, 4, 0);

	png_write_int(data_offset);
	png_write(type, 4);
	png_write_int(crc & 0xffffffff);
}

extern int png_read(const char* filename) {
	png_open(filename);
	png_header();
	png_data();
	fclose(img);
	return 1;
}

extern int png_read_ico(const char* filename, struct icondirentry* info) {
	png_open(filename);
	png_header_ico(info);
	png_data_ico(info);
	fclose(img);
	return 1;
}
