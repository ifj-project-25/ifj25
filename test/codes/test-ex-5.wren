import "ifj25" for Ifj
class Program {

static down(n) {
Ifj.write(n)
Ifj.write(" ")
if (n != 0) {
    var m
    m = n - 1
    return down(m)
}else{

}
return 0
}

static main() {
down(5)
}
}
