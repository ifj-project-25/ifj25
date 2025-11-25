import "ifj25" for Ifj

class Program {
    static factorial(n) {
        var result
        if (n < 2) {
            result = 1
        } else {
            var n_minus_1
            n_minus_1 = n - 1
            var fact_prev
            fact_prev = factorial(n_minus_1)
            result = n * fact_prev
        }
        return result
    }
    
    static main() {
        var f
        f = factorial(3)
        var f_str
        f_str = Ifj.str(f)
        Ifj.write("factorial(3) = ")
        Ifj.write(f_str)
        Ifj.write("\n")
    }
}
