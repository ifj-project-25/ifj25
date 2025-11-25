import "ifj25" for Ifj

class Program {
    static main(){
        Ifj.write("=== Testing while loops ===\n")
        
        // Test 1: Simple counting loop
        Ifj.write("Test 1: Count from 1 to 5\n")
        var i
        i = 1
        while (i <= 5) {
            Ifj.write(Ifj.str(i))
            Ifj.write(" ")
            i = i + 1
        }
        Ifj.write("\n")
        
        // Test 2: Countdown loop
        Ifj.write("Test 2: Countdown from 5 to 1\n")
        var j
        j = 5
        while (j > 0) {
            Ifj.write(Ifj.str(j))
            Ifj.write(" ")
            j = j - 1
        }
        Ifj.write("\n")
        
        // Test 3: Loop with string comparison
        Ifj.write("Test 3: !!Skipped invalid test!!!  String iteration\n")
        // var k
        // k = 0
        // var str
        // str = "hello"
        // while (k < Ifj.length(str)) {
        //     Ifj.write(Ifj.substring(str, k, k + 1))
        //     k = k + 1
        // }
        // Ifj.write("\n")
        
        // Test 4: Nested loops
        Ifj.write("Test 4:Nested loops (3x3 grid)\n")
        var row
        row = 0
        var col
        while (row < 3) {
            col = 0
            while (col < 3) {
                Ifj.write("*")
                col = col + 1
            }
            Ifj.write("\n")
            row = row + 1
        }
        
        // Test 5: Loop that doesn't execute
        Ifj.write("Test 5: Loop with false condition (should not print X)\n")
        var m
        m = 10
        while (m < 5) {
            Ifj.write("X")
            m = m + 1
        }
        Ifj.write("Test 5 completed - no X printed\n")
        
        // Test 6: Sum calculation
        Ifj.write("Test 6: Sum of numbers 1 to 10\n")
        var n
        n = 1
        var sum
        sum = 0
        while (n <= 10) {
            sum = sum + n
            n = n + 1
        }
        Ifj.write("Sum: ")
        Ifj.write(sum)
        Ifj.write("\n")
        
        // Test 7: Factorial calculation
        Ifj.write("Test 7: Factorial of 5\n")
        var fact_n
        fact_n = 5
        var factorial
        factorial = 1
        var counter
        counter = 1
        while (counter <= fact_n) {
            factorial = factorial * counter
            counter = counter + 1
        }
        Ifj.write("5! = ")
        Ifj.write(factorial)
        Ifj.write("\n")
        
        // Test 8: While loop with if inside
        Ifj.write("Test 8: Even numbers from 2 to 10\n")
        var p
        p = 1
        while (p <= 10) {
            Ifj.write("Checking ")
            Ifj.write(Ifj.str(p))
            Ifj.write(":\n")
            if (p == 2) {
                Ifj.write(Ifj.str(p))
                Ifj.write(" ")
            } else {
                if (p == 4) {
                    Ifj.write(Ifj.str(p))
                    Ifj.write(" ")
                } else {
                    if (p == 6) {
                        Ifj.write(Ifj.str(p))
                        Ifj.write(" ")
                    } else {
                        if (p == 8) {
                            Ifj.write(Ifj.str(p))
                            Ifj.write(" ")
                        } else {
                            if (p == 10) {
                                Ifj.write(Ifj.str(p))
                                Ifj.write(" ")
                            } else {
                            }
                        }
                    }
                }
            }
            p = p + 1
        }
        Ifj.write("\n")
        
        Ifj.write("=== All while tests completed ===\n")
    }
}
