The objective of this project is to understand [John Tromp's 2012 IOCCC entry](http://www.ioccc.org/2012/tromp/hint.html).

# Progress

1. ~~Refactor and understand the C interpreter from https://tromp.github.io/cl/uni.c~~
2. ~~Add call-by-need optimization to the interpreter~~
3. Refactor and understand the following programs:
    * ~~reverse~~
    * ~~uni~~
    * primes
    * uni8
    * bf
    * hilbert
    * sort
    * symbolic
    * parse

# Usage
uni is an interpreter for binary lambda calculus based on the Krivine machine.
It reads from standard input a program description followed by input for the
program, and output the result to the standard output.

It supports the following options:

* -b: binary mode
* -o: enable call-by-need optimization
