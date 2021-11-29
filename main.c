#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

/* #define int long long /1* 64 bit target *1/ */

int token;           /* current token */
char *src, *old_src; /* ptr to source code string */
int poolsize;        /* default size of text/data/stack */
int line;            /* line num */

int *text,     /* text segment */
    *old_text, /* dump text segment */
    *stack;    /* stack */
char *data;    /* data segment */

int *pc, *bp, *sp, *ax, cycle; /* VM registers */

/* Instructions */
enum {
    LEA, IMM, JMP, CALL, JZ, JNZ, ENT, ADJ, LEV, LI, LC, SI, SC,
    PUSH, OR, XOR, AND, EQ, NE, LT, GT, LE, GE, SHL, SHR, ADD, SUB,
    MUL, DIV, MOD, OPEN, READ, CLOS, PRTF, MALC, MSET, MCMP, EXIT
};

void next () {
    token = *src++;
    return;
}

void expression (int lvl) {
    /* TODO */
}

void program () {
    next();
    while (token > 0) {
        printf("token is: %c\n", token);
        next();
    }
}

int eval () {
    int op, *tmp;
    while (1) {
        op = *pc++; /* next op code */
        switch (op) {
            case IMM:  { ax = *pc++; } break;                    /* ld immediate val to ax */    
            case LC:   { ax = *(char *)ax; } break;              /* ld char to ax, addr in ax */ 
            case LI:   { ax = *(int *)ax; } break;               /* ld int to ax, addr in ax */  
            case SC:   { ax = *(char *)*sp++ = ax; } break;      /* save char to addr, val in ax, addr on stack */
            case SI:   { *(int *)*sp++ = ax; } break;            /* save int to addr, val in ax, addr on stack */
            case PUSH: { *--sp = ax; } break;                    /* push val of ax on stack */
            case JMP:  { pc = (int *)*pc; } break;               /* jmp to addr */
            case JZ:   { pc = ax ? pc + 1 : (int *)*pc; } break; /* jmp if ax is 0 */
            case JNZ:  { pc = ax ? (int *)*pc : pc + 1; } break; /* jmp if ax not 0 */
            case CALL: { *--sp = (int)(pc+1); pc = (int *)*pc; } break; /* call subroutine */
            case RET:  { pc = (int *)*sp++; } break;             /* ret from subroutine */
        }
    }
    return 0;
}

int main (int argc, char **argv) {
    int i, fd;

    argc--;
    argv++;
    poolsize = 256 * 1024;
    line = 1;

    if ((fd = open(*argv, 0)) < 0) {
        printf("could not open (%s)\n", *argv);
        return -1;
    }

    if (!(src = old_src = malloc(poolsize))) {
        printf("could not malloc (%d) for src area\n", poolsize);
        return -1;
    }

    if ((i = read(fd, src, poolsize-1)) <= 0) {
        printf("read() returned %d\n", i);
        return -1;
    }

    src[i] = 0;
    close(fd);

    /* Allocate memory for VM */
    if (!(text = old_text = malloc(poolsize))) {
        printf("could not malloc(%d) for text area\n", poolsize);
        return -1;
    }
    if (!(data = malloc(poolsize))) {
        printf("could not malloc(%d) for data area\n", poolsize);
        return -1;
    }
    if (!(stack = malloc(poolsize))) {
        printf("could not malloc(%d) for stack area\n", poolsize);
        return -1;
    }

    memset(text, 0, poolsize);
    memset(data, 0, poolsize);
    memset(stack, 0, poolsize);

    bp = sp = (int *)((int)stack + poolsize);
    ax = 0;

    program();
    return eval();
}
