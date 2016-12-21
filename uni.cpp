#include <cstdio>
#include <cassert>
#include <getopt.h>
#include <vector>
#include <memory>

#ifdef LOG
#define log(...) fprintf(stderr, ##__VA_ARGS__)
#define logp(M) M
#else
#define log(...)
#define logp(M)
#endif

using idx = std::vector<int>::size_type;

enum {
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

//closure
struct C {
    // lambda term (index in term space)
    idx t;
    // pointer to environment
    std::shared_ptr<C> e;
    // pointer to the next closure (as part of an environment), or next record on free list
    std::shared_ptr<C> n;
};

using Cp = std::shared_ptr<C>;

struct U {
// optimization flag
// false = default
// true  = call-by-need
int opt = 0;

// b=0 for bit mode and 7 for byte mode
// default is byte mode
idx b = 7;

//bits left in current byte
idx left;

//current input char
int c;

// Lambda term space
std::vector<idx> T = {
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
    
// current term to be processed
//  10 for byte mode
//  26 for bit mode
idx t = 10;

//current environment
// set initial environment as {0,0,0}
Cp e = std::make_shared<C>();

//s points to closure on the top of the stack
Cp s;

//copy T[l..u] to end of T
void x(idx l,idx u) {
    log("X %lu %lu\n", l, u);
    for(; l<=u; T.push_back(T[l++]));
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
void push(Cp* top, Cp l) {
    l->n = *top;
    *top = l;
}

//pop the top element
Cp pop(Cp* top) {
    Cp l = *top;
    *top = (*top)->n;
    return l;
}

//parses blc-encoded lambda term using g(), stores results in term space and returns length
idx p(const idx m) {
    if (g()) {
        T.push_back(V);
        T.push_back(0);
        while (g()) {
            T[T.size()-1]++;
        }
    } else {
        //01 -> application
        if (g()) {
            T.push_back(A);
            T.push_back(0);
            idx j = T.size() - 1;
            T[j] = p(m+2);
        //00 -> abstraction
        } else {
            T.push_back(L);
        }
        // decode the rest of the input
        p(T.size());
    }
    return T.size()-m;
}

void showL(Cp h, const char *name) {
    log("%s ", name);
    while (h) {
        log("(t:%lu, r:%ld, e:%p, a:%p) ", h->t, h.use_count(), h->e.get(), h.get());
        h = h->n;
    }
    log("\n");
}

idx showI(idx j) {
    log("T[%lu]: ", j);
    switch (T[j]) {
        case OB: log("OB\n"); break;
        case O0: log("O0\n"); break;
        case O1: log("O1\n"); break;
        case V: log("V %lu\n", T[j+1]); j++; break;
        case A: log("A %lu\n", T[j+1]); j++; break;
        case L: log("L\n"); break;
    }
    j++;
    return j;
}

void showP() {
    for (idx j = 44; j < T.size();) {
        j = showI(j);
    }
}

Cp newC(idx at, Cp ae) {
    Cp l=std::make_shared<C>();
    l->t=at;
    l->e=ae;
    return l;
}

void read() {
    g();
    left++;
    //not EOF and in byte mode, setup one byte
    if (~c&&b) {
        x(0,6);
        for(T[T.size()-5]=96; left; T.push_back(!g()))
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
    T.push_back(!b&&!g());
}

int run() {
    // output char
    char o = '\0';
    while(1) {
        if (t >= T.size()) {
            read();
        }
        //logp(showL(freel, "F"));
        logp(showL(s, "S"));
        logp(showL(e, "E"));
        logp(showI(t));
        switch(T[t]) {
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
            //resolve v to an closure clo and continue execution
            //with t = clo->t and e = clo->e
            const idx index = T[t+1];
            Cp old = e;
            Cp clo = e;
            for(idx j=index; j--; clo=clo->n);
            t=clo->t;
            e=clo->e;

            //push marker on the stack to update clo
            if (opt && t < T.size() && (T[t] == V || T[t] == A)) {
                push(&s, newC(0, clo));
            }

            break;
        }
        case A: {
            //create a closure and push it onto S
            // t = index of term that is the argument
            // e = current environment
            const idx size = T[t+1];
            t+=2;
            push(&s, newC(t+size, e));
            break;
        }
        case L: {
            //pop marker from the stack and update clo
            while (opt && s && s->t == 0) {
                Cp clo = pop(&s)->e;
                clo->t = t;
                clo->e = e;
            }

            //pop closure from stack and make it top level environment
            if (!s) {
                return 0;
            }
            push(&e, pop(&s));
            t++;
            break;
        }}
    }
}

void load() {
    // read program into end of T and store its size in T[43];
    T.push_back(0);
    T[43] = p(T.size());
    // clear bits left
    left=0;
}
};

int main(int argc, char **argv) {
    U u = U();

    // process options
    int ch;
    while ((ch = getopt(argc, argv, "bBpo")) != -1) {
        switch (ch) {
          case 'B': u.b = 7; u.t = 10; break;
          case 'b': u.b = 0; u.t = 26; break;
          case 'p': u.t = 44; break;
          case 'o': u.opt = 1; break;
        }
    }

    u.load();

    // show loaded program
    logp(u.showP());

    return u.run();
}
