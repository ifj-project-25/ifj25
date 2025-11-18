import "ifj25" for Ifj
class Program {

static build(n) {
var s
s = ""
var i
i = 0
while (i < n) {
    s = s + "a"
    i = i + 1
}
return s
}

static build(f, a) {
    return a
}

static main() {
var txt
txt = build(6)
Ifj.write(txt)
}
}
