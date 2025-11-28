import "ifj25" for Ifj

class Program {
    static memory {
        return __memory
    }
    
    static memory=(value) {
        __memory = value
    }
    
    static test(a, b) {
        memory = a
    }
    
    static main() {
        test(5, 10)
        var m
        m = memory
        var mStr
        mStr = Ifj.str(m)
        Ifj.write(mStr)
        Ifj.write("\n")
    }
}
