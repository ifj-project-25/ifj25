import "ifj25" for Ifj

class Program {
    static main(){
        Ifj.write("=== Testing Ifj.strcmp ===\n")
        
        var cmp1
        cmp1 = Ifj.strcmp("abc", "xyz")
        Ifj.write("strcmp(abc, xyz): ")
        Ifj.write(cmp1)
        Ifj.write("\n")
        
        var cmp2
        cmp2 = Ifj.strcmp("hello", "hello")
        Ifj.write("strcmp(hello, hello): ")
        Ifj.write(cmp2)
        Ifj.write("\n")
        
        var cmp3
        cmp3 = Ifj.strcmp("zebra", "apple")
        Ifj.write("strcmp(zebra, apple): ")
        Ifj.write(cmp3)
        Ifj.write("\n")
        
        Ifj.write("Test completed\n")
    }
}
