import "ifj25" for Ifj

class Program {
    static main() {
        var s1
        var s2
        var combined
        var repeated
        var length
        
        s1 = "Ahoj"
        s2 = " svet"
        combined = s1 + s2  // Konkatenácia
        Ifj.write(combined)
        Ifj.write("\n")
        
        // Iterácia reťazca
        repeated = "abc" * 3
        Ifj.write(repeated)  // Vypíše: abcabcabc
        Ifj.write("\n")
        
        // Dĺžka reťazca
        length = Ifj.length(combined)
        Ifj.write("Dlzka retazca: ")
        Ifj.write(Ifj.str(length))
        Ifj.write("\n")
    }
}