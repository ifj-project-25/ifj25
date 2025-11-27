import "ifj25" for Ifj
class Program {
static value {
return 10
}
static value=(v) {
__value = v
}
static main() {
var x
x = value
value = 20
var y
y = value
}
}
