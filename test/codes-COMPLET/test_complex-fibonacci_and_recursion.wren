import "ifj25" for Ifj

class Program {
    
    
    static callCount {
        if (__callCount is Null) {
            return 0
        } else {

        }
        return __callCount
    }
    
    static callCount=(value) {
        __callCount = value
    }
    
    static incrementCounter() {
        callCount = callCount + 1
    }
    
    static fibonacci(n) {
        incrementCounter()
        
        if (n < 0) {
            return null
        } else {

        }
        if (n == 0) {
            return 0
        } else {

        }
        if (n == 1) {
            return 1
        } else {

        }
        
        var a
        a = fibonacci(n - 1)
        var b
        b = fibonacci(n - 2)
        return a + b
    }
    
    static factorial(n) {
        incrementCounter()
        
        if (n < 0) {
            return null
        } else {

        }
        if (n == 0) {
            return 1
        } else {

        }
        
        var prev
        prev = factorial(n - 1)
        return n * prev
    }
    
    static main() {
        Ifj.write("=== Recursion Test ===\n")
        
        // Test Fibonacci
        Ifj.write("Fibonacci sequence:\n")
        var i
        i = 0
        while (i <= 6) {
            callCount = 0
            var fib
            fib = fibonacci(i)
            Ifj.write("fib(")
            Ifj.write(Ifj.str(i))
            Ifj.write(") = ")
            Ifj.write(Ifj.str(fib))
            Ifj.write(" (")
            Ifj.write(Ifj.str(callCount))
            Ifj.write(" calls)\n")
            i = i + 1
        }
        
        // Test Factorial
        Ifj.write("\nFactorial:\n")
        callCount = 0
        var fact5
        fact5 = factorial(5)
        Ifj.write("5! = ")
        Ifj.write(Ifj.str(fact5))
        Ifj.write(" (")
        Ifj.write(Ifj.str(callCount))
        Ifj.write(" calls)\n")
    }
}