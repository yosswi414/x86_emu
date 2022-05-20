#include <stdio.h>
#include <stdlib.h>

// bits/stdint-uintn.h
typedef unsigned char uint8_t;

int main(void) {
    int8_t s_val = -41;
    uint8_t u_val = s_val;
    printf("  signed = %d (%x)\nunsigned = %d (%x)\n", s_val, s_val, u_val, u_val);
    return 0;
}