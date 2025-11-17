import "ifj25" for Ifj
class Program {

static A(x) {
if (x < 1) { 
    return 1 
    }else{

}
var y
y = x - 1
return B(y)
}

static B(z) {
if (z == 0) { 
    return 10 
    }else{

    }

var r
r = z - 1
return A(r)
}

static main() {
var r
r = A(4)
Ifj.write(r)
}
}
