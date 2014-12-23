#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <getopt.h>

#ifdef LOG
#define log(M, ...) fprintf(stderr, M, ##__VA_ARGS__)
#define logp(M) M
#else
#define log(M, ...)
#define logp(M)
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
int left;

//current input char
int c;

// Lambda term space
int T[M]= {
//encoding of input stream
// S = L A8,A2V0 ..
// 0 = L A8 A2V0LLV0 ..
// 1 = L A8 A2V0LLV1 ..
// E = LL V0
// bit input : 10
// encoded   : 10E
// byte input: \1\0
// encoded   : S00000001E S00000000E E
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
int n = 44;

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
    log("\nX %d %d", l, u);
    for(; l<=u; T[n++]=T[l++]);
}

//gets one bit of input, setting i to -1 on EOF or to remaining number of bits in current byte
int g() {
    if (left == 0) {
        left=b;
        c=getchar();
    } else {
        left--;
    }
    return (c>>left)&1;
}

void w(char o) {
    putchar(o);
    log("\nP %c", o);
    fflush(stdout);
}

//decrease reference counter, add record to free list on reaching zero
void d(C *l) {
    if (l) {
        l->r--;
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
            int j = n++;
            T[j] = p(m+2);
        //00 -> abstraction
        } else {
            T[n++] = L;
        }
        // decode the rest of the input
        p(n);
    }
    return n-m;
}

void showL(C *h, char *s) {
    log("%s ", s);
    while (h) {
        log("(t:%d, r:%d, e:%p, a:%p) ", h->t, h->r, h->e, h);
        h = h->n;
    }
    log("\n");
}

void showT() {
    for (int j = 44; j < n; j++) {
        log("T[%d]: ", j);
        switch (T[j]) {
            case V: log("V %d\n", T[j+1]); j++; break;
            case A: log("A %d\n", T[j+1]); j++; break;
            case L: log("L\n"); break;
        }
    }
}

C* newC(int ar, int at, C* ae) {
    if (!freel) {
        freel=calloc(1,sizeof(C));
    }
    assert(freel);
    C *l=freel;
    freel=l->n;
    l->r=ar;
    l->t=at;
    l->e=ae;
    if (ae) {
        ae->r++;
    }
    return l;
}

void push(C** top, C* l) {
    l->n = *top;
    *top = l;
}

C* pop(C** top) {
    C *l = *top;
    *top = (*top)->n;
    return l;
}

int main(int argc, char **argv) {
    // default is byte mode
    //  7 for byte mode
    //  0 for bit mode
    b = 7;
    
    // current term to be processed
    //  10 for byte mode
    //  26 for bit mode
    int t = 10;
    
    // output char
    char o;
   
    // process options
    char ch;
    while ((ch = getopt(argc, argv, "bBp")) != -1) {
        switch (ch) {
          case 'B': b = 7; t = 10; break;
          case 'b': b = 0; t = 26; break;
          case 'p': t = 44; break;
        }
    }

    // read program into end of T and store its size in T[43];
    T[43]=p(n);

    // show loaded program
    logp(showT());

    // clear bits left
    left=0;

    while(1) {
        //assert(showL(freel, "F"));
        logp(showL(s, "S"));
        logp(showL(e, "E"));
        log("t:%d, T[t]:", t);
        switch(T[t]) {
        case I:
            log("I");
            g();
            left++;
            assert(n<M-99);
            //not EOF and in byte mode, setup one byte
            if (~c&&b) {
                x(0,6);
                for(T[n-5]=96; left; T[n++]=!g())
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
            log("OB");
            w(o);
            t = 12;
            break;
        case O0:
            log("O0");
            if (b) {
                o = 2 * o + 0;
            } else {
                w('0');
            }
            t = 28;
            break;
        case O1:
            log("O1");
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
            log("V");
            log(" %d", T[t+1]);
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
            log("A");
            log(" %d", T[t+1]);
            t+=2;
            push(&s, newC(1, t+T[t-1], e));
            break;
        }
        case L: {
            //pop closure from stack and make it top level environment
            log("L");
            if (!s) {
                return 0;
            }
            push(&e, pop(&s));
            t++;
            break;
        }}
        log("\n");
    }
    return T[t+2];
}
