import "ifj25" for Ifj

class Program {
    static main() {
        counter = 5  // Volá setter
        var counter_value
        counter_value = counter  // Volá getter
        Ifj.write("Hodnota counter: ")
        Ifj.write(Ifj.str(counter_value))
        Ifj.write("\n")
    }
    
    // Statický getter
    static counter {
        return _counter
    }
    
    // Statický setter
    static counter = (value) {
        _counter = value * 2  // Uloží dvojnásobnú hodnotu
    }
}