import "ifj25" for Ifj
class Program {

static pow(a, b) {
var r
r = 1
var i
i = 0
while (i < b) {
    r = r * a
    i = i + 1
}
return r
}

static main() {
var x
x = pow(2,5)
Ifj.write(x)
}
}
