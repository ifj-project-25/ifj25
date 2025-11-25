import "ifj25" for Ifj

class Program {
    static counter {
        if (__counter) {
            return __counter
        } else {
            return 0
        }
    }
    
    static counter=(val) {
        __counter = val
    }
    
    static main() {
        Ifj.write("Test 1: Initial\n")
        var v1
        v1 = counter
        var v1_str
        v1_str = Ifj.str(v1)
        Ifj.write(v1_str)
        Ifj.write("\n")
        
        Ifj.write("Test 2: Set to 10\n")
        counter = 10
        var v2
        v2 = counter
        var v2_str
        v2_str = Ifj.str(v2)
        Ifj.write(v2_str)
        Ifj.write("\n")
    }
}
