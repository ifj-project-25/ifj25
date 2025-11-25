import "ifj25" for Ifj
class Program {

    static main(){
        var gej
        gej = 10
        Ifj.write("Not using Ifj.str  10\n")
        Ifj.write(gej)
        Ifj.write("\n")
        Ifj.write("Using Ifj.str\n")
        Ifj.write(Ifj.str(gej))
        Ifj.write("\n")
        gej = 3.14
        Ifj.write("Not using Ifj.str  3.14\n")
        Ifj.write(gej)
        Ifj.write("\n")
        Ifj.write("Using Ifj.str\n")
        Ifj.write(Ifj.str(gej))
        Ifj.write("\n")
        }
}