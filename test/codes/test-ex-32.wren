import "ifj25" for Ifj

class Program {
    static main() {
        Ifj.write("Zadajte cislo: ")
        var input 
        input = Ifj.read_num()
        
        if (input != null) {
            var i
            i = 0
            while (i < input) {
                Ifj.write("Iteracia: ")
                Ifj.write(Ifj.str(i))
                Ifj.write("\n")
                i = i + 1
            }
        } else {
            Ifj.write("Chyba pri nacitani cisla!\n")
        }
    }
}