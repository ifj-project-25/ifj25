#!/bin/bash

# Comprehensive Edge Case Test Suite for Syntactic Analyzer
# Based on IFJ25 Language Specification and Grammar
# Tests boundary conditions, corner cases, and unusual but valid/invalid constructs

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
TEST_DIR="test/edge_case_tests"
mkdir -p "$TEST_DIR"

echo -e "${BLUE}=== Comprehensive Edge Case Test Suite for Syntactic Analyzer ===${NC}"
echo -e "${CYAN}Based on IFJ25 Language Specification${NC}\n"

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
        echo -e "${GREEN}✓ PASSED${NC} - $test_name"
        if [ -n "$description" ]; then
            echo -e "  ${CYAN}→ $description${NC}"
        fi
        passed=$((passed + 1))
    else
        echo -e "${RED}✗ FAILED${NC} - $test_name"
        if [ -n "$description" ]; then
            echo -e "  ${CYAN}→ $description${NC}"
        fi
        echo -e "  ${YELLOW}Expected: $expected_exit_code, Got: $actual_exit_code${NC}"
        failed=$((failed + 1))
    fi
}

echo -e "${YELLOW}━━━ 1. PROLOG (Import Statement) Edge Cases ━━━${NC}\n"

# Test 1: Import with different whitespace
run_test "edge_import_spaces" 'import   "ifj25"   for   Ifj
class Program {
static main() {
}
}' 0 "Multiple spaces in import statement"

# Test 2: No newline after import
run_test "edge_import_no_newline" 'import "ifj25" for Ifj class Program {
static main() {
}
}' 2 "Missing newline after import"

# Test 3: Wrong module name
run_test "edge_import_wrong_module" 'import "ifj24" for Ifj
class Program {
static main() {
}
}' 2 "Wrong module name (not ifj25)"

# Test 4: Wrong alias
run_test "edge_import_wrong_alias" 'import "ifj25" for Ifj2
class Program {
static main() {
}
}' 2 "Wrong alias (not Ifj)"

# Test 5: Missing quotes
run_test "edge_import_no_quotes" 'import ifj25 for Ifj
class Program {
static main() {
}
}' 2 "Missing quotes around module name"

echo -e "\n${YELLOW}━━━ 2. CLASS Definition Edge Cases ━━━${NC}\n"

# Test 6: Wrong class name
run_test "edge_class_wrong_name" 'import "ifj25" for Ifj
class MyProgram {
static main() {
}
}' 2 "Class name must be Program"

# Test 7: No space before brace
run_test "edge_class_no_space" 'import "ifj25" for Ifj
class Program{
static main() {
}
}' 0 "No space before opening brace (valid)"

# Test 8: Multiple newlines before class
run_test "edge_class_multiple_newlines" 'import "ifj25" for Ifj


class Program {
static main() {
}
}' 0 "Multiple newlines after import"

# Test 9: Empty class (no main)
run_test "edge_class_empty_no_main" 'import "ifj25" for Ifj
class Program {
}' 10 "Empty class without main method"

# Test 10: Class with only getters/setters, no main
run_test "edge_class_only_getter_no_main" 'import "ifj25" for Ifj
class Program {
static value {
return 10
}
}' 10 "Class with getter but no main"

echo -e "\n${YELLOW}━━━ 3. Function/Method Definition Edge Cases ━━━${NC}\n"

# Test 11: Function with no parameters
run_test "edge_func_no_params" 'import "ifj25" for Ifj
class Program {
static helper() {
}
static main() {
}
}' 0 "Function with empty parameter list"

# Test 12: Function with multiple parameters
run_test "edge_func_many_params" 'import "ifj25" for Ifj
class Program {
static calc(a, b, c, d, e) {
return a
}
static main() {
}
}' 0 "Function with 5 parameters"

# Test 13: Function with trailing comma in parameters
run_test "edge_func_trailing_comma" 'import "ifj25" for Ifj
class Program {
static add(a, b,) {
return a
}
static main() {
}
}' 2 "Trailing comma in parameter list (invalid)"

# Test 14: Multiple functions
run_test "edge_multiple_functions" 'import "ifj25" for Ifj
class Program {
static func1() {
}
static func2() {
}
static func3() {
}
static main() {
}
}' 0 "Multiple function definitions"

# Test 15: Getter without braces
run_test "edge_getter_no_braces" 'import "ifj25" for Ifj
class Program {
static value
return 10
static main() {
}
}' 2 "Getter without body braces (invalid)"

# Test 16: Setter with wrong parameter syntax
run_test "edge_setter_no_parens" 'import "ifj25" for Ifj
class Program {
static value=val {
}
static main() {
}
}' 2 "Setter without parentheses around parameter"

# Test 17: Function name with underscore
run_test "edge_func_underscore_name" 'import "ifj25" for Ifj
class Program {
static my_function() {
}
static main() {
}
}' 0 "Function name with underscore"

# Test 18: Non-static method
run_test "edge_non_static_method" 'import "ifj25" for Ifj
class Program {
main() {
}
}' 2 "Non-static method (invalid)"

echo -e "\n${YELLOW}━━━ 4. Variable Declaration Edge Cases ━━━${NC}\n"

# Test 19: Variable declaration without initialization
run_test "edge_var_no_init" 'import "ifj25" for Ifj
class Program {
static main() {
var x
}
}' 0 "Variable declared but not initialized"

# Test 20: Multiple var declarations in sequence
run_test "edge_multiple_vars_sequence" 'import "ifj25" for Ifj
class Program {
static main() {
var a
var b
var c
var d
var e
}
}' 0 "Multiple consecutive var declarations"

# Test 21: Variable with same name as function
run_test "edge_var_same_as_func" 'import "ifj25" for Ifj
class Program {
static helper() {
}
static main() {
var helper
}
}' 0 "Variable name same as function name (different scopes)"

# Test 22: Variable starting with underscore
run_test "edge_var_underscore" 'import "ifj25" for Ifj
class Program {
static main() {
var _private
}
}' 0 "Variable starting with underscore"

# Test 23: Variable with number in name
run_test "edge_var_with_number" 'import "ifj25" for Ifj
class Program {
static main() {
var var1
var test123
}
}' 0 "Variable names with numbers"

# Test 24: Reserved-looking identifier
run_test "edge_var_reserved_like" 'import "ifj25" for Ifj
class Program {
static main() {
var whileVar
var ifVar
}
}' 0 "Variables with reserved-word-like names"

echo -e "\n${YELLOW}━━━ 5. Expression and Assignment Edge Cases ━━━${NC}\n"

# Test 25: Assignment chain (invalid in most languages)
run_test "edge_assignment_chain" 'import "ifj25" for Ifj
class Program {
static main() {
var a
var b
a = b = 5
}
}' 2 "Chained assignment (typically invalid)"

# Test 26: Empty expression in assignment
run_test "edge_assignment_empty" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = 
}
}' 2 "Empty expression in assignment"

# Test 27: Complex nested expressions
run_test "edge_complex_expr" 'import "ifj25" for Ifj
class Program {
static main() {
var result
result = ((5 + 3) * (10 - 2)) / ((4 + 1) - 2)
}
}' 0 "Deeply nested arithmetic expressions"

# Test 28: Expression with all arithmetic operators
run_test "edge_all_arithmetic" 'import "ifj25" for Ifj
class Program {
static main() {
var r
r = 1 + 2 - 3 * 4 / 5
}
}' 0 "Expression with all arithmetic operators"

# Test 29: String concatenation with multiple operators
run_test "edge_string_concat_multi" 'import "ifj25" for Ifj
class Program {
static main() {
var str
str = "a" + "b" + "c" + "d"
}
}' 0 "Multiple string concatenations"

# Test 30: Mixed string and number (should fail in semantic)
run_test "edge_mixed_types" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = "text" + 5
}
}' 6 "Mixed string and number types (semantic error)"

echo -e "\n${YELLOW}━━━ 6. Control Flow (IF) Edge Cases ━━━${NC}\n"

# Test 31: If without else
run_test "edge_if_no_else" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = 5
if (x == 5) {
}
}
}' 2 "If without else clause (grammar requires else)"

# Test 32: If with empty then and else blocks
run_test "edge_if_empty_blocks" 'import "ifj25" for Ifj
class Program {
static main() {
if (1 == 1) {
} else {
}
}
}' 0 "If with empty then and else blocks"

# Test 33: Deeply nested if-else
run_test "edge_nested_if_deep" 'import "ifj25" for Ifj
class Program {
static main() {
var a
a = 1
if (a == 1) {
if (a == 1) {
if (a == 1) {
if (a == 1) {
} else {
}
} else {
}
} else {
}
} else {
}
}
}' 0 "Deeply nested if-else (4 levels)"

# Test 34: If with complex condition
run_test "edge_if_complex_cond" 'import "ifj25" for Ifj
class Program {
static main() {
if ((5 + 3) == (4 * 2)) {
} else {
}
}
}' 0 "If with arithmetic expression as condition"

# Test 35: If with null condition
run_test "edge_if_null_cond" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = null
if (x == null) {
} else {
}
}
}' 0 "If comparing with null"

echo -e "\n${YELLOW}━━━ 7. Control Flow (WHILE) Edge Cases ━━━${NC}\n"

# Test 36: While with empty body
run_test "edge_while_empty" 'import "ifj25" for Ifj
class Program {
static main() {
while (1 == 2) {
}
}
}' 0 "While loop with empty body"

# Test 37: Infinite loop structure
run_test "edge_while_infinite" 'import "ifj25" for Ifj
class Program {
static main() {
while (1 == 1) {
var x
x = 5
}
}
}' 0 "While loop that would be infinite"

# Test 38: Nested while loops (3 levels)
run_test "edge_while_triple_nested" 'import "ifj25" for Ifj
class Program {
static main() {
while (1 == 1) {
while (2 == 2) {
while (3 == 3) {
}
}
}
}
}' 0 "Triple nested while loops"

# Test 39: While with complex condition
run_test "edge_while_complex_cond" 'import "ifj25" for Ifj
class Program {
static main() {
var i
i = 0
while (i + 5 < 10) {
i = i + 1
}
}
}' 0 "While with arithmetic expression in condition"

echo -e "\n${YELLOW}━━━ 8. Function Calls Edge Cases ━━━${NC}\n"

# Test 40: Ifj built-in functions
run_test "edge_builtin_write" 'import "ifj25" for Ifj
class Program {
static main() {
Ifj.write("test")
}
}' 0 "Built-in Ifj.write() function"

# Test 41: Multiple Ifj calls
run_test "edge_multiple_ifj_calls" 'import "ifj25" for Ifj
class Program {
static main() {
Ifj.write("a")
Ifj.write("b")
Ifj.write("c")
}
}' 0 "Multiple consecutive Ifj calls"

# Test 42: Function call with no arguments
run_test "edge_call_no_args" 'import "ifj25" for Ifj
class Program {
static helper() {
}
static main() {
helper()
}
}' 0 "Function call with empty argument list"

# Test 43: Function call with multiple arguments
run_test "edge_call_many_args" 'import "ifj25" for Ifj
class Program {
static sum(a, b, c, d) {
return a
}
static main() {
var r
r = sum(1, 2, 3, 4)
}
}' 0 "Function call with 4 arguments"

# Test 44: Nested function calls
run_test "edge_nested_calls" 'import "ifj25" for Ifj
class Program {
static inner() {
return 5
}
static outer(x) {
return x
}
static main() {
var r
r = outer(inner())
}
}' 0 "Nested function calls"

# Test 45: Function call with expression as argument
run_test "edge_call_expr_arg" 'import "ifj25" for Ifj
class Program {
static func(x) {
return x
}
static main() {
var r
r = func(5 + 3)
}
}' 0 "Function call with expression as argument"

# Test 46: Trailing comma in function call
run_test "edge_call_trailing_comma" 'import "ifj25" for Ifj
class Program {
static func(a, b) {
return a
}
static main() {
var r
r = func(1, 2,)
}
}' 2 "Function call with trailing comma (invalid)"

echo -e "\n${YELLOW}━━━ 9. Return Statement Edge Cases ━━━${NC}\n"

# Test 47: Return null
run_test "edge_return_null" 'import "ifj25" for Ifj
class Program {
static getNone() {
return null
}
static main() {
}
}' 0 "Return null value"

# Test 48: Return expression
run_test "edge_return_expr" 'import "ifj25" for Ifj
class Program {
static calc() {
return 5 + 3 * 2
}
static main() {
}
}' 0 "Return arithmetic expression"

# Test 49: Return variable
run_test "edge_return_var" 'import "ifj25" for Ifj
class Program {
static getValue() {
var x
x = 10
return x
}
static main() {
}
}' 0 "Return variable"

# Test 50: Multiple returns in function (only first reachable)
run_test "edge_multiple_returns" 'import "ifj25" for Ifj
class Program {
static test() {
return 1
return 2
return 3
}
static main() {
}
}' 0 "Multiple return statements"

# Test 51: Return in main
run_test "edge_return_in_main" 'import "ifj25" for Ifj
class Program {
static main() {
return null
}
}' 0 "Return statement in main"

echo -e "\n${YELLOW}━━━ 10. Literal Edge Cases ━━━${NC}\n"

# Test 52: Empty string literal
run_test "edge_literal_empty_string" 'import "ifj25" for Ifj
class Program {
static main() {
var s
s = ""
}
}' 0 "Empty string literal"

# Test 53: String with escape sequences
run_test "edge_literal_escape_seq" 'import "ifj25" for Ifj
class Program {
static main() {
var s
s = "Line 1\nLine 2\tTabbed"
}
}' 0 "String with escape sequences"

# Test 54: Zero value
run_test "edge_literal_zero" 'import "ifj25" for Ifj
class Program {
static main() {
var z
z = 0
}
}' 0 "Zero as literal"

# Test 55: Negative number
run_test "edge_literal_negative" 'import "ifj25" for Ifj
class Program {
static main() {
var n
n = -5
}
}' 0 "Negative number literal"

# Test 56: Floating point number
run_test "edge_literal_float" 'import "ifj25" for Ifj
class Program {
static main() {
var f
f = 3.14
}
}' 0 "Floating point literal"

# Test 57: Very long string
run_test "edge_literal_long_string" 'import "ifj25" for Ifj
class Program {
static main() {
var s
s = "This is a very long string that contains many characters and should still be parsed correctly by the compiler"
}
}' 0 "Very long string literal"

echo -e "\n${YELLOW}━━━ 11. Block and Scope Edge Cases ━━━${NC}\n"

# Test 58: Empty block in function
run_test "edge_empty_block_func" 'import "ifj25" for Ifj
class Program {
static empty() {
}
static main() {
}
}' 0 "Function with empty block"

# Test 59: Nested blocks
run_test "edge_nested_blocks" 'import "ifj25" for Ifj
class Program {
static main() {
{
{
{
var x
x = 1
}
}
}
}
}' 0 "Nested empty blocks"

# Test 60: Variable shadowing in nested scope
run_test "edge_var_shadowing" 'import "ifj25" for Ifj
class Program {
static main() {
var x
x = 1
{
var x
x = 2
}
}
}' 0 "Variable shadowing in nested block"

# Test 61: Block as statement
run_test "edge_block_as_statement" 'import "ifj25" for Ifj
class Program {
static main() {
var a
a = 5
{
var b
b = 10
}
var c
c = 15
}
}' 0 "Block as statement in function"

echo -e "\n${YELLOW}━━━ 12. Whitespace and Formatting Edge Cases ━━━${NC}\n"

# Test 62: No whitespace between tokens (where allowed)
run_test "edge_no_whitespace" 'import "ifj25" for Ifj
class Program{static main(){var x
x=5}}' 0 "Minimal whitespace (compact format)"

# Test 63: Excessive whitespace
run_test "edge_excessive_whitespace" 'import    "ifj25"    for    Ifj
class    Program    {
    static    main    (    )    {
        var    x
        x    =    5
    }
}' 0 "Excessive whitespace between tokens"

# Test 64: Mixed tabs and spaces
run_test "edge_tabs_spaces" 'import "ifj25" for Ifj
class Program {
	static main() {
		var x
		x = 5
	}
}' 0 "Mixed tabs and spaces for indentation"

# Test 65: Windows line endings (CRLF)
run_test "edge_crlf_line_endings" $'import "ifj25" for Ifj\r\nclass Program {\r\nstatic main() {\r\n}\r\n}' 0 "Windows-style line endings (CRLF)"

echo -e "\n${YELLOW}━━━ 13. Comment Edge Cases ━━━${NC}\n"

# Test 66: Single-line comment
run_test "edge_comment_single_line" 'import "ifj25" for Ifj
// This is a comment
class Program {
static main() {
// Another comment
}
}' 0 "Single-line comments"

# Test 67: Block comment
run_test "edge_comment_block" 'import "ifj25" for Ifj
/* This is a
   block comment */
class Program {
static main() {
/* Another block comment */
}
}' 0 "Block comments"

# Test 68: Comment at end of line
run_test "edge_comment_eol" 'import "ifj25" for Ifj
class Program { // comment here
static main() { // and here
var x // and here too
x = 5 // everywhere
}
}' 0 "Comments at end of lines"

# Test 69: Nested block comments (if supported)
run_test "edge_comment_nested" 'import "ifj25" for Ifj
/* Outer comment
   /* Inner comment */
   Still outer
*/
class Program {
static main() {
}
}' 0 "Nested block comments"

echo -e "\n${YELLOW}━━━ 14. Syntax Boundary Cases ━━━${NC}\n"

# Test 70: Very long identifier
run_test "edge_long_identifier" 'import "ifj25" for Ifj
class Program {
static thisIsAVeryLongFunctionNameThatShouldStillBeValidAccordingToTheLanguageSpecification() {
}
static main() {
}
}' 0 "Very long identifier name"

# Test 71: Single character identifier
run_test "edge_single_char_id" 'import "ifj25" for Ifj
class Program {
static a() {
}
static main() {
var x
var y
var z
}
}' 0 "Single character identifiers"

# Test 72: Maximum nesting depth
run_test "edge_max_nesting" 'import "ifj25" for Ifj
class Program {
static main() {
{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{
var x
}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}
}
}' 0 "Very deep nesting of blocks"

# Test 73: Assignment to function call result
run_test "edge_assign_call_result" 'import "ifj25" for Ifj
class Program {
static getValue() {
return 42
}
static main() {
var x
x = getValue()
}
}' 0 "Assignment from function call return value"

# Test 74: Getter usage in expression
run_test "edge_getter_in_expr" 'import "ifj25" for Ifj
class Program {
static value {
return 10
}
static main() {
var x
x = value + 5
}
}' 0 "Getter used in arithmetic expression"

# Test 75: Setter usage
run_test "edge_setter_usage" 'import "ifj25" for Ifj
class Program {
static value=(v) {
__value = v
}
static main() {
value = 100
}
}' 0 "Setter method usage"

echo -e "\n${YELLOW}━━━ 15. Semantic Errors (Should Pass Syntax) ━━━${NC}\n"

# Test 76: Undefined function call (semantic error)
run_test "edge_undefined_func" 'import "ifj25" for Ifj
class Program {
static main() {
undefinedFunction()
}
}' 3 "Undefined function call (semantic error)"

# Test 77: Wrong number of arguments (semantic error)
run_test "edge_wrong_arg_count" 'import "ifj25" for Ifj
class Program {
static func(a, b) {
return a
}
static main() {
func(1)
}
}' 5 "Wrong number of arguments (semantic error)"

# Test 78: Undefined variable usage (semantic error)
run_test "edge_undefined_var" 'import "ifj25" for Ifj
class Program {
static main() {
x = 5
}
}' 3 "Undefined variable usage (semantic error)"

# Test 79: Variable redeclaration (semantic error)
run_test "edge_var_redeclaration" 'import "ifj25" for Ifj
class Program {
static main() {
var x
var x
}
}' 4 "Variable redeclaration (semantic error)"

# Test 80: Function redefinition (semantic error)
run_test "edge_func_redefine" 'import "ifj25" for Ifj
class Program {
static func() {
}
static func() {
}
static main() {
}
}' 4 "Function redefinition (semantic error)"

echo -e "\n${BLUE}=== Test Summary ===${NC}"
echo -e "Total tests: ${BLUE}$total${NC}"
echo -e "Passed: ${GREEN}$passed${NC}"
echo -e "Failed: ${RED}$failed${NC}"

if [ $failed -eq 0 ]; then
    echo -e "\n${GREEN}✅  All edge case tests passed!${NC}"
    rm -rf "$TEST_DIR"
    exit 0
else
    success_rate=$((passed * 100 / total))
    echo -e "\n${YELLOW}⚠️  Success rate: $success_rate%${NC}"
    echo -e "${YELLOW}Test files preserved in: $TEST_DIR${NC}"
    exit 1
fi
