#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr, M, ##__VA_ARGS__)
#endif

enum {
    // input
    I,
    // output
    OB,
    O0,
    O1,
    // denotes a variable, with next entry its de Bruijn index
    V,
    // denotes application with next entry the size of the term being applied
    A,
    // denotes lambda abstraction
    L
};

// b=0 for bit mode and 7 for byte mode
int b;

//bits left in current byte
int i;

//current input char
int c;

// Lambda term space
int T[M]= {
                 L,A,8,A,2,  V,0,L,L,V,
//10 - start here for byte mode
//V    12 - jump here after output a byte
//     V
  A,30,L,A,2,V,0,L,A,5,A,7,L,V,0,OB,
//26 - start here for bit mode
//V    28 - jump here after output a bit
//     V
  A,14,L,A,2,V,0,L,A,5,A,2,  V,0,O0,O1,A
};
// end of T
int n=44;

//closure
typedef struct _ {
    // lambda term (index in term space)
    int t;
    // reference count
    int r;
    // pointer to environment
    struct _ *e;
    // pointer to the next closure (as part of an environment), or next record on free list
    struct _ *n;
} C;

//current environment
C *e;

//free list
C *freel;

//s points to closure on the top of the stack
C* s;

//copy T[l..u] to end of T
void x(int l,int u) {
    debug("\nX %d %d", l, u);
    for(; l<=u; T[n++]=T[l++]);
}

//gets one bit of input, setting i to -1 on EOF or to remaining number of bits in current byte
int g() {
    if (i == 0) {
        i=b;
        c=getchar();
    } else {
        i--;
    }
    return c>>i&1;
}

void w(char o) {
    putchar(o);
    debug("\nP %c", o);
    fflush(stdout);
}

//decrease reference counter, add record to free list on reaching zero
void d(C *l) {
    if (l) {
        --l->r;
        if (l->r == 0) {
            d(l->e);
            d(l->n);
            l->n=freel;
            freel=l;
        }
    }
}

//parses blc-encoded lambda term using g(), stores results in term space and returns length
int p(const int m) {
    if (g()) {
        for(T[n++]=V; g(); T[n]++);
        n++;
    } else {
        //01 -> application
        if (g()) {
            T[n++] = A;
            T[n++] = p(m+2);
        //00 -> abstraction
        } else {
            T[n++] = L;
        }
        // decode the rest of the input
        p(n);
    }
    return n-m;
}

int main(int argc, char **argv) {
    int t = argc;
    char o;
    b=t>1?0:7;
    T[43]=p(n);
    for (int j = 43; j < n; j++) {
        debug("T[%d]: %d\n", j, T[j]);
    }
    i=0;
    // index into T
    t=b?10:26;
    while(1) { 
        debug("t:%d, s:%p, e:%p, T[t]:", t, s, e);
        switch(T[t]) {
        case I:
            debug("I");
            g();
            i++;
            assert(n<M-99);
            //not EOF and in byte mode, setup one byte
            if (~c&&b) {
                x(0,6);
                for(T[n-5]=96; i; T[n++]=!g())
                    x(0,9);
            }
            //EOF reached
            if (c < 0) {
                x(7, 9);
            //in bit mode, setup one bit
            //in byte mode, terminate byte list
            } else {
                x(b, 9);
            }
            T[n++]=!b&&!g();
            break;
        case OB:
            debug("OB");
            w(o);
            t = 12;
            break;
        case O0:
            debug("O0");
            if (b) {
                o = 2 * o + 0;
            } else {
                w('0');
            }
            t = 28;
            break;
        case O1:
            debug("O1");
            if (b) {
                o = 2 * o + 1;
            } else {
                w('1');
            }
            t = 28;
            break;
        case V: {
            //resolve v to an environment e and continue execution
            //with t = e->t and e = e->e
            debug("V");
            debug(" %d", T[t+1]);
            C *l=e;
            for(t=T[t+1]; t--; e=e->n);
            t=e->t;
            e=e->e;
            if (e) {
                e->r++;
            }
            d(l);
            break;
        }
        case A: {
            //create a closure and push it onto S
            // t = index of term that is the argument
            // e = current environment
            debug("A");
            debug(" %d", T[t+1]);
            t+=2;
            if (!freel) {
                freel=calloc(1,sizeof(C));
            }
            assert(freel);
            C *l=freel;
            freel=l->n;
            l->r=1;
            l->t=t+T[t-1];
            l->e=e;
            if (e) {
                e->r++;
            }
            l->n = s;
            s = l;
            break;
        }
        case L: {
            //pop closure from stack and make it top level environment
            debug("L");
            if (!s) {
                return 0;
            }
            C *l = s->n;
            s->n = e;
            e = s;
            s = l;
            t++;
            break;
        }}
        debug("\n");
    }
    return T[t+2];
}
