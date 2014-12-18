BEGIN {
    FS = " = "
}

/=/ {
    v[$1] = $2
    #print $1 "-" $2
    next 
}

{
    split($0, line, "")
    len = length(line)
    for (i = 1; i <= len; i++) {
        c = line [i]
        if (c in v) {
            printf("%s", v[c])
        } else {
            printf("%c", c)
        }
    }
}
