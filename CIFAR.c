#include <stdio.h>
#include "huffman.h"
#include "png.h"

int main() {
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

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
