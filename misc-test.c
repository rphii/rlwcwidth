#include "rlwcwidth.h"
#include <rlso.h>

#define UNICODE_MAX     0x10FFFFULL

int main(void) {
    So_Uc_Point ucp;
    So tmp = SO;
    for(unsigned int i = 0; i <= UNICODE_MAX; ++i) {
        ucp.val = i;
        so_clear(&tmp);
        so_fmt(&tmp, "%u:", rlwcwidth(i));
        so_uc_fmt_point(&tmp, &ucp);
        so_fmt(&tmp, " ");
        int x0, y0, xE, yE;
        so_print(tmp);
    }
}

