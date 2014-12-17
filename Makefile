uni: uni.c
	gcc -DM=1000000 $^ -o $@

%.lam: proc.awk %.lamp
	awk -f $^ > $@

%.blc: parse.Blc %.lam
	cat $^ | ./uni > $@

%.Blc: deflate.Blc %.blc
	cat $^ | ./uni > $@

%.lam: symbolic.Blc %.blc
	cat $^ | ./uni | head -1 > $@
