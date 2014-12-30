uni: uni.c
	gcc -std=c99 -O3 -DM=1000000 $^ -o $@

uni.pg: uni.c
	gcc -std=c99 -DM=1000000 -pg $^ -o $@

unid: uni.c
	gcc -std=c99 -Wall -DM=1000000 -DLOG $^ -o $@

# lam -> lc -> blc -> Blc

%.sym.lc: ioccc/symbolic.Blc %.blc
	cat $^ | ./uni | head -1 > $@

%.lc: %.lam
	gcc -E -x c -P $^ | awk -f rename.awk > $@

%.inf.blc: ioccc/inflate.Blc %.Blc
	cat $+ | ./uni > $@

%.blc: ioccc/parse.Blc %.lc
	cat $^ | ./uni > $@

%.Blc: ioccc/deflate.Blc %.blc
	cat $^ | ./uni > $@

%.b: %.blc
	cat $^ binary | ./uni -b | head -c 10
	@echo

%.B: %.Blc
	cat $^ bytes | ./uni
	@echo

hilbert:
	(cat hilbert.Blc; echo -n 1024) | ./uni

primes.dec: primes.blc
	(cat oddindices.Blc; echo -n " "; cat $^ | ./uni -b) | ./uni | head -c 70

primes.bin: primes.blc
	cat $^ | ./uni -b | head -c 70

hw_in_bf: bf.Blc hw.bf
	cat $^ | ./uni

reduction: symbolic.Blc 2exp3.blc
	cat $^ | ./uni

trace.b: id.blc unid
	(cat $<; echo -n 10) | ./unid -b 2> $@

trace.B: id.Blc unid
	(cat $<; echo -n 10) | ./unid 2> $@

debug: test.blc unid
	(cat $<; echo -n 10) | ./unid -b 2> $@

test_opt: uni unid
	(cat oddindices.Blc; echo -n " "; cat primes.blc | ./uni -b) | ./uni | head -c 70
	@echo
	(cat oddindices.Blc; echo -n " "; cat primes.blc | ./uni -b -o) | ./uni -o | head -c 70
	@echo
	(cat hilbert.Blc; echo -n 12) | ./unid 2> debug.orig
	(cat hilbert.Blc; echo -n 12) | ./unid -o 2> debug.opt

bench:
	/usr/bin/time -v make bench_uni
	/usr/bin/time -v make bench_opt

bench_uni: uni1.blc primes.blc uni
	@cat uni1.blc uni1.blc uni1.blc uni1.blc primes.blc | ./uni -b | head -c 70
	@echo

bench_opt: uni1.blc primes.blc uni
	@cat uni1.blc uni1.blc uni1.blc uni1.blc primes.blc | ./uni -b -o | head -c 70
	@echo
