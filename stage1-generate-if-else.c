#include "rlwcwidth.lut.h"
#include <rlso.h>
#include <rlc.h>

#define UNICODE_MAX     0x10FFFFULL

#define MAX_DEPTH   128

typedef struct Range {
    size_t from;
    size_t until;
    char width;
} Range, *Ranges;

typedef enum {
    SIDE_LEFT = 0,
    SIDE_RIGHT,
} Side;

typedef struct State {
    Side side;
    int depth;
    size_t i0;
    size_t iE;
    size_t n_cmp[MAX_DEPTH];
} State;

typedef struct Node {
    struct Node *l;
    struct Node *r;
    struct Node *p;
    int pad;
    Range *range;
    size_t i;
} Node;

typedef struct IfElse {
    struct IfElse *branch_if;
    struct IfElse *branch_else;
    size_t i;
    bool branch_into_else;
} IfElse;

void split_range(Ranges ranges, State *state, Node *parent, Node **node) {
    size_t i0 = state->i0;
    size_t iE = state->iE;
    size_t i_center = (iE - i0) / 2 + i0;
    bool l_at_end = (i0 + 1 >= iE);
    bool r_at_end = (i0 >= iE);
    bool valid = i_center < array_len(ranges);
    size_t depth = state->depth;
    size_t indent = state->depth;
    bool parentless = !parent;
    //if(state->side == SIDE_RIGHT) --indent;

        // GET CMP COUNT
        size_t n_cmp_old = state->n_cmp[depth];
        
        /// // PRINT IF-ELSE
        /// switch(state->side) {
        ///     case SIDE_LEFT: ++state->n_cmp[depth]; break;
        ///     case SIDE_RIGHT: --state->n_cmp[depth]; break;
        /// }

        size_t n_cmp_new = state->n_cmp[depth];

    if(i0 >= iE) valid = false;

    Node *pnode_parentless = 0;
    Node *pnode = 0;
    if(valid) {


        *node = malloc(sizeof(Node));
        memset(*node, 0, sizeof(Node));
        pnode = *node;

        if(parentless) {
            pnode_parentless = pnode;
            pnode->l = malloc(sizeof(Node));
            memset(pnode->l, 0, sizeof(Node));
            pnode->p = pnode;
            pnode = pnode->l;
        } else {
            pnode->p = parent;
        }

        //printf("%u:%2zi:%2zi ", indent, n_cmp_old, n_cmp_new);
        printf("%c", state->side == SIDE_LEFT ? 'L' : 'R');
        printf("%u  ", (int)indent);
        //printf(" [%u:%zi] ", depth, state->n_cmp[depth]);

        Range range = ranges[i_center];
        pnode->pad = indent;
        pnode->range = &ranges[i_center];
        //printf("%*s* %zu .. if <= %zu\n", depth, "", range.from, range.until);

        // PADDING
        for(size_t i = 0; i < indent; ++i) printf("│ ");
        //printf("%*s", (int)indent, "");

        // if(!n_cmp_old) {
        //     printf("if");
        // } else if(!n_cmp_new) {
        //     printf("else if");
        // } else {
        //     //printf("else if");
        // }
        
        // THE CONDITION
        //printf("( x <= %zu )", range.until);
        printf("┝ ");

        if(state->side == SIDE_LEFT) {
            ++state->n_cmp[depth];;
            printf("if");
            printf(" %zu", range.until);
        }
        else {
            printf("else");
            --state->n_cmp[depth];;
        }


        printf("\n");

        // PROCESS REST
    } else {
        if(state->side == SIDE_RIGHT && state->n_cmp[depth]) {
            --state->n_cmp[depth];
            printf(" #  ");
            for(size_t i = 0; i < indent; ++i) printf("│ ");
            printf("┝ ");
            printf("else\n");

#if 0
            *node = malloc(sizeof(Node));
            memset(*node, 0, sizeof(Node));
            Node *pnode = *node;
            pnode->p = parent;
            pnode->range = &ranges[i_center];
#endif
        }
    }
    if(!valid) return;

    //int deeper = *depth + 1;
    size_t depth_pass = depth + 1;
    if(!l_at_end) {
        state->i0 = i0;
        state->iE = i_center;
        state->side = SIDE_LEFT;
        state->depth = depth_pass;
        split_range(ranges, state, pnode, &pnode->l);
    }
    if(!r_at_end) {
        state->i0 = i_center + 1;
        state->iE = iE;
        state->side = SIDE_RIGHT;
        state->depth = i0 ? depth_pass : depth;
        if(parentless) {
            split_range(ranges, state, pnode_parentless, &pnode_parentless->r);
        } else {
            split_range(ranges, state, pnode, &pnode->r);
        }
    }
    else {
    }

        //if(opened) printf("}");
        //printf("\n");
}

void recalculate_into_ifelse(Ranges ranges, Node *node, int pad, IfElse **ifelse, size_t i, bool in_else) {

    *ifelse = malloc(sizeof(IfElse));
    memset(*ifelse, 0, sizeof(IfElse));
    IfElse *current = *ifelse;
    current->i = i;
    current->branch_into_else = in_else;
    if(!node) return;

    recalculate_into_ifelse(ranges, node->l, pad + 1, &current->branch_if, node->i, false);
    recalculate_into_ifelse(ranges, node->r, pad + 1, &current->branch_else, node->i, true);
}

void generate_ifelse(So *out, IfElse *ifelse, int depth, Ranges ranges) {
    if(!ifelse) return;
    bool closed = false;
    if(depth) {
        Range range;
        if(ifelse->branch_into_else) {
            range = ranges[ifelse->i + 1];
            so_fmt(out, "%*selse {", depth, "");
        } else {
            range = ranges[ifelse->i];
            so_fmt(out, "%*sif ( x < %zu ) {", depth, "", range.until);
        }
        if(!ifelse->branch_else || !ifelse->branch_if) {
            so_fmt(out, " r = %u;", range.width);
            closed = true;
        }
        so_fmt(out, "\n");
    }
    generate_ifelse(out, ifelse->branch_if, depth + 1, ranges);
    generate_ifelse(out, ifelse->branch_else, depth + 1, ranges);
    if(depth) {
        so_fmt(out, "%*s}\n", depth, "");
        if(!closed) {
        }
        if(ifelse->branch_into_else) {
        } else {
            //printf("%*s} ", depth, "");
        }
    }
}

static int depth2 = 0;
void generate_binary_tree(Ranges ranges, size_t i0, size_t iE, Node *parent, Node **next) {
    size_t len = array_len(ranges);
    if(i0 >= iE) return;
    if(!next) {
        /* first iteration */
        size_t i2 = (iE - i0) / 2 + i0;
        printff("%*s%u..%u => %u", depth2++,"", i0,iE,i2);
        parent->i = i2;
        parent->range = &ranges[i2];
        generate_binary_tree(ranges, i0, i2, parent, &parent->l);
        generate_binary_tree(ranges, i2 + 1, iE, parent, &parent->r);
        --depth2;
    } else {
        assert(parent);
        /* alloc */
        *next = malloc(sizeof(Node));
        Node *pnext = *next;
        memset(pnext, 0, sizeof(Node));
        pnext->p = parent;
        /* algo */
        size_t i2 = (iE - i0) / 2 + i0;
        printff("%*s%u..%u => %u", depth2++,"", i0,iE,i2);
        pnext->range = &ranges[i2];
        pnext->i = i2;
        generate_binary_tree(ranges, i0, i2, pnext, &pnext->l);
        generate_binary_tree(ranges, i2 + 1, iE, pnext, &pnext->r);
        --depth2;
    }
}

int main(void) {
    Ranges ranges = 0;

    So out = SO;
    size_t i0 = 0;
    size_t iE = 0;
    char prev = 0;
    So_Uc_Point ucp;

    for(unsigned int i = 0; i <= UNICODE_MAX; ++i) {
        char now = rlwcwidth_lookup(i);
        if(now != prev || i == UNICODE_MAX) {
            iE = i;
            Range add = {
                .from = i0,
                .until = iE,
                .width = prev,
            };
            array_push(ranges, add);
            i0 = iE;
        }
        prev = now;
    }

    printf("Found %lu ranges, start splitting!\n", array_len(ranges));
    int depth = 0;
    size_t until = array_len(ranges);
    //size_t until = 20;
    Node node = {0};
    generate_binary_tree(ranges, 0, until, &node, 0);

    IfElse *ifelse = malloc(sizeof(IfElse));
    memset(ifelse, 0, sizeof(IfElse));
    recalculate_into_ifelse(ranges, &node, 0, &ifelse, 0, false);

    so_clear(&out);
    so_fmt(&out, "char rlwcwidth(int x) {\n");
    so_fmt(&out, " char r = -1;\n");
    generate_ifelse(&out, ifelse, 0, ranges);
    so_fmt(&out, " return r;\n");
    so_fmt(&out, "}\n");
    so_file_write(so("rlwcwidth.c"), out);
    so_println(out);

    so_clear(&out);
    so_fmt(&out, "#ifndef RLWCWIDTH_AUTO_H\n");
    so_fmt(&out, "char rlwcwidth(int x);\n");
    so_fmt(&out, "#define RLWCWIDTH_AUTO_H\n");
    so_fmt(&out, "#endif\n");
    so_file_write(so("rlwcwidth.h"), out);
    so_println(out);

    printf("done.\n");
    return 0;
}


