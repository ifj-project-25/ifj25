import "ifj25" for Ifj

class Program {
    static main() {
        var result
        result = is_even(4)
        Ifj.write("4 je parne: ")
        Ifj.write("\n")
    }
    
    static is_even(n) {
        if (n == 0) {
            return true
        } else {
            var odd_result
            odd_result = is_odd(n - 1)
            return odd_result
        }
    }
    
    static is_odd(n) {
        if (n == 0) {
            return false
        } else {
            var even_result
            even_result = is_even(n - 1)
            return even_result
        }
    }
}