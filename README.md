The objective of this project is to understand [John Tromp's 2012 IOCCC entry](http://www.ioccc.org/2012/tromp/hint.html).

# Done

* Refactor and understand the C interpreter from https://tromp.github.io/cl/uni.c
* Add call-by-need optimization to the interpreter
* Refactor and understand the following programs:
    * [reverse](reverse.lam)

# Todo
* Refactor and understand the following programs:
    * [uni](uni232.lam)
    * [primes](primes.lam)
    * [uni8](uni8.lam)
    * [bf](bf.lam)
    * [hilbert](hilbert.lam)
    * [sort](sort.lam)
    * [oddindices](oddindices.lam)
    * [inflate](inflate.lam)
    * [deflate](deflate.lam)
    * [parse](parse.lam)

# Usage
uni is an interpreter for binary lambda calculus based on the Krivine machine.
It reads from standard input a program description followed by input for the
program, and output the result to the standard output.

It supports the following options:

* -B: byte mode (default)
* -b: binary mode
* -o: enable call-by-need optimization
