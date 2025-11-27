#!/bin/bash

# Syntax Error Detection Test Suite
# Focuses specifically on invalid syntax patterns that should be rejected
# Tests the parser's ability to catch malformed constructs

# Color codes
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

# Test counters
total=0
passed=0
failed=0

# Create temporary test directory
TEST_DIR="test/syntax_error_tests"
mkdir -p "$TEST_DIR"

echo -e "${BLUE}=== Syntax Error Detection Test Suite ===${NC}\n"

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

echo -e "${YELLOW}━━━ Missing Tokens ━━━${NC}\n"

run_test "err_missing_import" 'class Program {
static main() {
}
}' 2 "Missing import statement"

run_test "err_missing_class_keyword" 'import "ifj25" for Ifj
Program {
static main() {
}
}' 2 "Missing 'class' keyword"

run_test "err_missing_static" 'import "ifj25" for Ifj
class Program {
main() {
}
}' 2 "Missing 'static' keyword in main"

run_test "err_missing_open_brace" 'import "ifj25" for Ifj
class Program
static main() {
}
}' 2 "Missing opening brace after class name"

run_test "err_missing_close_brace" 'import "ifj25" for Ifj
class Program {
static main() {
}' 2 "Missing closing brace for class"

run_test "err_missing_func_open_paren" 'import "ifj25" for Ifj
class Program {
static main) {
}
}' 2 "Missing opening parenthesis in function"

run_test "err_missing_func_close_paren" 'import "ifj25" for Ifj
class Program {
static main( {
}
}' 2 "Missing closing parenthesis in function"

run_test "err_missing_block_open" 'import "ifj25" for Ifj
class Program {
static main()
}
}' 2 "Missing opening brace for function body"

run_test "err_missing_block_close" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = 5
}' 2 "Missing closing brace for function body"

echo -e "\n${YELLOW}━━━ Malformed Statements ━━━${NC}\n"

run_test "err_var_no_id" 'import "ifj25" for Ifj
class Program {
static main() {
var
}
}' 2 "var keyword without identifier"

run_test "err_var_equals" 'import "ifj25" for Ifj
class Program {
static main() {
var x =
}
}' 2 "var with equals but no value"

run_test "err_assign_no_expr" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x =
}
}' 2 "Assignment without expression"

run_test "err_assign_incomplete" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = 5 +
}
}' 2 "Incomplete arithmetic expression"

run_test "err_two_operators" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = 5 + * 3
}
}' 2 "Two consecutive operators"

run_test "err_operator_at_end" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = 5 + 3 *
}
}' 2 "Operator at end of expression"

echo -e "\n${YELLOW}━━━ Malformed Control Structures ━━━${NC}\n"

run_test "err_if_no_condition" 'import "ifj25" for Ifj
class Program {
static main() {
if {
} else {
}
}
}' 2 "if without condition parentheses"

run_test "err_if_no_open_paren" 'import "ifj25" for Ifj
class Program {
static main() {
if 1 == 1) {
} else {
}
}
}' 2 "if missing opening parenthesis"

run_test "err_if_no_close_paren" 'import "ifj25" for Ifj
class Program {
static main() {
if (1 == 1 {
} else {
}
}
}' 2 "if missing closing parenthesis"

run_test "err_if_no_then_block" 'import "ifj25" for Ifj
class Program {
static main() {
if (1 == 1)
else {
}
}
}' 2 "if without then block"

run_test "err_if_no_else_block" 'import "ifj25" for Ifj
class Program {
static main() {
if (1 == 1) {
} else
}
}' 2 "if with else but no block"

run_test "err_while_no_condition" 'import "ifj25" for Ifj
class Program {
static main() {
while {
}
}
}' 2 "while without condition"

run_test "err_while_no_open_paren" 'import "ifj25" for Ifj
class Program {
static main() {
while 1 == 1) {
}
}
}' 2 "while missing opening parenthesis"

run_test "err_while_no_close_paren" 'import "ifj25" for Ifj
class Program {
static main() {
while (1 == 1 {
}
}
}' 2 "while missing closing parenthesis"

run_test "err_while_no_body" 'import "ifj25" for Ifj
class Program {
static main() {
while (1 == 1)
}
}' 2 "while without body block"

echo -e "\n${YELLOW}━━━ Malformed Function Calls ━━━${NC}\n"

run_test "err_call_no_open_paren" 'import "ifj25" for Ifj
class Program {
static helper() {
}
static main() {
helper)
}
}' 2 "Function call missing opening parenthesis"

run_test "err_call_no_close_paren" 'import "ifj25" for Ifj
class Program {
static helper() {
}
static main() {
helper(
}
}' 2 "Function call missing closing parenthesis"

run_test "err_call_trailing_comma" 'import "ifj25" for Ifj
class Program {
static func(a, b) {
}
static main() {
func(1, 2,)
}
}' 2 "Function call with trailing comma"

run_test "err_call_double_comma" 'import "ifj25" for Ifj
class Program {
static func(a, b, c) {
}
static main() {
func(1,, 3)
}
}' 2 "Function call with double comma"

run_test "err_call_missing_arg" 'import "ifj25" for Ifj
class Program {
static func(a, b) {
}
static main() {
func(1,)
}
}' 2 "Function call with missing argument after comma"

echo -e "\n${YELLOW}━━━ Malformed Function Definitions ━━━${NC}\n"

run_test "err_func_no_name" 'import "ifj25" for Ifj
class Program {
static () {
}
static main() {
}
}' 2 "Function without name"

run_test "err_func_no_params_parens" 'import "ifj25" for Ifj
class Program {
static helper {
}
static main() {
}
}' 0 "Function without parameter parentheses (valid getter syntax)"

run_test "err_param_trailing_comma" 'import "ifj25" for Ifj
class Program {
static func(a, b,) {
}
static main() {
}
}' 2 "Parameter list with trailing comma"

run_test "err_param_double_comma" 'import "ifj25" for Ifj
class Program {
static func(a,, b) {
}
static main() {
}
}' 2 "Parameter list with double comma"

run_test "err_param_no_id" 'import "ifj25" for Ifj
class Program {
static func(a, , c) {
}
static main() {
}
}' 2 "Parameter list with missing identifier"

echo -e "\n${YELLOW}━━━ Malformed Expressions ━━━${NC}\n"

run_test "err_expr_unclosed_paren" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = (5 + 3
}
}' 2 "Unclosed parenthesis in expression"

run_test "err_expr_extra_paren" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = 5 + 3)
}
}' 2 "Extra closing parenthesis"

run_test "err_expr_empty_parens" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = ()
}
}' 2 "Empty parentheses in expression"

run_test "err_expr_operator_only" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = +
}
}' 2 "Only operator in expression"

run_test "err_expr_starting_operator" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = * 5
}
}' 2 "Expression starting with binary operator"

echo -e "\n${YELLOW}━━━ Malformed String Literals ━━━${NC}\n"

run_test "err_string_unclosed" 'import "ifj25" for Ifj
class Program {
static main() {
var s
s = "unclosed
}
}' 1 "Unclosed string literal (lexical error)"

run_test "err_string_single_quote" 'import "ifj25" for Ifj
class Program {
static main() {
var s
s = '"'"'single'"'"'
}
}' 1 "Single quotes instead of double (lexical error)"

run_test "err_string_escape_invalid" 'import "ifj25" for Ifj
class Program {
static main() {
var s
s = "invalid\xescape"
}
}' 1 "Invalid escape sequence (lexical error)"

echo -e "\n${YELLOW}━━━ Malformed Getter/Setter ━━━${NC}\n"

run_test "err_getter_with_params" 'import "ifj25" for Ifj
class Program {
static value(x) {
return 10
}
static main() {
}
}' 0 "Getter with parameters (becomes regular function)"

run_test "err_setter_no_param" 'import "ifj25" for Ifj
class Program {
static value=() {
}
static main() {
}
}' 2 "Setter without parameter"

run_test "err_setter_multiple_params" 'import "ifj25" for Ifj
class Program {
static value=(a, b) {
}
static main() {
}
}' 2 "Setter with multiple parameters"

run_test "err_setter_no_parens" 'import "ifj25" for Ifj
class Program {
static value=val {
}
static main() {
}
}' 2 "Setter without parentheses around parameter"

echo -e "\n${YELLOW}━━━ Multiple Statement Errors ━━━${NC}\n"

run_test "err_multiple_vars_one_line" 'import "ifj25" for Ifj
class Program {
static main() {
var x var y
}
}' 2 "Two var declarations without separator"

run_test "err_statement_no_separator" 'import "ifj25" for Ifj
class Program {
static main() {
var x x = 5
}
}' 2 "Statements without proper separator"

run_test "err_incomplete_then_statement" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = 5
var y
}
}' 0 "Multiple statements separated properly (valid)"

echo -e "\n${YELLOW}━━━ Misplaced Keywords ━━━${NC}\n"

run_test "err_static_inside_func" 'import "ifj25" for Ifj
class Program {
static main() {
static var x
}
}' 2 "static keyword inside function body"

run_test "err_class_inside_func" 'import "ifj25" for Ifj
class Program {
static main() {
class Inner {
}
}
}' 2 "class keyword inside function"

run_test "err_import_inside_class" 'import "ifj25" for Ifj
class Program {
import "something"
static main() {
}
}' 2 "import statement inside class"

echo -e "\n${YELLOW}━━━ Wrong Token Types ━━━${NC}\n"

run_test "err_number_as_id" 'import "ifj25" for Ifj
class Program {
static main() {
var 123
}
}' 2 "Number used as identifier"

run_test "err_keyword_as_id" 'import "ifj25" for Ifj
class Program {
static main() {
var while
}
}' 2 "Keyword used as variable name"

run_test "err_keyword_as_func" 'import "ifj25" for Ifj
class Program {
static if() {
}
static main() {
}
}' 2 "Keyword used as function name"

echo -e "\n${BLUE}=== Test Summary ===${NC}"
echo -e "Total tests: ${BLUE}$total${NC}"
echo -e "Passed: ${GREEN}$passed${NC}"
echo -e "Failed: ${RED}$failed${NC}"

if [ $failed -eq 0 ]; then
    echo -e "\n${GREEN}✅  All syntax error detection tests passed!${NC}"
    rm -rf "$TEST_DIR"
    exit 0
else
    success_rate=$((passed * 100 / total))
    echo -e "\n${YELLOW}⚠️  Success rate: $success_rate%${NC}"
    echo -e "${YELLOW}Test files preserved in: $TEST_DIR${NC}"
    exit 1
fi
