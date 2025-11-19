import "ifj25" for Ifj

class Program {
    
    // Getter for counter - returns current value
    static counter {
        if (__counter) {
            return __counter
        } else {
            return 0
        }
    }
    
    // Setter for counter - sets the value
    static counter=(val) {
        __counter = val
    }
    
    // Getter for lastResult
    static lastResult {
        if (__lastResult) {
            return __lastResult
        } else {
            return 0
        }
    }
    
    // Setter for lastResult
    static lastResult=(val) {
        __lastResult = val
    }
    
    // Getter for storedString
    static storedString {
        if (__storedString) {
            return __storedString
        } else {
            return ""
        }
    }
    
    // Setter for storedString
    static storedString=(val) {
        __storedString = val
    }
    
    static incrementCounter() {
        var current
        current = counter
        counter = current + 1
    }
    
    static decrementCounter() {
        var current
        current = counter
        counter = current - 1
    }
    
    static resetCounter() {
        counter = 0
    }
    
    static add(a, b) {
        var result
        result = a + b
        lastResult = result
        return result
    }
    
    static subtract(a, b) {
        var result
        result = a - b
        lastResult = result
        return result
    }
    
    static multiply(a, b) {
        var result
        result = a * b
        lastResult = result
        return result
    }
    
    static divide(a, b) {
        var result
        if (b == 0) {
            result = null
        } else {
            result = a / b
        }
        lastResult = result
        return result
    }
    
    static appendString(str) {
        var current
        current = storedString
        var temp1
        temp1 = current + str
        storedString = temp1
    }
    
    static getStringLength() {
        var current
        current = storedString
        var len
        len = Ifj.length(current)
        return len
    }
    
    static main() {
        Ifj.write("=== Testing Class Methods (Getters/Setters) ===\n")
        
        // Test 1: Counter getter (initial value)
        Ifj.write("Test 1: Counter getter (initial value)\n")
        var val1
        val1 = counter
        var val1_str
        val1_str = Ifj.str(val1)
        Ifj.write("Initial value: ")
        Ifj.write(val1_str)
        Ifj.write("\n")
        
        // Test 2: Counter setter
        Ifj.write("Test 2: Counter setter\n")
        counter = 10
        var val2
        val2 = counter
        var val2_str
        val2_str = Ifj.str(val2)
        Ifj.write("After counter = 10: ")
        Ifj.write(val2_str)
        Ifj.write("\n")
        
        // Test 3: Counter increment
        Ifj.write("Test 3: Counter increment\n")
        incrementCounter()
        var val3
        val3 = counter
        var val3_str
        val3_str = Ifj.str(val3)
        Ifj.write("After increment: ")
        Ifj.write(val3_str)
        Ifj.write("\n")
        
        // Test 4: Counter decrement
        Ifj.write("Test 4: Counter decrement\n")
        decrementCounter()
        decrementCounter()
        var val4
        val4 = counter
        var val4_str
        val4_str = Ifj.str(val4)
        Ifj.write("After two decrements: ")
        Ifj.write(val4_str)
        Ifj.write("\n")
        
        // Test 5: Counter reset
        Ifj.write("Test 5: Counter reset\n")
        resetCounter()
        var val5
        val5 = counter
        var val5_str
        val5_str = Ifj.str(val5)
        Ifj.write("After reset: ")
        Ifj.write(val5_str)
        Ifj.write("\n")
        
        // Test 6: lastResult getter (initial value)
        Ifj.write("Test 6: lastResult getter (initial value)\n")
        var calc1
        calc1 = lastResult
        var calc1_str
        calc1_str = Ifj.str(calc1)
        Ifj.write("Initial lastResult: ")
        Ifj.write(calc1_str)
        Ifj.write("\n")
        
        // Test 7: Calculator add
        Ifj.write("Test 7: Calculator add\n")
        var sum
        sum = add(15, 7)
        var sum_str
        sum_str = Ifj.str(sum)
        Ifj.write("15 + 7 = ")
        Ifj.write(sum_str)
        Ifj.write("\n")
        
        // Test 8: Calculator subtract
        Ifj.write("Test 8: Calculator subtract\n")
        var diff
        diff = subtract(20, 8)
        var diff_str
        diff_str = Ifj.str(diff)
        Ifj.write("20 - 8 = ")
        Ifj.write(diff_str)
        Ifj.write("\n")
        
        // Test 9: Calculator multiply
        Ifj.write("Test 9: Calculator multiply\n")
        var prod
        prod = multiply(6, 7)
        var prod_str
        prod_str = Ifj.str(prod)
        Ifj.write("6 * 7 = ")
        Ifj.write(prod_str)
        Ifj.write("\n")
        
        // Test 10: Calculator divide
        Ifj.write("Test 10: Calculator divide\n")
        var quot
        quot = divide(50, 5)
        var quot_str
        quot_str = Ifj.str(quot)
        Ifj.write("50 / 5 = ")
        Ifj.write(quot_str)
        Ifj.write("\n")
        
        // Test 11: Calculator divide by zero
        Ifj.write("Test 11: Calculator divide by zero\n")
        var divZero
        divZero = divide(10, 0)
        if (divZero == null) {
            Ifj.write("Division by zero returns null - PASS\n")
        } else {
            Ifj.write("ERROR: Division by zero did not return null\n")
        }
        
        // Test 12: storedString getter (initial value)
        Ifj.write("Test 12: storedString getter (initial value)\n")
        var str1
        str1 = storedString
        var len1
        len1 = getStringLength()
        var len1_str
        len1_str = Ifj.str(len1)
        Ifj.write("Initial string length: ")
        Ifj.write(len1_str)
        Ifj.write("\n")
        
        // Test 13: storedString setter
        Ifj.write("Test 13: storedString setter\n")
        storedString = "Hello"
        var str2
        str2 = storedString
        Ifj.write("Stored string: ")
        Ifj.write(str2)
        Ifj.write("\n")
        
        // Test 14: StringHolder appendString
        Ifj.write("Test 14: StringHolder appendString\n")
        appendString(" World")
        var str3
        str3 = storedString
        Ifj.write("After append: ")
        Ifj.write(str3)
        Ifj.write("\n")
        
        // Test 15: StringHolder getLength
        Ifj.write("Test 15: StringHolder getLength\n")
        var len2
        len2 = getStringLength()
        var len2_str
        len2_str = Ifj.str(len2)
        Ifj.write("Final string length: ")
        Ifj.write(len2_str)
        Ifj.write("\n")
        
        Ifj.write("=== All class method tests completed ===\n")
    }
}
