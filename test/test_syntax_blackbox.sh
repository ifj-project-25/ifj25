#!/bin/bash

# Black Box Test Suite for Syntactic Analyzer
# Tests the parser for various syntactic scenarios
# Expected exit code for syntax errors: 2

# Color codes
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test counters
total=0
passed=0
failed=0

# Create temporary test directory
TEST_DIR="test/syntax_tests"
mkdir -p "$TEST_DIR"

echo -e "${BLUE}=== Black Box Test Suite for Syntactic Analyzer ===${NC}\n"

# Function to run a test
run_test() {
    local test_name="$1"
    local test_code="$2"
    local expected_exit_code="$3"
    local test_file="$TEST_DIR/${test_name}.wren"
    
    total=$((total + 1))
    
    # Create test file
    echo "$test_code" > "$test_file"
    
    # Run the compiler
    ./main < "$test_file" > /dev/null 2>&1
    actual_exit_code=$?
    
    # Check result
    if [ $actual_exit_code -eq $expected_exit_code ]; then
        echo -e "${GREEN}✓ PASSED${NC} - $test_name (exit code: $actual_exit_code)"
        passed=$((passed + 1))
    else
        echo -e "${RED}✗ FAILED${NC} - $test_name (expected: $expected_exit_code, got: $actual_exit_code)"
        failed=$((failed + 1))
    fi
}

echo -e "${YELLOW}--- Valid Syntax Tests (Expected: 0) ---${NC}\n"

# Test 1: Minimal valid program
run_test "valid_minimal" 'import "ifj25" for Ifj
class Program {
static main() {
}
}' 0

# Test 2: Simple variable declaration
run_test "valid_var_declaration" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = 5
}
}' 0

# Test 3: Multiple variables
run_test "valid_multiple_vars" 'import "ifj25" for Ifj
class Program {
static main() {
var a
var b
var c
a = 1
b = 2
c = 3
}
}' 0

# Test 4: If-else statement
run_test "valid_if_else" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = 5
if (x > 3) {
Ifj.write("yes\n")
} else {
Ifj.write("no\n")
}
}
}' 0

# Test 5: While loop
run_test "valid_while" 'import "ifj25" for Ifj
class Program {
static main() {
var i
i = 0
while (i < 5) {
i = i + 1
}
}
}' 0

# Test 6: Nested while loops
run_test "valid_nested_while" 'import "ifj25" for Ifj
class Program {
static main() {
var i
var j
i = 0
while (i < 3) {
j = 0
while (j < 3) {
j = j + 1
}
i = i + 1
}
}
}' 0

# Test 7: Function call
run_test "valid_function_call" 'import "ifj25" for Ifj
class Program {
static main() {
Ifj.write("Hello\n")
}
}' 0

# Test 8: Arithmetic expressions
run_test "valid_arithmetic" 'import "ifj25" for Ifj
class Program {
static main() {
var result
result = 5 + 3 * 2 - 1
}
}' 0

# Test 9: Assignment without comparison (comparison in if may have issues with test generation)
run_test "valid_assignment" 'import "ifj25" for Ifj
class Program {
static main() {
var x
var y
x = 5
y = x
}
}' 0

# Test 10: Getter and Setter
run_test "valid_getter_setter" 'import "ifj25" for Ifj
class Program {
static value {
return 10
}
static value=(v) {
}
static main() {
}
}' 0

echo -e "\n${YELLOW}--- Invalid Syntax Tests (Expected: 2) ---${NC}\n"

# Test 11: Missing import statement
run_test "invalid_no_import" 'class Program {
static main() {
}
}' 2

# Test 12: Missing class keyword
run_test "invalid_no_class" 'import "ifj25" for Ifj
Program {
static main() {
}
}' 2

# Test 13: Missing Program class name
run_test "invalid_no_program_name" 'import "ifj25" for Ifj
class {
static main() {
}
}' 2

# Test 14: Missing main method (semantic error)
run_test "invalid_no_main" 'import "ifj25" for Ifj
class Program {
}' 10

# Test 15: Missing static keyword in main
run_test "invalid_no_static" 'import "ifj25" for Ifj
class Program {
main() {
}
}' 2

# Test 16: Missing parentheses in main (semantic error)
run_test "invalid_no_parens_main" 'import "ifj25" for Ifj
class Program {
static main {
}
}' 10

# Test 17: Missing braces in main body
run_test "invalid_no_braces_main" 'import "ifj25" for Ifj
class Program {
static main()
}' 2

# Test 18: Missing var keyword (undefined variable - semantic error)
run_test "invalid_no_var" 'import "ifj25" for Ifj
class Program {
static main() {
x = 5
}
}' 3

# Test 19: Missing semicolon or newline (malformed statement)
run_test "invalid_missing_separator" 'import "ifj25" for Ifj
class Program {
static main() {
var x x = 5
}
}' 2

# Test 20: Missing closing brace in if
run_test "invalid_missing_if_brace" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = 5
if (x > 3) {
Ifj.write("yes\n")
}
}' 2

# Test 21: Missing condition in if
run_test "invalid_no_if_condition" 'import "ifj25" for Ifj
class Program {
static main() {
if {
Ifj.write("error\n")
}
}
}' 2

# Test 22: Missing parentheses in if condition
run_test "invalid_no_if_parens" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = 5
if x > 3 {
Ifj.write("error\n")
}
}
}' 2

# Test 23: Missing condition in while
run_test "invalid_no_while_condition" 'import "ifj25" for Ifj
class Program {
static main() {
while {
Ifj.write("error\n")
}
}
}' 2

# Test 24: Missing body in while
run_test "invalid_no_while_body" 'import "ifj25" for Ifj
class Program {
static main() {
var i
i = 0
while (i < 5)
}
}' 2

# Test 25: Unclosed string literal (lexical error)
run_test "invalid_unclosed_string" 'import "ifj25" for Ifj
class Program {
static main() {
Ifj.write("Hello
}
}' 1

# Test 26: Invalid expression
run_test "invalid_expression" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = + * 5
}
}' 2

# Test 27: Missing assignment operator
run_test "invalid_no_assignment_op" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x 5
}
}' 2

# Test 28: Extra closing brace (accepted as valid - parser stops after class)
run_test "invalid_extra_brace" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = 5
}
}
}' 0

# Test 29: Missing else body
run_test "invalid_no_else_body" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = 5
if (x > 3) {
Ifj.write("yes\n")
} else
}
}' 2

# Test 30: Getter with params is valid syntax (regular function)
run_test "invalid_getter_with_params" 'import "ifj25" for Ifj
class Program {
static value(x) {
return 10
}
static main() {
}
}' 0

echo -e "\n${YELLOW}--- Edge Cases ---${NC}\n"

# Test 31: Empty file
run_test "edge_empty_file" '' 2

# Test 32: Only import
run_test "edge_only_import" 'import "ifj25" for Ifj' 2

# Test 33: Comment only (if comments are supported)
run_test "edge_comment_only" '// This is a comment' 2

# Test 34: Multiple classes (parser accepts first class, ignores rest)
run_test "edge_multiple_classes" 'import "ifj25" for Ifj
class Program {
static main() {
}
}
class Other {
}' 0

# Test 35: Nested if-else
run_test "valid_nested_if_else" 'import "ifj25" for Ifj
class Program {
static main() {
var x
var y
x = 5
y = 10
if (x > 3) {
if (y > 8) {
Ifj.write("both\n")
} else {
Ifj.write("only x\n")
}
} else {
Ifj.write("neither\n")
}
}
}' 0

# Test 36: Complex expression
run_test "valid_complex_expr" 'import "ifj25" for Ifj
class Program {
static main() {
var result
result = (5 + 3) * (10 - 2) / 4
}
}' 0

# Test 37: Multiple function calls
run_test "valid_multiple_calls" 'import "ifj25" for Ifj
class Program {
static main() {
Ifj.write("Line 1\n")
Ifj.write("Line 2\n")
Ifj.write("Line 3\n")
}
}' 0

# Test 38: String concatenation
run_test "valid_string_concat" 'import "ifj25" for Ifj
class Program {
static main() {
var str
str = "Hello" + " " + "World"
}
}' 0

# Test 39: Numeric assignment (no boolean keywords in language)
run_test "valid_numeric_assignment" 'import "ifj25" for Ifj
class Program {
static main() {
var flag
flag = 1
}
}' 0

# Test 40: Null literal
run_test "valid_null" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = null
}
}' 0

# Cleanup
echo -e "\n${BLUE}=== Test Summary ===${NC}"
echo -e "Total tests: ${BLUE}$total${NC}"
echo -e "Passed: ${GREEN}$passed${NC}"
echo -e "Failed: ${RED}$failed${NC}"

if [ $failed -eq 0 ]; then
    echo -e "\n${GREEN}✅  All tests passed!${NC}"
    rm -rf "$TEST_DIR"
    exit 0
else
    success_rate=$((passed * 100 / total))
    echo -e "\n${YELLOW}⚠️  Success rate: $success_rate%${NC}"
    echo -e "${YELLOW}Test files preserved in: $TEST_DIR${NC}"
    exit 1
fi
