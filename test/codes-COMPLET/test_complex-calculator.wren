import "ifj25" for Ifj

class Program {
    
    static memory {
        return __memory
    }
    
    static memory=(value) {
        __memory = value
    }
    
    static history {
        if (__history is Null) {
            return ""
        } else {

        }
        return __history
    }
    
    static history=(value) {
        __history = value
    }
    
    static addToHistory(operation, result) {
        var resultStr
        resultStr = Ifj.str(result)
        var newEntry
        newEntry = operation + " = " + resultStr + "; "
        var current
        current = history
        history = current + newEntry
    }
    
    static calculate(operation, a, b) {
        var result
        if (operation == "+") {
            result = a + b
        } else {
            if (operation == "-") {
                result = a - b
            } else {
                if (operation == "*") {
                    result = a * b
                } else {
                    if (operation == "/") {
                        if (b == 0) {
                            result = null
                        } else {
                            result = a / b
                        }
                    } else {
                        result = null
                    }
                }
            }
        }
        
        if (result != null) {
            addToHistory(operation, result)
            memory = result
        } else {

        }
        return result
    }
    
    static clearMemory() {
        memory = 0
        history = ""
    }
    
    static displayStatus() {
        Ifj.write("Memory: ")
        var memStr
        memStr = Ifj.str(memory)
        Ifj.write(memStr)
        Ifj.write("\nHistory: ")
        Ifj.write(history)
        Ifj.write("\n")
    }
    
    static main() {
        Ifj.write("=== Calculator with Memory Test ===\n")
        
        // Test calculations
        var res1
        res1 = calculate("+", 10, 5)
        var res2
        res2 = calculate("*", res1, 2)
        var res3
        res3 = calculate("-", res2, 8)
        
        displayStatus()
        
        // Test division
        var res4
        res4 = calculate("/", 20, 4)
        Ifj.write("20 / 4 = ")
        var res4Str
        res4Str = Ifj.str(res4)
        Ifj.write(res4Str)
        Ifj.write("\n")
        
        // Test division by zero
        var res5
        res5 = calculate("/", 10, 0)
        if (res5 == null) {
            Ifj.write("Division by zero handled correctly\n")
        } else {

        }
        
        // Clear and show final state
        clearMemory()
        Ifj.write("After clear:\n")
        displayStatus()
    }
}