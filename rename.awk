# rename parameters to single lowercase letter

BEGIN {
    reserved[" "]
    reserved["\\"]
    reserved["("]
    reserved[")"]
    c = 97;
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
                map[v] = c
                if (c > 122) {
                    print "|\\" v "|\nERROR: out of symbols" > "/dev/stderr"
                    exit 1
                }
                c++
            }
            printf("\\%c", map[v])
        } else if (!(tok[i] in reserved)) {
            v = ""
            while (!(tok[i] in reserved) && i <= len) {
                v = v tok[i]
                i++
            }
            if (!(v in map)) {
                print "|" v "|\nERROR: " v " undefined" > "/dev/stderr"
                exit 1
            }
            printf("%c", map[v])
        } else {
            printf("%c", tok[i])
            i++
        }
    }
    printf("\n")
}
