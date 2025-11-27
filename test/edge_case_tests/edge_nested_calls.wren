import "ifj25" for Ifj
class Program {
static inner() {
return 5
}
static outer(x) {
return x
}
static main() {
var r
r = outer(inner())
}
}
