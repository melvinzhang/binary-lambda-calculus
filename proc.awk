BEGIN {
    FS = " = "
}

function proc(str, v) {
    split(str, tok, "")
    len = length(tok)
    res = ""
    for (i = 1; i <= len; i++) {
        c = tok[i]
        if (c in v) {
            res = res v[c]
        } else {
            res = res tok[i]
        }
    }
    return res;
}

/=/ {
    str = proc($2, v)
    v[$1] = str
    next
}

{
    printf("%s\n", proc($0, v));
}
