import "ifj25" for Ifj

class Program {
    static main() {
        var x
        var y
        var z
        var is_num
        var is_string
        var is_null
        
        x = 42
        y = "text"
        z = null
        
        is_num = x is Num
        is_string = y is String
        is_null = z is Null
        
      
        Ifj.write("\n")
        
        Ifj.write("y je String: ")
        
        Ifj.write("\n")
        
        Ifj.write("z je Null: ")
    
        Ifj.write("\n")
    }
}