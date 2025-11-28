import "ifj25" for Ifj

class Program {
    static typeChecker(value) {
        Ifj.write("Value: ")
        var valueStr
        valueStr = Ifj.str(value)
        Ifj.write(valueStr)
        Ifj.write(" | Type: ")
        
        if (value is Num) {
            Ifj.write("Num")
        } else {
            if (value is String) {
                Ifj.write("String")
            } else {
                if (value is Null) {
                    Ifj.write("Null")
                } else {

                }
            }
        }
        Ifj.write("\n")
    }
    
    static main() {
        Ifj.write("=== Type Checking Test ===\n")
        
        // Test various types
        typeChecker(42)
        typeChecker(3.14)
        typeChecker("Hello World")
        typeChecker(null)
        
        // Test type conversions
        Ifj.write("\nType conversions:\n")
        var numStr
        numStr = Ifj.str(123)
        typeChecker(numStr)
        
        var strNum
        strNum = Ifj.read_num()  // This might return Num or Null
        if (strNum != null) {
            typeChecker(strNum)
        } else {
            Ifj.write("read_num returned null\n")
        }
        
        // Test is operator in expressions
        var testVar
        testVar = "Hello"
        Ifj.write("\nis operator test: ")
        if (testVar is String) {
            Ifj.write("It's a string!\n")
        } else {
            Ifj.write("It's not a string!\n")
        }
    }
}