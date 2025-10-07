#include "rlwcwidth.h"
#include "rlwcwidth.lut.h"
#include <stdio.h>

#define UNICODE_MAX     0x10FFFFULL

int main(void) {
    int errcount = 0;
    for(unsigned int i = 0; i <= UNICODE_MAX; ++i) {
        int w1 = rlwcwidth(i);
        int w2 = rlwcwidth_lookup(i);
        if(w1 != w2) {
            errcount++;
            printf("ERR : %u -> w1 %i, w2: %i\n", i, w1, w2);
        } else {
            printf("OK : %u -> w %i\n", i, w1);
        }
    }
    printf("done. %u errors.\n", errcount);
    return 0;
}


