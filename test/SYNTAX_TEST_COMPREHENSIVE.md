# Comprehensive Syntax Test Suite Documentation

## Overview

A complete testing framework for the IFJ25 syntactic analyzer with **247 total tests** across 4 comprehensive test suites.

## Test Results Summary

- **Total Tests**: 247
- **Passed**: 242 (97.97%)
- **Failed**: 5 (2.03%)
- **Test Suites**: 4
- **Passed Suites**: 3

## Test Suites

### 1. Basic Black Box Tests (`test_syntax_blackbox.sh`)

**40 tests | 100% passing ✅**

Fundamental syntax validation covering:

- Minimal valid programs
- Variable declarations and assignments
- Control flow (if-else, while)
- Function definitions and calls
- Getters and setters
- Basic expressions
- Error conditions

**Categories**:

- 10 Valid Syntax Tests
- 20 Invalid Syntax Tests
- 10 Edge Cases

**Usage**: `./test/test_syntax_blackbox.sh`

---

### 2. Edge Case Tests (`test_syntax_edge_cases.sh`)

**80 tests | 93.75% passing (75/80) ⚠️**

Comprehensive boundary and corner case testing based on language specification:

**Categories** (15 total):

1. **PROLOG (Import)** - 5 tests ✅
2. **CLASS Definition** - 5 tests ✅
3. **Function/Method Definition** - 8 tests ✅
4. **Variable Declaration** - 6 tests (5/6 passing)
5. **Expressions & Assignment** - 6 tests ✅
6. **IF Control Flow** - 5 tests ✅
7. **WHILE Control Flow** - 4 tests ✅
8. **Function Calls** - 7 tests ✅
9. **Return Statements** - 5 tests ✅
10. **Literals** - 6 tests ✅
11. **Blocks & Scope** - 4 tests ✅
12. **Whitespace & Formatting** - 4 tests ✅
13. **Comments** - 4 tests ✅
14. **Syntax Boundaries** - 6 tests (5/6 passing)
15. **Semantic Errors** - 5 tests (3/5 passing)

**Known Failures**:

- Underscore-prefixed variables (lexical error)
- Maximum nesting depth exceeded
- Function argument count mismatch detection
- Function redefinition not caught

**Usage**: `./test/test_syntax_edge_cases.sh`

---

### 3. Extended Syntax Tests (`test_syntax_extended.sh`)

**72 tests | 100% passing ✅**

Advanced syntax scenarios and combinations:

**Categories** (11 total):

- **Operator Precedence & Associativity** - 6 tests

  - Multiplication vs addition precedence
  - Left-to-right associativity
  - Parentheses override
  - Complex nested expressions

- **Comparison Operators** - 5 tests

  - All comparison operators (==, !=, <, >, etc.)
  - Comparison in assignments
  - Comparison of expressions

- **String Operations** - 6 tests

  - Variable concatenation
  - Escape sequences
  - Inline concatenation
  - Empty strings
  - Escaped quotes

- **Function Call Variations** - 6 tests

  - Multiple parameters
  - Nested calls
  - Expressions as arguments
  - Recursive calls

- **Built-in Functions (Ifj.\*)** - 6 tests

  - Ifj.write(), Ifj.length()
  - Ifj.str(), Ifj.substring()
  - Ifj.chr(), multiple calls

- **Complex Control Flow** - 5 tests

  - If inside while
  - While inside if
  - Nested loops with conditionals
  - Multiple nested branches

- **Return Statement Variations** - 5 tests

  - Return variables, expressions
  - Return in both branches
  - Return in nested structures

- **Getter and Setter Patterns** - 5 tests

  - Getter with if-else
  - Getter usage in expressions
  - Setter usage
  - Getter/setter pairs

- **Variable Scoping** - 4 tests

  - Block scopes
  - Variable shadowing
  - Loop variable scope
  - Function-local variables

- **Null Handling** - 4 tests

  - Null assignments
  - Null returns
  - Null comparisons
  - Null checks

- **Complex Expressions** - 5 tests

  - Mixed operators
  - Multiple subexpressions
  - Function calls in expressions
  - Deep nesting

- **Invalid Syntax Tests** - 10 tests

  - Incomplete assignments
  - Unclosed parentheses
  - Consecutive operators
  - Missing blocks/conditions

- **Statement Ordering** - 5 tests
  - Sequential vs interleaved declarations
  - Function call ordering
  - Getter/setter ordering

**Usage**: `./test/test_syntax_extended.sh`

---

### 4. Syntax Error Detection Tests (`test_syntax_errors.sh`)

**55 tests | 100% passing ✅**

Focused on parser's error detection capabilities:

**Categories** (9 total):

- **Missing Tokens** - 9 tests

  - Missing import, class, static keywords
  - Missing braces, parentheses

- **Malformed Statements** - 6 tests

  - var without identifier
  - Incomplete assignments
  - Consecutive operators
  - Operators at wrong positions

- **Malformed Control Structures** - 9 tests

  - if/while without conditions
  - Missing parentheses
  - Missing blocks

- **Malformed Function Calls** - 5 tests

  - Missing parentheses
  - Trailing commas
  - Double commas
  - Missing arguments

- **Malformed Function Definitions** - 5 tests

  - Function without name
  - Trailing commas in parameters
  - Missing parameter identifiers

- **Malformed Expressions** - 5 tests

  - Unclosed/extra parentheses
  - Empty parentheses
  - Invalid operator placement

- **Malformed String Literals** - 3 tests

  - Unclosed strings
  - Single quotes (invalid)
  - Invalid escape sequences

- **Malformed Getter/Setter** - 4 tests

  - Setter without parameter
  - Setter with multiple parameters
  - Missing parentheses

- **Multiple Statement Errors** - 3 tests

  - Statements without separators
  - Multiple declarations on one line

- **Misplaced Keywords** - 3 tests

  - Keywords in wrong contexts

- **Wrong Token Types** - 3 tests
  - Numbers/keywords as identifiers

**Usage**: `./test/test_syntax_errors.sh`

---

## Running Tests

### Run Individual Suites

```bash
./test/test_syntax_blackbox.sh      # Basic tests
./test/test_syntax_edge_cases.sh    # Edge cases
./test/test_syntax_extended.sh      # Extended tests
./test/test_syntax_errors.sh        # Error detection
```

### Run All Tests

```bash
./test/run_all_syntax_tests.sh      # Master runner
```

### Prerequisites

```bash
make                                 # Build compiler first
```

## Test Coverage

### Language Features Tested

✅ Import statements  
✅ Class declarations  
✅ Function definitions (static methods)  
✅ Getters and setters  
✅ Variable declarations  
✅ Assignments and expressions  
✅ Arithmetic operators (+, -, \*, /)  
✅ Comparison operators (==, !=, <, >, etc.)  
✅ String operations and concatenation  
✅ Control flow (if-else, while)  
✅ Function calls (user-defined and built-in)  
✅ Return statements  
✅ Null handling  
✅ Comments (single-line and block)  
✅ Nested structures  
✅ Scope and shadowing  
✅ Expression precedence and associativity

### Error Detection Tested

✅ Missing keywords and tokens  
✅ Malformed statements  
✅ Unclosed blocks and parentheses  
✅ Invalid operator placement  
✅ Wrong token types  
✅ Misplaced keywords  
✅ Incomplete expressions  
✅ Malformed control structures  
✅ Invalid string literals

## Test File Structure

```
test/
├── run_all_syntax_tests.sh          # Master runner
├── test_syntax_blackbox.sh          # 40 basic tests
├── test_syntax_edge_cases.sh        # 80 edge case tests
├── test_syntax_extended.sh          # 72 extended tests
├── test_syntax_errors.sh            # 55 error detection tests
├── SYNTAX_TESTS_README.md           # Basic tests doc
├── EDGE_CASES_README.md             # Edge cases doc
└── SYNTAX_TEST_COMPREHENSIVE.md     # This file
```

## Exit Codes

Tests validate these compiler exit codes:

- `0` - Success (valid syntax and semantics)
- `1` - Lexical error (scanner)
- `2` - Syntax error (parser)
- `3` - Semantic error (undefined)
- `4` - Semantic error (redefined)
- `5` - Semantic error (wrong parameters)
- `6` - Semantic error (type incompatibility)
- `10` - Other semantic error

## Known Issues

### Edge Case Suite (5 failures)

1. **Variables starting with underscore** - Lexical error instead of syntax
2. **Maximum nesting depth** - Parser limit exceeded (100+ levels)
3. **Function argument count mismatch** - Returns error 3 instead of 5
4. **Function redefinition** - Not detected (passes instead of error 4)
5. **Single underscore identifiers** - Rejected by lexer

These failures represent actual compiler behavior and are documented.

## Adding New Tests

### Template

```bash
run_test "test_name" 'import "ifj25" for Ifj
class Program {
static main() {
    // Your test code
}
}' expected_exit_code "Description"
```

### Best Practices

1. Use descriptive test names
2. Add clear descriptions
3. Group related tests in categories
4. Test one concept per test
5. Include both valid and invalid cases
6. Document expected behavior

## CI/CD Integration

```bash
#!/bin/bash
# In your CI pipeline
make clean
make
./test/run_all_syntax_tests.sh
if [ $? -eq 0 ]; then
    echo "All syntax tests passed"
else
    echo "Some syntax tests failed"
    exit 1
fi
```

## Statistics

| Suite           | Tests   | Passed  | Failed | Rate       |
| --------------- | ------- | ------- | ------ | ---------- |
| Basic Black Box | 40      | 40      | 0      | 100%       |
| Edge Cases      | 80      | 75      | 5      | 93.75%     |
| Extended Syntax | 72      | 72      | 0      | 100%       |
| Error Detection | 55      | 55      | 0      | 100%       |
| **TOTAL**       | **247** | **242** | **5**  | **97.97%** |

## Feature Coverage Matrix

| Feature         | Basic | Edge | Extended | Errors | Total |
| --------------- | ----- | ---- | -------- | ------ | ----- |
| Import/Class    | 3     | 10   | 0        | 4      | 17    |
| Functions       | 4     | 18   | 11       | 10     | 43    |
| Variables       | 3     | 6    | 4        | 6      | 19    |
| Control Flow    | 5     | 14   | 5        | 9      | 33    |
| Expressions     | 3     | 6    | 16       | 5      | 30    |
| Strings         | 2     | 6    | 6        | 3      | 17    |
| Operators       | 1     | 6    | 11       | 6      | 24    |
| Getters/Setters | 2     | 0    | 5        | 4      | 11    |
| Null/Literals   | 2     | 6    | 4        | 0      | 12    |
| Comments        | 1     | 4    | 0        | 0      | 5     |
| Others          | 14    | 4    | 10       | 8      | 36    |

## Maintenance

### Regular Tasks

1. Run all tests after parser changes
2. Update tests when grammar changes
3. Add tests for new language features
4. Review and document failures
5. Keep expected exit codes accurate

### When to Update

- Grammar modifications
- New language features added
- Bug fixes affecting syntax
- Error code changes
- Scanner/lexer updates

---

**Test Suite Version**: 1.0  
**Last Updated**: November 25, 2025  
**Total Test Count**: 247  
**Overall Success Rate**: 97.97%  
**Status**: ✅ Production Ready
