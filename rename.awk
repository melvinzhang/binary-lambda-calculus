# rename parameters to single lowercase letter

BEGIN {
    reserved[" "]
    reserved["\\"]
    reserved["("]
    reserved[")"]
    l = 0
    c[l] = 97
    new[l] = ""
}

{
    len = split($0, tok, "")
    for (i = 1; i <= len;) {
        if (tok[i] == "\\") {
            s = i
            i++
            v = ""
            while (!(tok[i] in reserved) && i <= len) {
                v = v tok[i]
                i++
            }
            if (!(v in map)) {
                map[v] = c[l]
                new[l] = new[l] " " v
                if (c[l] > 127) {
                    printf("\nrename:%d: %s|\\%s|\n", NR, substr($0, 0, s-1), v) > "/dev/stderr"
                    printf("ERROR: out of symbols\n") > "/dev/stderr"
                    exit 1
                }
                c[l]++
            }
            printf("\\%c", map[v])
        } else if (!(tok[i] in reserved)) {
            s = i
            v = ""
            while (!(tok[i] in reserved) && i <= len) {
                v = v tok[i]
                i++
            }
            if (!(v in map)) {
                printf("\nrename:%d: %s|%s|\n", NR, substr($0, 0, s-1), v) > "/dev/stderr"
                printf("ERROR: %s undefined\n", v) > "/dev/stderr"
                exit 1
            }
            printf("%c", map[v])
        } else {
            if (tok[i] == "(") {
                l++
                c[l] = c[l-1]
                new[l] = ""
            } else if (tok[i] == ")") {
                # clear mappings made in level l
                slen = split(new[l], source, " ");
                for (j = 1; j <= slen; j++) {
                    delete map[source[j]]
                }
                l--
            }
            printf("%c", tok[i])
            i++
        }
    }
    printf("\n")
}
