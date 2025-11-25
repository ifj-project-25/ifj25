import "ifj25" for Ifj

class Program {
    static main(){
        Ifj.write("=== Testing Ifj.substring ===\n")
        
        var str4
        str4 = "hello world"
        var sub1
        sub1 = Ifj.substring(str4, 0, 5)
        Ifj.write("substring(0, 5): ")
        Ifj.write(sub1)
        Ifj.write("\n")
        
        var sub2
        sub2 = Ifj.substring(str4, 6, 11)
        Ifj.write("substring(6, 11): ")
        Ifj.write(sub2)
        Ifj.write("\n")
        
        var sub3
        sub3 = Ifj.substring(str4, 0, 0)
        Ifj.write("substring(0, 0) empty: ")
        Ifj.write(sub3)
        Ifj.write("\n")
        
        Ifj.write("Testing null returns:\n")
        var sub4
        sub4 = Ifj.substring(str4, -1, 5)
        if (sub4 is Null) {
            Ifj.write("substring(-1, 5): null OK\n")
        } else {
            Ifj.write("ERROR sub4\n")
        }
        
        var sub5
        sub5 = Ifj.substring(str4, 0, -1)
        if (sub5 is Null) {
            Ifj.write("substring(0, -1): null OK\n")
        } else {
            Ifj.write("ERROR sub5\n")
        }
        
        var sub6
        sub6 = Ifj.substring(str4, 5, 3)
        if (sub6 is Null) {
            Ifj.write("substring(5, 3): null OK\n")
        } else {
            Ifj.write("ERROR sub6\n")
        }
        
        var sub7
        sub7 = Ifj.substring(str4, 20, 25)
        if (sub7 is Null) {
            Ifj.write("substring(20, 25): null OK\n")
        } else {
            Ifj.write("ERROR sub7\n")
        }
        
        var sub8
        sub8 = Ifj.substring(str4, 0, 20)
        if (sub8 is Null) {
            Ifj.write("substring(0, 20): null OK\n")
        } else {
            Ifj.write("ERROR sub8\n")
        }
        
        Ifj.write("Test completed\n")
    }
}
