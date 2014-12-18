uni: uni.c
	gcc -DM=1000000 $^ -o $@

%.lc: proc.awk %.lam
	awk -f $^ > $@

%.blc: parse.Blc %.lc
	cat $^ | ./uni > $@

%.Blc: deflate.Blc %.blc
	cat $^ | ./uni > $@

%.lc: symbolic.Blc %.blc
	cat $^ | ./uni | head -1 > $@

%.b: %.blc
	cat $^ | ./uni -b | head -c 10

%.B: %.Blc
	cat $^ text | ./uni
	@echo
