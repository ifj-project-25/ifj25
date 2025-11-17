import "ifj25" for Ifj
class Program {

static mix(a, b, c) {
var s
s = a + b
return s + c
}

static main() {
var r
r = mix(1,2,3)
Ifj.write(r)
}
}
