import "ifj25" for Ifj

class Program {
    static main() {
        var result 
        result = factorial(5)
        Ifj.write("Faktorial 5 je: ")
        Ifj.write(Ifj.str(result))
        Ifj.write("\n")
    }
    
    static factorial(n) {
        if (n < 2) {
            return 1
        } else {
            return n * 5
        }
    }
}