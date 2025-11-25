import "ifj25" for Ifj

class Program {
    
    // Test 1: Simple function with no parameters
    static greet() {
        Ifj.write("Hello from greet function!\n")
    }
    
    // Test 2: Function with one parameter
    static greetName(nm) {
        Ifj.write("Hello, ")
        Ifj.write(nm)
        Ifj.write("!\n")
    }
    
    // Test 3: Function with return value
    static add5(val) {
        var res
        res = val + 5
        return res
    }
    
    // Test 4: Function with multiple parameters
    static addTwo(a, b) {
        var s
        s = a + b
        return s
    }
    
    // Test 5: Recursive function
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
    
    // Test 6: Function returning null
    static returnNull() {
        return null
    }
    
    // Test 7: Function with string parameter
    static printMessage(message) {
        Ifj.write("Message: ")
        Ifj.write(message)
        Ifj.write("\n")
    }
    
    // Test 8: Helper for nested calls
    static multiply(x, y) {
        var prod
        prod = x * y
        return prod
    }
    
    // Test 9: Function with conditional return
    static absolute(num) {
        var result
        if (num < 0) {
            result = 0 - num
        } else {
            result = num
        }
        return result
    }
    
    // Test 10: Function calling another function
    static square(n) {
        var sq
        sq = multiply(n, n)
        return sq
    }
    
    static main() {
        Ifj.write("=== Testing Functions ===\n")
        
        // Test 1: Simple function with no parameters
        Ifj.write("Test 1: Function with no parameters\n")
        greet()
        
        // Test 2: Function with one parameter
        Ifj.write("Test 2: Function with one parameter\n")
        var name
        name = "World"
        greetName(name)
        
        // Test 3: Function with return value
        Ifj.write("Test 3: Function with return value\n")
        var result
        result = add5(10)
        var result_str
        result_str = Ifj.str(result)
        Ifj.write("10 + 5 = ")
        Ifj.write(result_str)
        Ifj.write("\n")
        
        // Test 4: Function with multiple parameters
        Ifj.write("Test 4: Function with multiple parameters\n")
        var sum
        sum = addTwo(7, 3)
        var sum_str
        sum_str = Ifj.str(sum)
        Ifj.write("7 + 3 = ")
        Ifj.write(sum_str)
        Ifj.write("\n")
        
        // Test 5: Recursive function
        Ifj.write("Test 5: Recursive factorial\n")
        var fact
        Ifj.write("Calculating 5!...\n")
        fact = factorial(5)
        Ifj.write("Factorial computed.\n")
        var fact_str
        fact_str = Ifj.str(fact)
        Ifj.write("5! = ")
        Ifj.write(fact_str)
        Ifj.write("\n")
        
        // Test 6: Function returning null
        Ifj.write("Test 6: Function returning null\n")
        var nullVal
        nullVal = returnNull()
        if (nullVal == null) {
            Ifj.write("Returned null successfully\n")
        } else {
            Ifj.write("ERROR: Did not return null\n")
        }
        
        // Test 7: Function with string parameter
        Ifj.write("Test 7: Function with string parameter\n")
        var msg
        msg = "Hello from test"
        printMessage(msg)
        
        // Test 8: Nested function calls
        Ifj.write("Test 8: Nested function calls\n")
        var nested
        nested = multiply(add5(5), 2)
        var nested_str
        nested_str = Ifj.str(nested)
        Ifj.write("multiply(add5(5), 2) = ")
        Ifj.write(nested_str)
        Ifj.write("\n")
        
        // Test 9: Function with conditional return
        Ifj.write("Test 9: Function with conditional return\n")
        var abs1
        abs1 = absolute(0 - 15)
        var abs1_str
        abs1_str = Ifj.str(abs1)
        Ifj.write("absolute(-15) = ")
        Ifj.write(abs1_str)
        Ifj.write("\n")
        
        var abs2
        abs2 = absolute(20)
        var abs2_str
        abs2_str = Ifj.str(abs2)
        Ifj.write("absolute(20) = ")
        Ifj.write(abs2_str)
        Ifj.write("\n")
        
        // Test 10: Function calling another function
        Ifj.write("Test 10: Function calling another function\n")
        var power
        power = square(4)
        var power_str
        power_str = Ifj.str(power)
        Ifj.write("square(4) = ")
        Ifj.write(power_str)
        Ifj.write("\n")
        
        Ifj.write("=== All function tests completed ===\n")
    }
}