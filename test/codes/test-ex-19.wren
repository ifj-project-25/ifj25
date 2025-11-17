import "ifj25" for Ifj
class Program {
static main() {
var x
x = "outer"

{
    var x
    x = "inner"
    Ifj.write(x)
    Ifj.write("\n")
}

Ifj.write(x)
}
}
