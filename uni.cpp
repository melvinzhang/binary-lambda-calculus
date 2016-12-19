#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <getopt.h>
#include <vector>

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

// optimization flag
// false = default
// true  = call-by-need
int opt = 0;

// b=0 for bit mode and 7 for byte mode
int b;

//bits left in current byte
int left;

//current input char
int c;

// Lambda term space
std::vector<int> T = {
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

// byte mode term
// (\output output (\B B (\b b O0 O1) OB)) (P input)

// bit mode term
// (\output output (\b b O0 O1)) (P input)

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

void push_back(int v) {
    T[n++] = v;
}

//copy T[l..u] to end of T
void x(int l,int u) {
    log("X %d %d\n", l, u);
    for(; l<=u; push_back(T[l++]));
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

//write one char to stdout
void w(char o) {
    putchar(o);
    log("P %c\n", o);
    fflush(stdout);
}

//push l onto top
void push(C** top, C* l) {
    l->n = *top;
    *top = l;
}

//pop the top element
C* pop(C** top) {
    C *l = *top;
    *top = (*top)->n;
    return l;
}

//decrease reference counter, add record to free list on reaching zero
void d(C *l) {
    l->r--;
    if (l->r == 0) {
        d(l->e);
        d(l->n);
        push(&freel, l);
    }
}

//parses blc-encoded lambda term using g(), stores results in term space and returns length
int p(const int m) {
    if (g()) {
        push_back(V);
        push_back(0);
        while (g()) {
            T[n-1]++;
        }
    } else {
        //01 -> application
        if (g()) {
            push_back(A);
            push_back(0);
            int j = n - 1;
            T[j] = p(m+2);
        //00 -> abstraction
        } else {
            push_back(L);
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

int showI(int j) {
    log("T[%d]: ", j);
    switch (T[j]) {
        case I: log("I\n"); break;
        case OB: log("OB\n"); break;
        case O0: log("O0\n"); break;
        case O1: log("O1\n"); break;
        case V: log("V %d\n", T[j+1]); j++; break;
        case A: log("A %d\n", T[j+1]); j++; break;
        case L: log("L\n"); break;
    }
    j++;
    return j;
}

void showP() {
    for (int j = 44; j < n;) {
        j = showI(j);
    }
}

C* newC(int ar, int at, C* ae) {
    if (!freel) {
        freel=(C*)calloc(1,sizeof(C));
    }
    assert(freel);
    C *l=pop(&freel);
    l->r=ar;
    l->t=at;
    l->e=ae;
    ae->r++;
    return l;
}

int main(int argc, char **argv) {
    T.resize(100000, 0);

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
    while ((ch = getopt(argc, argv, "bBpo")) != -1) {
        switch (ch) {
          case 'B': b = 7; t = 10; break;
          case 'b': b = 0; t = 26; break;
          case 'p': t = 44; break;
          case 'o': opt = 1; break;
        }
    }

    // read program into end of T and store its size in T[43];
    T[43]=p(n);

    // show loaded program
    logp(showP());

    // clear bits left
    left=0;

    // set initial environment as {0,0,0}
    e = (C*)calloc(1,sizeof(C));

    while(1) {
        //logp(showL(freel, "F"));
        logp(showL(s, "S"));
        logp(showL(e, "E"));
        logp(showI(t));
        switch(T[t]) {
        case I:
            g();
            left++;
            //not EOF and in byte mode, setup one byte
            if (~c&&b) {
                x(0,6);
                for(T[n-5]=96; left; push_back(!g()))
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
            push_back(!b&&!g());
            break;
        case OB:
            w(o);
            t = 12;
            break;
        case O0:
            if (b) {
                o = 2 * o + 0;
            } else {
                w('0');
            }
            t = 28;
            break;
        case O1:
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
            const int index = T[t+1];
            C *old = e;
            C *env = e;
            for(int j=index; j--; env=env->n);
            t=env->t;
            e=env->e;
            e->r++;
            d(old);
            break;
        }
        case A: {
            //create a closure and push it onto S
            // t = index of term that is the argument
            // e = current environment
            const int size = T[t+1];
            t+=2;
            push(&s, newC(1, t+size, e));
            break;
        }
        case L: {
            //pop closure from stack and make it top level environment
            if (!s) {
                return 0;
            }
            push(&e, pop(&s));
            t++;
            break;
        }}
    }
    return T[t+2];
}
