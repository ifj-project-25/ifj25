import "ifj25" for Ifj

class Program {
    static main(){
        Ifj.write("=== Testing Ifj.length ===\n")
        
        var str1
        str1 = "hello"
        var len1
        len1 = Ifj.length(str1)
        Ifj.write("length of hello: ")
        Ifj.write(len1)
        Ifj.write("\n")
        
        var str2
        str2 = ""
        var len2
        len2 = Ifj.length(str2)
        Ifj.write("length of empty string: ")
        Ifj.write(len2)
        Ifj.write("\n")
        
        var str3
        str3 = "IFJ project 2025"
        var len3
        len3 = Ifj.length(str3)
        Ifj.write("length of IFJ project 2025: ")
        Ifj.write(len3)
        Ifj.write("\n")
        
        Ifj.write("Test completed\n")
    }
}
