import "ifj25" for Ifj

class Program {
    static main(){
        Ifj.write("=== Testing Ifj.chr ===\n")
        
        var chr1
        chr1 = Ifj.chr(65)
        Ifj.write("chr(65): ")
        Ifj.write(chr1)
        Ifj.write("\n")
        
        var chr2
        chr2 = Ifj.chr(97)
        Ifj.write("chr(97): ")
        Ifj.write(chr2)
        Ifj.write("\n")
        
        var chr3
        chr3 = Ifj.chr(48)
        Ifj.write("chr(48): ")
        Ifj.write(chr3)
        Ifj.write("\n")
        
        var chr4
        chr4 = Ifj.chr(32)
        Ifj.write("chr(32): ")
        Ifj.write(chr4)
        Ifj.write("\n")
        
        Ifj.write("Test completed\n")
    }
}
