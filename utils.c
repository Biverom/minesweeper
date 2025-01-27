#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>

int numPlaces(int n) {
    if (n < 0) return numPlaces ((n == INT_MIN) ? INT_MAX: -n);
    if (n < 10) return 1;
    return 1 + numPlaces (n / 10);
}

int getNthDigit(int number, int n) {
    int digit = abs(number) / (int)pow(10, n - 1) % 10;
    return digit;
}

void fixpath(char* path) {
    #ifdef _WIN32
        for (int i = 0; path[i] != '\0'; i++) {
            if (path[i] == '/') {
                path[i] = '\\';
            }
        }
    #endif
}

void copyFile(const char *sourceFile, const char *destFile) {
    FILE *src, *dest;
    char ch;

    src = fopen(sourceFile, "r");
    if (src == NULL) {
        printf("Error opening source file %s\n", sourceFile);
        return;
    }

    dest = fopen(destFile, "w");
    if (dest == NULL) {
        printf("Error opening destination file %s\n", destFile);
        fclose(src);
        return;
    }

    while ((ch = fgetc(src)) != EOF) {
        fputc(ch, dest);
    }

    fclose(src);
    fclose(dest);

    printf("File copied successfully from %s to %s\n", sourceFile, destFile);
}
