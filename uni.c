#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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

C *e, *f, *l, *S[M];

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
    !l||--l->r||(d(l->e),d(l->n),l->n=f,f=l);
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
        switch(T[t]) {
        case I:
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
            t=b+t>42?
                (o=2*o|t&1,28):
                (putchar(b?o:t+8),fflush(stdout),b?12:28);
            break;
        case V:
            l=e;
            for(t=T[t+1]; t--; e=e->n);
            t=e->t;
            (e=e->e)&&e->r++;
            d(l);
            break;
        case A:
            t+=2;
            f||(f=calloc(1,sizeof(C)));
            assert(f&&s<M);
            S[s++]=l=f;
            f=l->n;
            l->r=1;
            l->t=t+T[t-1];
            (l->e=e)&&e->r++;
            break;
        case L:
            if(!s--)return 0;
            S[s]->n=e;
            e=S[s];
            t++;
            break;
        }
    }
    return T[t+2];
}
