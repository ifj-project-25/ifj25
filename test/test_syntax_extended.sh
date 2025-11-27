#!/bin/bash

# Extended Syntax Test Suite for Syntactic Analyzer
# Additional comprehensive tests for syntax validation
# Focuses on advanced scenarios, operator combinations, and complex structures

# Color codes
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Test counters
total=0
passed=0
failed=0

# Create temporary test directory
TEST_DIR="test/extended_syntax_tests"
mkdir -p "$TEST_DIR"

echo -e "${BLUE}=== Extended Syntax Test Suite ===${NC}\n"

# Function to run a test
run_test() {
    local test_name="$1"
    local test_code="$2"
    local expected_exit_code="$3"
    local description="$4"
    local test_file="$TEST_DIR/${test_name}.wren"
    
    total=$((total + 1))
    
    # Create test file
    echo "$test_code" > "$test_file"
    
    # Run the compiler
    ./main < "$test_file" > /dev/null 2>&1
    actual_exit_code=$?
    
    # Check result
    if [ $actual_exit_code -eq $expected_exit_code ]; then
        echo -e "${GREEN}✓${NC} $test_name"
        [ -n "$description" ] && echo -e "  ${CYAN}$description${NC}"
        passed=$((passed + 1))
    else
        echo -e "${RED}✗${NC} $test_name"
        [ -n "$description" ] && echo -e "  ${CYAN}$description${NC}"
        echo -e "  ${YELLOW}Expected: $expected_exit_code, Got: $actual_exit_code${NC}"
        failed=$((failed + 1))
    fi
}

echo -e "${YELLOW}━━━ Operator Precedence & Associativity ━━━${NC}\n"

run_test "syntax_precedence_01" 'import "ifj25" for Ifj
class Program {
static main() {
var result
result = 2 + 3 * 4
}
}' 0 "Multiplication before addition"

run_test "syntax_precedence_02" 'import "ifj25" for Ifj
class Program {
static main() {
var result
result = 10 - 5 - 2
}
}' 0 "Left-to-right subtraction"

run_test "syntax_precedence_03" 'import "ifj25" for Ifj
class Program {
static main() {
var result
result = 20 / 5 / 2
}
}' 0 "Left-to-right division"

run_test "syntax_precedence_04" 'import "ifj25" for Ifj
class Program {
static main() {
var result
result = 2 * 3 + 4 * 5
}
}' 0 "Multiple multiplications with addition"

run_test "syntax_precedence_05" 'import "ifj25" for Ifj
class Program {
static main() {
var result
result = (2 + 3) * 4
}
}' 0 "Parentheses override precedence"

run_test "syntax_precedence_06" 'import "ifj25" for Ifj
class Program {
static main() {
var result
result = ((2 + 3) * 4) - (5 / (1 + 1))
}
}' 0 "Complex nested parentheses"

echo -e "\n${YELLOW}━━━ Comparison Operators ━━━${NC}\n"

run_test "syntax_comparison_01" 'import "ifj25" for Ifj
class Program {
static main() {
var a
var b
a = 5
b = 3
if (a != b) {
} else {
}
}
}' 0 "Not equal operator"

run_test "syntax_comparison_02" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = 10
if (x < 20) {
} else {
}
}
}' 0 "Less than operator"

run_test "syntax_comparison_03" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = 10
if (x > 5) {
} else {
}
}
}' 0 "Greater than operator"

run_test "syntax_comparison_04" 'import "ifj25" for Ifj
class Program {
static main() {
var result
result = 5 < 10
}
}' 0 "Comparison result assignment"

run_test "syntax_comparison_05" 'import "ifj25" for Ifj
class Program {
static main() {
if ((5 + 3) > (2 * 3)) {
} else {
}
}
}' 0 "Comparison of expressions"

echo -e "\n${YELLOW}━━━ String Operations ━━━${NC}\n"

run_test "syntax_string_01" 'import "ifj25" for Ifj
class Program {
static main() {
var s1
var s2
var s3
s1 = "Hello"
s2 = "World"
s3 = s1 + s2
}
}' 0 "String variable concatenation"

run_test "syntax_string_02" 'import "ifj25" for Ifj
class Program {
static main() {
var msg
msg = "Line1" + "\n" + "Line2"
}
}' 0 "String concatenation with escape sequence"

run_test "syntax_string_03" 'import "ifj25" for Ifj
class Program {
static main() {
Ifj.write("Hello" + " " + "World")
}
}' 0 "Inline string concatenation in function call"

run_test "syntax_string_04" 'import "ifj25" for Ifj
class Program {
static main() {
var empty
empty = ""
}
}' 0 "Empty string assignment"

run_test "syntax_string_05" 'import "ifj25" for Ifj
class Program {
static main() {
var quote
quote = "She said \"Hello\""
}
}' 0 "Escaped quotes in string"

run_test "syntax_string_06" 'import "ifj25" for Ifj
class Program {
static main() {
var special
special = "Tab\there\nNew\rline\\"
}
}' 0 "Multiple escape sequences"

echo -e "\n${YELLOW}━━━ Function Call Variations ━━━${NC}\n"

run_test "syntax_funcall_01" 'import "ifj25" for Ifj
class Program {
static helper(a, b, c) {
return a + b + c
}
static main() {
var r
r = helper(1, 2, 3)
}
}' 0 "Function call with 3 arguments"

run_test "syntax_funcall_02" 'import "ifj25" for Ifj
class Program {
static getValue() {
return 42
}
static main() {
Ifj.write(getValue())
}
}' 0 "Function call as argument to another function"

run_test "syntax_funcall_03" 'import "ifj25" for Ifj
class Program {
static add(x, y) {
return x + y
}
static main() {
var result
result = add(5 + 3, 10 - 2)
}
}' 0 "Expressions as function arguments"

run_test "syntax_funcall_04" 'import "ifj25" for Ifj
class Program {
static calc(a, b) {
return a * b
}
static main() {
var x
x = 5
var y
y = 3
var z
z = calc(x, y)
}
}' 0 "Variables as function arguments"

run_test "syntax_funcall_05" 'import "ifj25" for Ifj
class Program {
static inner() {
return 10
}
static outer(n) {
return n * 2
}
static main() {
var result
result = outer(inner())
}
}' 0 "Nested function calls (inner result as outer argument)"

run_test "syntax_funcall_06" 'import "ifj25" for Ifj
class Program {
static test() {
test()
}
static main() {
}
}' 0 "Recursive function call (syntactically valid)"

echo -e "\n${YELLOW}━━━ Built-in Functions (Ifj.*) ━━━${NC}\n"

run_test "syntax_builtin_01" 'import "ifj25" for Ifj
class Program {
static main() {
var len
len = Ifj.length("test")
}
}' 0 "Ifj.length() function"

run_test "syntax_builtin_02" 'import "ifj25" for Ifj
class Program {
static main() {
var s
s = Ifj.str(42)
}
}' 0 "Ifj.str() function"

run_test "syntax_builtin_03" 'import "ifj25" for Ifj
class Program {
static main() {
var sub
sub = Ifj.substring("hello", 1, 3)
}
}' 0 "Ifj.substring() function"

run_test "syntax_builtin_04" 'import "ifj25" for Ifj
class Program {
static main() {
var c
c = Ifj.ord("A")
}
}' 3 "Ifj.ord() function (may be undefined)"

run_test "syntax_builtin_05" 'import "ifj25" for Ifj
class Program {
static main() {
var ch
ch = Ifj.chr(65)
}
}' 0 "Ifj.chr() function"

run_test "syntax_builtin_06" 'import "ifj25" for Ifj
class Program {
static main() {
Ifj.write("Line1\n")
Ifj.write("Line2\n")
Ifj.write("Line3\n")
}
}' 0 "Multiple Ifj.write() calls"

echo -e "\n${YELLOW}━━━ Complex Control Flow ━━━${NC}\n"

run_test "syntax_control_01" 'import "ifj25" for Ifj
class Program {
static main() {
var i
i = 0
while (i < 5) {
if (i == 3) {
} else {
}
i = i + 1
}
}
}' 0 "If inside while loop"

run_test "syntax_control_02" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = 10
if (x > 5) {
var y
y = 0
while (y < x) {
y = y + 1
}
} else {
}
}
}' 0 "While inside if block"

run_test "syntax_control_03" 'import "ifj25" for Ifj
class Program {
static main() {
var a
a = 0
while (a < 3) {
var b
b = 0
while (b < 3) {
if (a == b) {
} else {
}
b = b + 1
}
a = a + 1
}
}
}' 0 "Nested loops with if inside"

run_test "syntax_control_04" 'import "ifj25" for Ifj
class Program {
static main() {
if (1 == 1) {
if (2 == 2) {
if (3 == 3) {
} else {
}
} else {
if (4 == 4) {
} else {
}
}
} else {
}
}
}' 0 "Multiple nested if-else branches"

run_test "syntax_control_05" 'import "ifj25" for Ifj
class Program {
static main() {
var outer
outer = 0
while (outer < 2) {
if (outer == 0) {
var inner
inner = 0
while (inner < 2) {
inner = inner + 1
}
} else {
var inner
inner = 5
}
outer = outer + 1
}
}
}' 0 "Complex nesting with variable shadowing"

echo -e "\n${YELLOW}━━━ Return Statement Variations ━━━${NC}\n"

run_test "syntax_return_01" 'import "ifj25" for Ifj
class Program {
static add(a, b) {
var sum
sum = a + b
return sum
}
static main() {
}
}' 0 "Return local variable"

run_test "syntax_return_02" 'import "ifj25" for Ifj
class Program {
static calculate() {
return 5 + 3 * 2
}
static main() {
}
}' 0 "Return arithmetic expression"

run_test "syntax_return_03" 'import "ifj25" for Ifj
class Program {
static concat() {
return "Hello" + " " + "World"
}
static main() {
}
}' 0 "Return string concatenation"

run_test "syntax_return_04" 'import "ifj25" for Ifj
class Program {
static getValue(flag) {
if (flag == 1) {
return 100
} else {
return 200
}
}
static main() {
}
}' 0 "Return in both if branches"

run_test "syntax_return_05" 'import "ifj25" for Ifj
class Program {
static test() {
var i
i = 0
while (i < 10) {
if (i == 5) {
return i
} else {
}
i = i + 1
}
return null
}
static main() {
}
}' 0 "Return inside nested control structures"

echo -e "\n${YELLOW}━━━ Getter and Setter Patterns ━━━${NC}\n"

run_test "syntax_getter_01" 'import "ifj25" for Ifj
class Program {
static count {
if (__count) {
return __count
} else {
return 0
}
}
static main() {
}
}' 0 "Getter with if-else"

run_test "syntax_getter_02" 'import "ifj25" for Ifj
class Program {
static value {
return 42
}
static main() {
var x
x = value
}
}' 0 "Simple getter usage"

run_test "syntax_setter_01" 'import "ifj25" for Ifj
class Program {
static count=(val) {
__count = val
}
static main() {
count = 10
}
}' 0 "Simple setter usage"

run_test "syntax_getter_setter_01" 'import "ifj25" for Ifj
class Program {
static data {
return __data
}
static data=(val) {
__data = val
}
static main() {
data = 100
var x
x = data
}
}' 0 "Getter and setter pair"

run_test "syntax_getter_expr_01" 'import "ifj25" for Ifj
class Program {
static value {
return 10
}
static main() {
var result
result = value * 2 + 5
}
}' 0 "Getter in arithmetic expression"

echo -e "\n${YELLOW}━━━ Variable Scoping ━━━${NC}\n"

run_test "syntax_scope_01" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = 1
{
var y
y = 2
}
var z
z = 3
}
}' 0 "Variables in different block scopes"

run_test "syntax_scope_02" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = 1
if (x == 1) {
var x
x = 2
} else {
var x
x = 3
}
}
}' 0 "Variable shadowing in if-else"

run_test "syntax_scope_03" 'import "ifj25" for Ifj
class Program {
static main() {
var i
i = 0
while (i < 3) {
var j
j = i
i = i + 1
}
}
}' 0 "Variable declared in loop body"

run_test "syntax_scope_04" 'import "ifj25" for Ifj
class Program {
static helper() {
var local
local = 10
}
static main() {
var local
local = 20
}
}' 0 "Same variable name in different functions"

echo -e "\n${YELLOW}━━━ Null Handling ━━━${NC}\n"

run_test "syntax_null_01" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = null
}
}' 0 "Assign null to variable"

run_test "syntax_null_02" 'import "ifj25" for Ifj
class Program {
static getNull() {
return null
}
static main() {
var x
x = getNull()
}
}' 0 "Function returning null"

run_test "syntax_null_03" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = null
if (x == null) {
} else {
}
}
}' 0 "Comparing with null"

run_test "syntax_null_04" 'import "ifj25" for Ifj
class Program {
static process(val) {
if (val != null) {
return val
} else {
return 0
}
}
static main() {
}
}' 0 "Null check in function"

echo -e "\n${YELLOW}━━━ Complex Expressions ━━━${NC}\n"

run_test "syntax_expr_01" 'import "ifj25" for Ifj
class Program {
static main() {
var result
result = 1 + 2 * 3 - 4 / 2 + 5
}
}' 0 "Mixed arithmetic operators"

run_test "syntax_expr_02" 'import "ifj25" for Ifj
class Program {
static main() {
var result
result = (1 + 2) * (3 + 4) / (5 - 3)
}
}' 0 "Multiple parenthesized subexpressions"

run_test "syntax_expr_03" 'import "ifj25" for Ifj
class Program {
static main() {
var a
var b
var c
a = 5
b = 3
c = (a + b) * (a - b)
}
}' 0 "Variables in parenthesized expressions"

run_test "syntax_expr_04" 'import "ifj25" for Ifj
class Program {
static getValue() {
return 10
}
static main() {
var result
var val
val = getValue()
result = val + val * 2
}
}' 0 "Function calls stored in variables"

run_test "syntax_expr_05" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = 5
var y
y = ((x + 1) * 2 - 3) / (x - 2)
}
}' 0 "Deeply nested arithmetic"

echo -e "\n${YELLOW}━━━ Invalid Syntax Tests ━━━${NC}\n"

run_test "syntax_invalid_01" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = 
y = 5
}
}' 2 "Incomplete assignment"

run_test "syntax_invalid_02" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = (5 + 3
}
}' 2 "Unclosed parenthesis"

run_test "syntax_invalid_03" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = 5 + * 3
}
}' 2 "Consecutive operators"

run_test "syntax_invalid_04" 'import "ifj25" for Ifj
class Program {
static main() {
if (x == 5)
}
}' 2 "Missing block after if condition"

run_test "syntax_invalid_05" 'import "ifj25" for Ifj
class Program {
static main() {
while
}
}' 2 "While without condition"

run_test "syntax_invalid_06" 'import "ifj25" for Ifj
class Program {
static main() {
var
x = 5
}
}' 2 "Var without identifier"

run_test "syntax_invalid_07" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x =
}
}' 2 "Assignment without expression"

run_test "syntax_invalid_08" 'import "ifj25" for Ifj
class Program {
static (x) {
}
static main() {
}
}' 2 "Function without name"

run_test "syntax_invalid_09" 'import "ifj25" for Ifj
class Program {
static main() {
Ifj.write(
}
}' 2 "Unclosed function call"

run_test "syntax_invalid_10" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = 5 + (3 * 2
}
}' 2 "Missing closing parenthesis in expression"

echo -e "\n${YELLOW}━━━ Statement Ordering ━━━${NC}\n"

run_test "syntax_order_01" 'import "ifj25" for Ifj
class Program {
static main() {
var a
var b
var c
a = 1
b = 2
c = a + b
}
}' 0 "Sequential declarations then assignments"

run_test "syntax_order_02" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = 5
var y
y = x + 3
var z
z = y * 2
}
}' 0 "Interleaved declarations and assignments"

run_test "syntax_order_03" 'import "ifj25" for Ifj
class Program {
static helper() {
return 10
}
static helper2() {
return helper()
}
static main() {
}
}' 0 "Functions calling each other"

run_test "syntax_order_04" 'import "ifj25" for Ifj
class Program {
static value {
return 5
}
static value=(v) {
}
static main() {
}
}' 0 "Getter before setter"

run_test "syntax_order_05" 'import "ifj25" for Ifj
class Program {
static count=(v) {
}
static count {
return 0
}
static main() {
}
}' 0 "Setter before getter"

echo -e "\n${BLUE}=== Test Summary ===${NC}"
echo -e "Total tests: ${BLUE}$total${NC}"
echo -e "Passed: ${GREEN}$passed${NC}"
echo -e "Failed: ${RED}$failed${NC}"

if [ $failed -eq 0 ]; then
    echo -e "\n${GREEN}✅  All extended syntax tests passed!${NC}"
    rm -rf "$TEST_DIR"
    exit 0
else
    success_rate=$((passed * 100 / total))
    echo -e "\n${YELLOW}⚠️  Success rate: $success_rate%${NC}"
    echo -e "${YELLOW}Test files preserved in: $TEST_DIR${NC}"
    exit 1
fi
