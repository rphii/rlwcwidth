#include "rlwcwidth.lut.h"
#include <rlso.h>

#define UNICODE_MAX     0x10FFFFULL

int main(void) {
    So out = SO;
    So tmp = SO;
    size_t i0 = 0;
    size_t iE = 0;
    char prev = 0;
    So_Uc_Point ucp;
    for(unsigned int i = 0; i <= UNICODE_MAX; ++i) {
        char now = rlwcwidth_lookup(i);
        /// if(i > 30) {
        ///     ucp.val = i;
        ///     so_clear(&tmp);
        ///     so_uc_fmt_point(&tmp, &ucp);
        ///     printff("%u [%x] : %u [%.*s]",i,i,now, SO_F(tmp));getchar();
        /// }
        if(now != prev || i == UNICODE_MAX) {
            iE = i;
            so_fmt(&out, "%u..%u [ u+%x .. u+%x ] : %u\n", i0, iE, i0, iE, prev);
            i0 = iE;
        }
        prev = now;
    }
    so_println(out);
    return 0;
}

