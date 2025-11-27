import "ifj25" for Ifj
class Program {
static add(a, b) {
var tmp
tmp = a + b
return tmp
}
static main() {
var x
var y
var z
var w
x = 3
y = 4
z = add(x, y)
w = add(10, add(1, 2))
}
}
