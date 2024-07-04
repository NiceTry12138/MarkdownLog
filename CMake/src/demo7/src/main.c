#include <stdio.h>

int main() {
#ifdef DEBUG
    printf("Hello, World!\n");
#endif
    printf("End!\n");
    return 0;
}