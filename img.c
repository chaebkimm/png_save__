#include <stdio.h>
#include "png.h"

void make_png_from_cifar_100() {
    FILE* cifar;
    errno_t error;
    error = fopen_s(&cifar, "./cifar-100-binary./train.bin", "rb");

    if (!cifar) return 1;

    printf("%d", error);
    unsigned char c;

    c = fgetc(cifar);
    printf("%d", c);

    c = fgetc(cifar);
    printf("%d", c);

    unsigned char rgbs[32 * 32 * 3];

    for (int i = 0; i < 32 * 32; i++) {
        rgbs[i * 3] = fgetc(cifar);
    }

    for (int i = 0; i < 32 * 32; i++) {
        rgbs[i * 3 + 1] = fgetc(cifar);
    }

    for (int i = 0; i < 32 * 32; i++) {
        rgbs[i * 3 + 2] = fgetc(cifar);
    }

    png_save("test.png", 32, 32, rgbs);

    fclose(cifar);
}

void make_small_c_img() {
    unsigned char rgbs[4 * 3 * 3];

    unsigned char light_red[3] = { 0x55, 0x00, 0x00 };
    unsigned char white[3] = { 0xff, 0xff, 0xff };

    unsigned char c_pattern[4][3] = {
        {0, 1, 1},
        {1, 0, 0},
        {1, 0, 0},
        {0, 1, 1}
    };

    int pos = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            unsigned char* color;
            if (c_pattern[i][j] == 0) color = white;
            else color = light_red;
            for (int n = 0; n < 3; n++) rgbs[pos++] = color[n];
        }
    }

    rgbs[0] = 255;
    rgbs[1] = 0;
    rgbs[2] = 0;

    for (int i = 0; i < 4 * 3 * 3; i++)
        printf("%d ", rgbs[i]);

    png_save("test.png", 3, 4, rgbs);
}

void minimal_img() {
    unsigned char rgbs[3 * 2 * 2];

    rgbs[0] = 200;
    rgbs[1] = 0;
    rgbs[2] = 0;

    rgbs[3] = 255;
    rgbs[4] = 0;
    rgbs[5] = 0;

    rgbs[6] = 100;
    rgbs[7] = 0;
    rgbs[8] = 0;

    rgbs[9] = 50;
    rgbs[10] = 0;
    rgbs[11] = 0;

    png_save("test.png", 2, 2, rgbs);
}

void pixel(int red, int green, int blue) {

    unsigned char rgbs[3];

    rgbs[0] = red & 0xff;
    rgbs[1] = green & 0xff;
    rgbs[2] = blue & 0xff;

    png_save("test.png", 1, 1, rgbs);
}