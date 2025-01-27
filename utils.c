#include <stdlib.h>
#include <limits.h>
#include <math.h>

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