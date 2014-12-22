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
    O,
    // denotes a variable, with next entry its de Bruijn index
    V,
    // denotes application with next entry the size of the term being applied
    A,
    // denotes lambda abstraction
    L
};

//bits left in current byte
int i;

//current input char
int c;

// b=0 for bits mode and 7 for bytes mode
int b;

// Lambda term space
int T[M]= {
               L,A,8,A,2,  V,0,L,L,V,
//10 - start here for byte mode
A,30,L,A,2,V,0,L,A,5,A,7,L,V,0,O,
//26 - start here for binary mode
A,14,L,A,2,V,0,L,A,5,A,2,  V,0,O,O,A
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

// stack of closures
C *S[M];

// s is index to top of S
int s;

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
int p(int m) {
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
        debug("t:%d, s:%d, i:%d, T[t]:", t, s, i);
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
            //in binary mode, setup one bit
            //in byte mode, terminate byte list
            } else {
                x(b, 9);
            }
            T[n++]=!b&&!g();
            break;
        case O:
            debug("O");
            t=b+t>42?
                (o=2*o|(t&1),28):
                (putchar(b?o:t+8),fflush(stdout),b?12:28);
            break;
        case V: {
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
            debug("A");
            debug(" %d", T[t+1]);
            t+=2;
            if (!freel) {
                freel=calloc(1,sizeof(C));
            }
            assert(freel&&s<M);
            C *l=freel;
            freel=l->n;
            l->r=1;
            l->t=t+T[t-1];
            l->e=e;
            if (e) {
                e->r++;
            }
            S[s++]=l;
            break;
        }
        case L:
            debug("L");
            if(!s--)return 0;
            S[s]->n=e;
            e=S[s];
            t++;
            break;
        }
        debug("\n");
    }
    return T[t+2];
}
