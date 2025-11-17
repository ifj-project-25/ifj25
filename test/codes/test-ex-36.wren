import "ifj25" for Ifj

class Program {
    static main() {
        var text
        var first_char
        var rest
        var ascii_val
        
        text = "Hello World"
        first_char = Ifj.substring(text, 0, 1)
        rest = Ifj.substring(text, 1, Ifj.length(text))
        
        Ifj.write("Prve pismeno: ")
        Ifj.write(first_char)
        Ifj.write("\n")
        Ifj.write("Zvysok: ")
        Ifj.write(rest)
        Ifj.write("\n")
        
        // ASCII hodnota znaku
        ascii_val = Ifj.ord(text, 0)
        Ifj.write("ASCII hodnota 'H': ")
        Ifj.write(Ifj.str(ascii_val))
        Ifj.write("\n")
    }
}