import "ifj25" for Ifj
class Program {
static main() {
var s
s = "abcdef"

if (s is String) {
    var x
    x = Ifj.substring(s, 1, 4)
    Ifj.write(x)
}else{
    
}
}
}
