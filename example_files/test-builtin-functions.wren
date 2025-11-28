import "ifj25" for Ifj

class Program {
    static main(){
        // Test Ifj.length(s : String) → Num
        Ifj.write("=== Testing Ifj.length ===\n")
        var str1
        str1 = "hello"
        var len1
        len1 = Ifj.length(str1)
        Ifj.write("length of 'hello': ")
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
        Ifj.write("length of 'IFJ project 2025': ")
        Ifj.write(len3)
        Ifj.write("\n\n")
        
        // Test Ifj.substring(s : String, i : Num, j : Num) → String | Null
        Ifj.write("=== Testing Ifj.substring ===\n")
        var str4
        str4 = "hello world"
        var sub1
        sub1 = Ifj.substring(str4, 0, 5)
        Ifj.write("substring(0, 5) of 'hello world': '")
        Ifj.write(sub1)
        Ifj.write("'\n")
        
        var sub2
        sub2 = Ifj.substring(str4, 6, 11)
        Ifj.write("substring(6, 11) of 'hello world': '")
        Ifj.write(sub2)
        Ifj.write("'\n")
        
        var sub3
        sub3 = Ifj.substring(str4, 0, 0)
        Ifj.write("substring(0, 0) should be empty: '")
        Ifj.write(sub3)
        Ifj.write("'\n")
        
        // Test null returns
        Ifj.write("Testing null returns:\n")
        var sub4
        sub4 = Ifj.substring(str4, -1, 5)
        if (sub4 is Null) {
            Ifj.write("substring(-1, 5): null (i < 0) - OK\n")
        } else {
            Ifj.write("ERROR: sub4 should be null\n")
        }
        
        var sub5
        sub5 = Ifj.substring(str4, 0, -1)
        if (sub5 is Null) {
            Ifj.write("substring(0, -1): null (j < 0) - OK\n")
        } else {
            Ifj.write("ERROR: sub5 should be null\n")
        }
        
        var sub6
        sub6 = Ifj.substring(str4, 5, 3)
        if (sub6 is Null) {
            Ifj.write("substring(5, 3): null (i > j) - OK\n")
        } else {
            Ifj.write("ERROR: sub6 should be null\n")
        }
        
        var sub7
        sub7 = Ifj.substring(str4, 20, 25)
        if (sub7 is Null) {
            Ifj.write("substring(20, 25): null (i >= length) - OK\n")
        } else {
            Ifj.write("ERROR: sub7 should be null\n")
        }
        
        var sub8
        sub8 = Ifj.substring(str4, 0, 20)
        if (sub8 is Null) {
            Ifj.write("substring(0, 20): null (j > length) - OK\n\n")
        } else {
            Ifj.write("ERROR: sub8 should be null\n\n")
        }
        
        // Test Ifj.strcmp(s1 : String, s2 : String) → Num
        Ifj.write("=== Testing Ifj.strcmp ===\n")
        var cmp1
        cmp1 = Ifj.strcmp("abc", "xyz")
        Ifj.write("strcmp('abc', 'xyz'): ")
        Ifj.write(cmp1)
        Ifj.write(" (should be -1)\n")
        
        var cmp2
        cmp2 = Ifj.strcmp("hello", "hello")
        Ifj.write("strcmp('hello', 'hello'): ")
        Ifj.write(cmp2)
        Ifj.write(" (should be 0)\n")
        
        var cmp3
        cmp3 = Ifj.strcmp("zebra", "apple")
        Ifj.write("strcmp('zebra', 'apple'): ")
        Ifj.write(cmp3)
        Ifj.write(" (should be 1)\n\n")
        
        // Test Ifj.ord(s : String, i : Num) → Num
        Ifj.write("=== Testing Ifj.ord ===\n")
        var str5
        str5 = "ABC"
        var ord1
        ord1 = Ifj.ord(str5, 0)
        Ifj.write("ord('ABC', 0): ")
        Ifj.write(ord1)
        Ifj.write(" (should be 65 for 'A')\n")
        
        var ord2
        ord2 = Ifj.ord(str5, 1)
        Ifj.write("ord('ABC', 1): ")
        Ifj.write(ord2)
        Ifj.write(" (should be 66 for 'B')\n")
        
        var ord3
        ord3 = Ifj.ord(str5, 2)
        Ifj.write("ord('ABC', 2): ")
        Ifj.write(ord3)
        Ifj.write(" (should be 67 for 'C')\n")
        
        var ord4
        ord4 = Ifj.ord("", 0)
        Ifj.write("ord('', 0): ")
        Ifj.write(ord4)
        Ifj.write(" (should be 0 for empty string)\n")
        
        var ord5
        ord5 = Ifj.ord(str5, 10)
        Ifj.write("ord('ABC', 10): ")
        Ifj.write(ord5)
        Ifj.write(" (should be 0 for out of bounds)\n\n")
        
        // Test Ifj.chr(i : Num) → String
        Ifj.write("=== Testing Ifj.chr ===\n")
        var chr1
        chr1 = Ifj.chr(65)
        Ifj.write("chr(65): '")
        Ifj.write(chr1)
        Ifj.write("' (should be 'A')\n")
        
        var chr2
        chr2 = Ifj.chr(97)
        Ifj.write("chr(97): '")
        Ifj.write(chr2)
        Ifj.write("' (should be 'a')\n")
        
        var chr3
        chr3 = Ifj.chr(48)
        Ifj.write("chr(48): '")
        Ifj.write(chr3)
        Ifj.write("' (should be '0')\n")
        
        var chr4
        chr4 = Ifj.chr(32)
        Ifj.write("chr(32): '")
        Ifj.write(chr4)
        Ifj.write("' (should be space)\n")
        
        Ifj.write("\n=== All tests completed ===\n")
    }
}
