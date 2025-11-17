import "ifj25" for Ifj

class Program {
    static main() {
        Ifj.write("Zadajte vase meno: ")
        var name
        name = Ifj.read_str()
        
        if (name != null) {
            Ifj.write("Ahoj, ")
            Ifj.write(name)
            Ifj.write("!\n")
            
            Ifj.write("Zadajte vas vek: ")
            var age
            age = Ifj.read_num()
            
            if (age != null) {
                Ifj.write("Mate ")
                Ifj.write(Ifj.str(age))
                Ifj.write(" rokov.\n")
                
                if (age < 18) {
                    Ifj.write("Ste mladistvy.\n")
                } else {
                    Ifj.write("Ste dospely.\n")
                }
            } else {
                Ifj.write("Neplatny vek!\n")
            }
        } else {
            Ifj.write("Neplatne meno!\n")
        }
    }
}