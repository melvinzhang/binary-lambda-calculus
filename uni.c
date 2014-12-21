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

int n=44;
int i;
int c;

// Lambda term space
int T[M]= {
               L,A,8,A,2,  V,0,L,L,V,
A,30,L,A,2,V,0,L,A,5,A,7,L,V,0,O,
A,14,L,A,2,V,0,L,A,5,A,2,  V,0,O,O,A
};
int b;
int s;

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

C *l;

C *S[M];

//copy T[l..u] to end of T
void x(int l,int u) {
    for(; l<=u; T[n++]=T[l++]);
}

//gets one bit of input, setting i to -1 on EOF or to remaining number of bits in current byte
int g() {
    i--||(i=b,c=getchar());
    return c>>i&1;
}

//decrease reference counter, add record to free list on reaching zero
void d(C *l) {
    !l||--l->r||(d(l->e),d(l->n),l->n=freel,freel=l);
}

//parses blc-encoded lambda term using g(), stores results in term space and returns length
int p(int m) {
    if (g()) {
        for(T[n++]=V; g(); T[n]++);
        n++;
    } else {
        T[m]=n++&&g()?(T[m+1]=p(++n),A):L,p(n);
    }
    return n-m;
}

int main(int t) {
    char o;
    b=t>1?0:7;
    T[43]=p(n);
    i=0;
    for(t=b?10:26;;) { 
        debug("t:%d, s:%d, i:%d, T[t]:", t, s, i);
        switch(T[t]) {
        case I:
            debug("I");
            g();
            i++;
            assert(n<M-99);
            if(~c&&b) {
                x(0,6);
                for(T[n-5]=96; i; T[n++]=!g())
                    x(0,9);
            }
            x(c<0?7:b,9);
            T[n++]=!b&&!g();
            break;
        case O:
            debug("O");
            t=b+t>42?
                (o=2*o|t&1,28):
                (putchar(b?o:t+8),fflush(stdout),b?12:28);
            break;
        case V:
            debug("V");
            l=e;
            for(t=T[t+1]; t--; e=e->n);
            t=e->t;
            (e=e->e)&&e->r++;
            d(l);
            break;
        case A:
            debug("A");
            t+=2;
            freel||(freel=calloc(1,sizeof(C)));
            assert(freel&&s<M);
            S[s++]=l=freel;
            freel=l->n;
            l->r=1;
            l->t=t+T[t-1];
            (l->e=e)&&e->r++;
            break;
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
