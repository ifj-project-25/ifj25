# Black Box Tests for Syntactic Analyzer

## Overview

This test suite provides comprehensive black box testing for the syntactic analyzer (parser) component of the IFJ25 compiler. The tests verify that the parser correctly accepts valid syntax and rejects invalid syntax according to the language specification.

## Test Script

- **File**: `test/test_syntax_blackbox.sh`
- **Purpose**: Automated testing of parser syntax validation
- **Expected Exit Codes**:
  - `0` - Valid syntax (successful parsing)
  - `2` - Syntax error detected

## Running the Tests

### Prerequisites

1. Build the compiler first:

   ```bash
   make
   ```

2. Run the test suite:
   ```bash
   ./test/test_syntax_blackbox.sh
   ```

### Output

The script provides colored output:

- 🟢 **Green** - Test passed
- 🔴 **Red** - Test failed
- 🔵 **Blue** - Summary information
- 🟡 **Yellow** - Section headers

## Test Categories

### 1. Valid Syntax Tests (10 tests)

These tests verify that the parser accepts syntactically correct programs:

- **valid_minimal** - Minimal valid program structure
- **valid_var_declaration** - Simple variable declaration and assignment
- **valid_multiple_vars** - Multiple variable declarations
- **valid_if_else** - If-else statement
- **valid_while** - While loop
- **valid_nested_while** - Nested while loops
- **valid_function_call** - Built-in function calls
- **valid_arithmetic** - Arithmetic expressions
- **valid_comparison** - Comparison operators
- **valid_getter_setter** - Getter and setter methods

### 2. Invalid Syntax Tests (20 tests)

These tests verify that the parser correctly rejects syntactically incorrect programs:

- **invalid_no_import** - Missing import statement
- **invalid_no_class** - Missing class keyword
- **invalid_no_program_name** - Missing class name
- **invalid_no_main** - Missing main method
- **invalid_no_static** - Missing static keyword
- **invalid_no_parens_main** - Missing parentheses in main
- **invalid_no_braces_main** - Missing braces in main body
- **invalid_no_var** - Missing var keyword
- **invalid_missing_separator** - Malformed statement
- **invalid_missing_if_brace** - Unclosed if statement
- **invalid_no_if_condition** - Missing if condition
- **invalid_no_if_parens** - Missing parentheses in if
- **invalid_no_while_condition** - Missing while condition
- **invalid_no_while_body** - Missing while body
- **invalid_unclosed_string** - Unclosed string literal
- **invalid_expression** - Invalid expression syntax
- **invalid_no_assignment_op** - Missing assignment operator
- **invalid_extra_brace** - Extra closing brace
- **invalid_no_else_body** - Missing else body
- **invalid_getter_with_params** - Invalid getter with parameters

### 3. Edge Cases (10 tests)

These tests cover boundary conditions and special scenarios:

- **edge_empty_file** - Empty source file
- **edge_only_import** - Only import statement
- **edge_comment_only** - Only comments
- **edge_multiple_classes** - Multiple class definitions
- **valid_nested_if_else** - Nested if-else statements
- **valid_complex_expr** - Complex arithmetic expressions
- **valid_multiple_calls** - Multiple function calls
- **valid_string_concat** - String concatenation
- **valid_boolean** - Boolean literals (true/false)
- **valid_null** - Null literal

## Test Structure

Each test follows this pattern:

```bash
run_test "test_name" 'source_code' expected_exit_code
```

The test function:

1. Creates a temporary `.wren` file with the test code
2. Runs the compiler with the test file as input
3. Compares the actual exit code with the expected exit code
4. Reports success or failure

## Temporary Files

Test files are created in `test/syntax_tests/` directory:

- Files are automatically cleaned up if all tests pass
- Files are preserved if any test fails (for debugging)

## Exit Codes

The script returns:

- `0` - All tests passed
- `1` - One or more tests failed

## Test Coverage

This black box test suite covers:

- ✅ Basic program structure (import, class, main)
- ✅ Variable declarations and assignments
- ✅ Control flow statements (if-else, while)
- ✅ Expressions (arithmetic, comparison, logical)
- ✅ Function calls
- ✅ Getter and setter methods
- ✅ Nested structures
- ✅ Edge cases and error conditions
- ✅ Missing keywords and syntax elements
- ✅ Malformed statements
- ✅ Unclosed blocks and literals

## Extending the Tests

To add new tests:

1. Add a new test case in the appropriate section:

   ```bash
   run_test "test_name" 'your_wren_code' expected_exit_code
   ```

2. Make sure the test code is properly quoted
3. Use expected exit code `0` for valid syntax, `2` for syntax errors

## Integration with CI/CD

This test suite can be integrated into continuous integration pipelines:

```bash
# In your CI script
make clean
make
./test/test_syntax_blackbox.sh
if [ $? -eq 0 ]; then
    echo "Syntax tests passed"
else
    echo "Syntax tests failed"
    exit 1
fi
```

## Known Limitations

- Tests focus on syntax only (not semantic analysis)
- Some edge cases may depend on specific scanner behavior
- Test assumes standard error reporting (exit code 2 for syntax errors)

## Maintenance

When updating the parser:

1. Run the full test suite after changes
2. Add tests for new language features
3. Update expected results if syntax rules change
4. Keep test names descriptive and organized

## Related Tests

- `test_parsem.sh` - Parser and semantic connection tests
- `test/codes/*.wren` - Individual test cases
- `example_files/*.wren` - Example programs

---

**Author**: Generated for IFJ25 project  
**Last Updated**: November 25, 2025
