import "ifj25" for Ifj

class Program {
    static main(){
        Ifj.write("=== Testing if statements ===\n")
        
        // Test 1: Simple if-else with true condition
        var x
        x = 10
        if (x == 10) {
            Ifj.write("Test 1: x is 10 - PASS\n")
        } else {
            Ifj.write("Test 1: x is not 10 - FAIL\n")
        }
        
        // Test 2: Simple if-else with false condition
        if (x == 5) {
            Ifj.write("Test 2: x is 5 - FAIL\n")
        } else {
            Ifj.write("Test 2: x is not 5 - PASS\n")
        }
        
        // Test 3: Nested if statements
        var y
        y = 20
        if (x > 5) {
            if (y > 15) {
                Ifj.write("Test 3: x > 5 and y > 15 - PASS\n")
            } else {
                Ifj.write("Test 3: nested condition failed - FAIL\n")
            }
        } else {
            Ifj.write("Test 3: x <= 5 - FAIL\n")
        }
        
        // Test 4: if with else
        if (x < 20) {
            Ifj.write("Test 4: x < 20 - PASS\n")
        } else {
            Ifj.write("Test 4: x >= 20 - FAIL\n")
        }
        
        // Test 5: Comparison operators
        if (x >= 10) {
            Ifj.write("Test 5a: x >= 10 - PASS\n")
        } else {
            Ifj.write("Test 5a: x < 10 - FAIL\n")
        }
        
        if (x <= 10) {
            Ifj.write("Test 5b: x <= 10 - PASS\n")
        } else {
            Ifj.write("Test 5b: x > 10 - FAIL\n")
        }
        
        if (x != 5) {
            Ifj.write("Test 5c: x != 5 - PASS\n")
        } else {
            Ifj.write("Test 5c: x == 5 - FAIL\n")
        }
        
        // Test 6: String comparison
        var str1
        str1 = "hello"
        var str2
        str2 = "hello"
        if (str1 == str2) {
            Ifj.write("Test 6: string equality - PASS\n")
        } else {
            Ifj.write("Test 6: string equality - FAIL\n")
        }
        
        // Test 7: Type checking with is operator
        var number
        number = 42
        if (number is Num) {
            Ifj.write("Test 7a: number is Num - PASS\n")
        } else {
            Ifj.write("Test 7a: number is Num - FAIL\n")
        }
        
        if (str1 is String) {
            Ifj.write("Test 7b: str1 is String - PASS\n")
        } else {
            Ifj.write("Test 7b: str1 is String - FAIL\n")
        }
        
        var nullVar
        nullVar = null
        if (nullVar is Null) {
            Ifj.write("Test 7c: nullVar is Null - PASS\n")
        } else {
            Ifj.write("Test 7c: nullVar is Null - FAIL\n")
        }
        
        // Test 8: Multiple conditions with arithmetic
        var a
        a = 5
        var b
        b = 3
        if (a + b == 8) {
            Ifj.write("Test 8: arithmetic in condition - PASS\n")
        } else {
            Ifj.write("Test 8: arithmetic in condition - FAIL\n")
        }
        
        Ifj.write("=== All if tests completed ===\n")
    }
}
