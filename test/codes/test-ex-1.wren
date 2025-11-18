import "ifj25" for Ifj
class Program {
static main() {
Ifj.write("Test 4:Nested loops (3x3 grid)\n")
        var row
        row = 0
        while (row < 3) {
            var col
            col = 0
            while (col < 3) {
                Ifj.write("*")
                col = col + 1
            }
            Ifj.write("\n")
            row = row + 1
        }
}
}
