# rename parameters to single lowercase letter

BEGIN {
    reserved[" "]
    reserved["\\"]
    reserved["("]
    reserved[")"]
    l = 0
    c[l] = 97
}

{
    split($0, tok, "")
    len = length(tok)
    for (i = 1; i <= len;) {
        if (tok[i] == "\\") {
            i++
            v = ""
            while (!(tok[i] in reserved) && i <= len) {
                v = v tok[i]
                i++
            }
            if (!(v in map)) {
                map[v] = c[l]
                new[l] = new[l] " " v
                if (c[l] > 122) {
                    print "|\\" v "|\n"
                    print "ERROR: out of symbols" > "/dev/stderr"
                    exit 1
                }
                c[l]++
            }
            printf("\\%c", map[v])
        } else if (!(tok[i] in reserved)) {
            v = ""
            while (!(tok[i] in reserved) && i <= len) {
                v = v tok[i]
                i++
            }
            if (!(v in map)) {
                print "|" v "|\n"
                print "ERROR: " v " undefined" > "/dev/stderr"
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
                split(new[l], source, " ");
                slen = length(source);
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
