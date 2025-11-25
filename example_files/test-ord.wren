import "ifj25" for Ifj

class Program {
    static main(){
        Ifj.write("=== Testing Ifj.ord ===\n")
        
        var str5
        str5 = "ABC"
        var ord1
        ord1 = Ifj.ord(str5, 0)
        Ifj.write("ord(ABC, 0): ")
        Ifj.write(ord1)
        Ifj.write("\n")
        
        var ord2
        ord2 = Ifj.ord(str5, 1)
        Ifj.write("ord(ABC, 1): ")
        Ifj.write(ord2)
        Ifj.write("\n")
        
        var ord3
        ord3 = Ifj.ord(str5, 2)
        Ifj.write("ord(ABC, 2): ")
        Ifj.write(ord3)
        Ifj.write("\n")
        
        var ord4
        ord4 = Ifj.ord("", 0)
        Ifj.write("ord(empty, 0): ")
        Ifj.write(ord4)
        Ifj.write("\n")
        
        var ord5
        ord5 = Ifj.ord(str5, 10)
        Ifj.write("ord(ABC, 10): ")
        Ifj.write(ord5)
        Ifj.write("\n")
        
        Ifj.write("Test completed\n")
    }
}
