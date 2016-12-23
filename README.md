The objective of this project is to understand [John Tromp's 2012 IOCCC entry](http://www.ioccc.org/2012/tromp/hint.html).

# Done

* Refactor and understand the interpreter from https://tromp.github.io/cl/uni.c
* Replace manual reference counting with shared_ptr
* Add call-by-need optimization (<cite>[Winebarger2003][1]'s L machine</cite>)
* Add "collapse marker" optimization (<cite>[Winebarger2003][1]'s C machine</cite>)
* Add "eager dereferncing" optimization (<cite>[Winebarger2003][1]'s S machine</cite>)
* Refactor and understand the following programs:
    * [reverse](reverse.lam)
    * [inflate](inflate.lam)

[1]: http://www.cs.indiana.edu/pub/techreports/TR581.pdf

# Todo
* Refactor and understand the following programs:
    * [uni](uni232.lam)
    * [primes](primes.lam)
    * [uni8](uni8.lam)
    * [bf](bf.lam)
    * [sort](sort.lam)
    * [oddindices](oddindices.lam)
    * [deflate](deflate.lam)
    * [parse](parse.lam)

# Usage
uni is an interpreter for binary lambda calculus based on the Krivine machine.
It reads from standard input a program description followed by input for the
program, and output the result to the standard output.

It supports the following options:

* -B: byte mode (default)
* -b: binary mode
* -o: enable all optmizations
* -l: enable call-by-need optimization
* -c: enable "collapse marker" optimization
* -s: enable "eager dereferncing" optimization
