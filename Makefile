uni: uni.c
	gcc -DM=1000000 $^ -o $@

%.lc: proc.awk %.lam
	awk -f $^ > $@

%.inf.blc: inflate.Blc %.Blc
	cat $^ | ./uni > $@

%.blc: parse.Blc %.lc
	cat $^ | ./uni > $@

%.Blc: deflate.Blc %.blc
	cat $^ | ./uni > $@

%.lc: symbolic.Blc %.blc
	cat $^ | ./uni | head -1 > $@

%.b: %.blc
	cat $^ binary | ./uni -b | head -c 10
	@echo

%.B: %.Blc
	cat $^ bytes | ./uni
	@echo

hilbert:
	(cat hilbert.Blc; echo -n 1024) | ./uni

primes.dec:
	(cat oddindices.Blc; echo -n " "; cat primes.blc | ./uni -b) | ./uni | head -c 70

primes.bin: primes.blc
	cat $^ | ./uni -b | head -c 70

hw_in_bf: bf.Blc hw.bf
	cat $^ | ./uni
