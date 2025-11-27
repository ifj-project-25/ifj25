#!/bin/bash

# Master Test Runner for All Syntax Tests
# Runs all syntax test suites and provides comprehensive summary

# Color codes
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
MAGENTA='\033[0;35m'
NC='\033[0m'

echo -e "${MAGENTA}╔════════════════════════════════════════════════════════╗${NC}"
echo -e "${MAGENTA}║   IFJ25 Comprehensive Syntax Test Suite Runner        ║${NC}"
echo -e "${MAGENTA}╚════════════════════════════════════════════════════════╝${NC}\n"

# Check if compiler is built
if [ ! -f "./main" ]; then
    echo -e "${RED}Error: Compiler not found. Please run 'make' first.${NC}"
    exit 1
fi

# Track overall results
total_suites=0
passed_suites=0
total_tests=0
total_passed=0
total_failed=0

# Function to run a test suite
run_suite() {
    local suite_name="$1"
    local suite_script="$2"
    
    echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${BLUE}Running: $suite_name${NC}"
    echo -e "${CYAN}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}\n"
    
    total_suites=$((total_suites + 1))
    
    if [ ! -f "$suite_script" ]; then
        echo -e "${RED}✗ Suite script not found: $suite_script${NC}\n"
        return 1
    fi
    
    # Run the suite and capture output
    output=$($suite_script 2>&1)
    exit_code=$?
    
    # Extract statistics from output (remove ANSI color codes first)
    clean_output=$(echo "$output" | sed 's/\x1b\[[0-9;]*m//g')
    suite_total=$(echo "$clean_output" | grep "Total tests:" | awk '{print $3}')
    suite_passed=$(echo "$clean_output" | grep "Passed:" | grep -v "Suite" | awk '{print $2}')
    suite_failed=$(echo "$clean_output" | grep "Failed:" | grep -v "Suite" | awk '{print $2}')
    
    # Display results
    if [ $exit_code -eq 0 ]; then
        echo -e "${GREEN}✓ Suite passed${NC}"
        passed_suites=$((passed_suites + 1))
    else
        echo -e "${RED}✗ Suite had failures${NC}"
    fi
    
    echo -e "  Tests: ${suite_passed}/${suite_total} passed"
    if [ "$suite_failed" != "0" ]; then
        echo -e "  ${YELLOW}Failed: $suite_failed${NC}"
    fi
    echo ""
    
    # Update totals (handle empty values)
    [ -n "$suite_total" ] && [ "$suite_total" -eq "$suite_total" ] 2>/dev/null && total_tests=$((total_tests + suite_total))
    [ -n "$suite_passed" ] && [ "$suite_passed" -eq "$suite_passed" ] 2>/dev/null && total_passed=$((total_passed + suite_passed))
    [ -n "$suite_failed" ] && [ "$suite_failed" -eq "$suite_failed" ] 2>/dev/null && total_failed=$((total_failed + suite_failed))
}

# Run all test suites
echo -e "${YELLOW}Starting test execution...${NC}\n"

run_suite "Basic Black Box Tests" "./test/test_syntax_blackbox.sh"
run_suite "Edge Case Tests" "./test/test_syntax_edge_cases.sh"
run_suite "Extended Syntax Tests" "./test/test_syntax_extended.sh"
run_suite "Syntax Error Detection Tests" "./test/test_syntax_errors.sh"

# Final summary
echo -e "${MAGENTA}╔════════════════════════════════════════════════════════╗${NC}"
echo -e "${MAGENTA}║              COMPREHENSIVE TEST SUMMARY                ║${NC}"
echo -e "${MAGENTA}╚════════════════════════════════════════════════════════╝${NC}\n"

echo -e "${BLUE}Test Suites:${NC}"
echo -e "  Total Suites: $total_suites"
echo -e "  Passed Suites: ${GREEN}$passed_suites${NC}"
echo -e "  Failed Suites: ${RED}$((total_suites - passed_suites))${NC}\n"

echo -e "${BLUE}Individual Tests:${NC}"
echo -e "  Total Tests: $total_tests"
echo -e "  Passed: ${GREEN}$total_passed${NC}"
echo -e "  Failed: ${RED}$total_failed${NC}"

if [ $total_tests -gt 0 ]; then
    success_rate=$((total_passed * 100 / total_tests))
    echo -e "  Success Rate: ${CYAN}${success_rate}%${NC}\n"
fi

# Final verdict
if [ $total_failed -eq 0 ]; then
    echo -e "${GREEN}╔════════════════════════════════════════════════════════╗${NC}"
    echo -e "${GREEN}║     ✅  ALL TESTS PASSED! EXCELLENT WORK! ✅          ║${NC}"
    echo -e "${GREEN}╚════════════════════════════════════════════════════════╝${NC}"
    exit 0
else
    echo -e "${YELLOW}╔════════════════════════════════════════════════════════╗${NC}"
    echo -e "${YELLOW}║     ⚠️  SOME TESTS FAILED - REVIEW REQUIRED ⚠️        ║${NC}"
    echo -e "${YELLOW}╚════════════════════════════════════════════════════════╝${NC}"
    exit 1
fi
