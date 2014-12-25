The program implements a so called Krivine machine, enhanced with a basic
input/output system and a reference counting garbage collector.

The Krivine machine is a stepwise transformer of a global state consisting of
the current term, the current environment (a list of closures), and a stack of
continuations (See the 3rd reference for details.)
-- http://www.ioccc.org/2012/tromp/how13

The present version adds options parsing and supports the following options

* -b: binary mode
* -o: interpret with a call-by-need lazy Krivine machine, based on L machine defined in http://www.cs.indiana.edu/ftp/techreports/TR581.pdf
