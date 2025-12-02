import "ifj25" for Ifj

class Program {
    static processNumbers() {
        Ifj.write("Enter numbers to process (empty to finish):\n")
        
        var sum
        sum = 0
        var count
        count = 0
        var input
        var continueLoop
        continueLoop = 1
        
        while (continueLoop != 0) {
            Ifj.write("Number ")
            var countPlus1
            countPlus1 = count + 1
            var countStr
            countStr = Ifj.str(countPlus1)
            Ifj.write(countStr)
            Ifj.write(": ")
            
            input = Ifj.read_num()
            
            if (input == null) {
                Ifj.write("End of input or invalid number\n")
                continueLoop = 0
            } else{
                sum = sum + input
                count = count + 1
            }
        }
        
        if (count > 0) {
            var average
            average = sum / count
            Ifj.write("\nResults:\n")
            Ifj.write("Total numbers: ")
            var countStr2
            countStr2 = Ifj.str(count)
            Ifj.write(countStr2)
            Ifj.write("\nSum: ")
            var sumStr
            sumStr = Ifj.str(sum)
            Ifj.write(sumStr)
            Ifj.write("\nAverage: ")
            var avgStr
            avgStr = Ifj.str(average)
            Ifj.write(avgStr)
            Ifj.write("\n")
        } else {
            Ifj.write("No numbers entered\n")
        }
    }
    
    static stringOperations() {
        Ifj.write("\n=== String Operations ===\n")
        Ifj.write("Enter a string: ")
        
        var inputStr
        inputStr = Ifj.read_str()
        if (inputStr != null) {
            Ifj.write("You entered: ")
            Ifj.write(inputStr)
            Ifj.write("\n")
            
            Ifj.write("Length: ")
            Ifj.write(Ifj.str(Ifj.length(inputStr)))
            Ifj.write("\n")
            
            Ifj.write("First 3 characters: ")
            Ifj.write(Ifj.substring(inputStr, 0, 3))
            Ifj.write("\n")
            
            Ifj.write("Repeated 2 times: ")
            Ifj.write(inputStr * 2)
            Ifj.write("\n")
        } else {
            Ifj.write("No string entered\n")
        }
    }
    
    static main() {
        Ifj.write("=== Comprehensive I/O Test ===\n")
        processNumbers()
        stringOperations()
    }
}