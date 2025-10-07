#include <rlso.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define write_cstr(x)   \
    write(STDOUT_FILENO, x, sizeof(x)-1)

#define write_so(so)   \
    write(STDOUT_FILENO, so_it0(so), so_len(so))


struct termios termios_entry;

void cleanup(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &termios_entry);
    write_cstr("\e[?25h"); // show cursor
}

void makeraw(void) {
    if((tcgetattr(STDIN_FILENO, &termios_entry) == -1)) exit(1);
    struct termios raw;
    cfmakeraw(&raw);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) exit(1);
}

int cursor_pos_get(int *x, int *y) {
    char buf[16];
    unsigned int i = 0;
    if(write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;
    while(i < sizeof(buf) - 1) {
        if(read(STDIN_FILENO, &buf[i], 1) != 1) break;
        if(buf[i] == 'R') break;
        i++;
    }
    buf[i] = 0;
    if(buf[0] != '\x1b' || buf[1] != '[') return -1;
    char *endptr;
    *y = strtoul(&buf[2], &endptr, 10);
    int offs = endptr - buf;
    if(buf[offs++] != ';') return -1;
    *x = strtoul(&buf[offs], &endptr, 10);
    offs = endptr - buf;
    // could check if i == offs
    return 0;
}

#define UNICODE_MAX     0x10FFFFULL

int main(void) {
    makeraw();
    So_Uc_Point ucp;
    So tmp = SO;
    So out = SO;
    unsigned char *widths = malloc(UNICODE_MAX + 1);
    memset(widths, 0, UNICODE_MAX + 1);
    write_cstr("\e[?25l"); // hide cursor
    write_cstr("\e[2J"); // clear all

    for(size_t i = 0x80; i <= UNICODE_MAX; ++i) {
        ucp.val = i;
        so_clear(&tmp);
        //so_extend(&tmp, so("\e[K")); // clear line
        so_extend(&tmp, so("\e[H")); // goto 1,1
        so_uc_fmt_point(&tmp, &ucp);
        //write_cstr("\e[2K"); // clear line
        int x0 = 1, xE, yE;
        ///cursor_pos_get(&x0, &y0);
        write_so(tmp);
        cursor_pos_get(&xE, &yE);
        widths[i] = xE - x0;
        //printff("\r\n%zu : %u",i,xE - x0);
        //usleep(5e5);

        if(!((i + 1) % 250)) {
            write_cstr("\r\n\e[2K"); // clear line
            so_clear(&out);
            so_fmt(&out, "%zu/%zu (%4g%%)", i + 1, UNICODE_MAX, 100.0f*(float)(i+1)/(float)UNICODE_MAX);
            write_so(out);
        }
    }

    so_clear(&out);
    for(size_t i = 0; i < 30; ++i) {
        so_fmt(&out, "%u,\n", 0);
    }
    for(size_t i = 30; i < 0x80; ++i) {
        so_fmt(&out, "%u,\n", 1);
    }
    for(size_t i = 0x80; i <= UNICODE_MAX; ++i) {
        so_fmt(&out, "%u%s\n", widths[i], i < UNICODE_MAX ? "," : "");
    }
    so_file_write(so("rlwcwidth.lut.data.h"), out);
}

