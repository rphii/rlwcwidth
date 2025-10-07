#include "rlwcwidth.lut.h"

char conversion_tab[] = {
#include "rlwcwidth.lut.data.h"
};

#define RLWCWIDTH_UNICODE_MAX     0x10FFFFULL

char rlwcwidth_lookup(unsigned int point) {
    if(point > RLWCWIDTH_UNICODE_MAX) return -1;
    return conversion_tab[point];
}

