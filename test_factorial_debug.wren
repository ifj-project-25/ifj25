import "ifj25" for Ifj

class Program {
    static factorial(n) {
        var n_str
        n_str = Ifj.str(n)
        Ifj.write("factorial(")
        Ifj.write(n_str)
        Ifj.write(") called\n")
        
        var result
        if (n < 2) {
            Ifj.write("  Base case: returning 1\n")
            result = 1
        } else {
            var n_minus_1
            n_minus_1 = n - 1
            Ifj.write("  Recursive case: calling factorial(")
            var nm1_str
            nm1_str = Ifj.str(n_minus_1)
            Ifj.write(nm1_str)
            Ifj.write(")\n")
            
            var fact_prev
            fact_prev = factorial(n_minus_1)
            
            var fp_str
            fp_str = Ifj.str(fact_prev)
            Ifj.write("  Got fact_prev = ")
            Ifj.write(fp_str)
            Ifj.write("\n")
            
            result = n * fact_prev
            
            var r_str
            r_str = Ifj.str(result)
            Ifj.write("  Computed result = ")
            Ifj.write(n_str)
            Ifj.write(" * ")
            Ifj.write(fp_str)
            Ifj.write(" = ")
            Ifj.write(r_str)
            Ifj.write("\n")
        }
        
        var ret_str
        ret_str = Ifj.str(result)
        Ifj.write("  Returning ")
        Ifj.write(ret_str)
        Ifj.write("\n")
        return result
    }
    
    static main() {
        var f
        f = factorial(3)
        var f_str
        f_str = Ifj.str(f)
        Ifj.write("Final result: factorial(3) = ")
        Ifj.write(f_str)
        Ifj.write("\n")
    }
}
