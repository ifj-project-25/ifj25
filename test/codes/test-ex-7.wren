import "ifj25" for Ifj
class Program {
static main() {
var a
a = 10
if (a is Num) {
    Ifj.write("num\n")
} else {
    Ifj.write("not num\n")
}
}
}
