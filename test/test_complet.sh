#!/bin/bash

# Farbové kódy
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Parse options
VERBOSE=1
while getopts "n" opt; do
    case $opt in
        n)
            VERBOSE=0
            shift
            ;;
        \?)
            echo "Usage: $0 [-n] [file]"
            echo "  -n    No verbose tmp_output (suppress docker tmp_output)"
            exit 1
            ;;
    esac
done

# Ak je zadaný parameter, testuj len ten súbor
if [ -n "$1" ]; then
    if [ ! -f "$1" ]; then
        echo -e "${RED}Error: File $1 not found${NC}"
        exit 1
    fi
    
    echo -e "${BLUE}Testing single file: $1${NC}"
    echo -n "Compiling... "
    ./main < "$1" > tmp_output.ifj25 2>&1
    compiler_exit=$?
    
    if [ $compiler_exit -ne 0 ]; then
        echo -e "${RED}✗ Compilation FAILED (exit code: $compiler_exit)${NC}"
        cat tmp_output.ifj25
        exit 1
    fi
    
    echo -e "${GREEN}✓${NC}"
    echo -n "Running in docker... "
    
    sudo docker run --rm -v "$(pwd)/tmp_output.ifj25:/app/prog:ro,Z" docker.io/leondryaso/ic25int:latest prog > docker_tmp_output.txt 2>&1
    docker_exit=$?
    
    if [ $docker_exit -eq 0 ]; then
        echo -e "${GREEN}✓ SUCCESS${NC}"
    else
        echo -e "${RED}✗ FAILED (exit code: $docker_exit)${NC}"
    fi
    
    if [ $VERBOSE -eq 1 ]; then
        echo -e "${BLUE}Docker tmp_output:${NC}"
        cat docker_tmp_output.txt
    fi
    
    rm -f docker_tmp_output.txt
    exit $docker_exit
fi

echo -e "${BLUE}Running complete integration tests with docker...${NC}"

# Refresh sudo credentials at the start
sudo -v

total=0
passed=0
compiler_errors=0
runtime_errors=0

echo -e "${BLUE}=== Testing files that should PASS (codes-COMPLET) ===${NC}"
for file in test/codes-COMPLET/*.wren; do
    [ -f "$file" ] || continue
    total=$((total + 1))
    
    echo -n "Testing $file... "
    
    # Compile
    ./main < "$file" > tmp_output.ifj25 2>&1
    compiler_exit=$?
    
    if [ $compiler_exit -ne 0 ]; then
        echo -e "${RED}✗ COMPILATION FAILED (exit code: $compiler_exit)${NC}"
        compiler_errors=$((compiler_errors + 1))
        continue
    fi
    
    # Run in docker
    if [ $VERBOSE -eq 1 ]; then
        sudo docker run --rm -v "$(pwd)/tmp_output.ifj25:/app/prog:ro,Z" docker.io/leondryaso/ic25int:latest prog 2>&1 | tee docker_tmp_output.tmp
        docker_exit=${PIPESTATUS[0]}
    else
        sudo docker run --rm -v "$(pwd)/tmp_output.ifj25:/app/prog:ro,Z" docker.io/leondryaso/ic25int:latest prog > /dev/null 2>&1
        docker_exit=$?
    fi
    
    if [ $docker_exit -eq 0 ]; then
        echo -e "${GREEN}✓ PASSED${NC}"
        passed=$((passed + 1))
    else
        echo -e "${RED}✗ RUNTIME FAILED (exit code: $docker_exit)${NC}"
        runtime_errors=$((runtime_errors + 1))
    fi
    
    [ -f docker_tmp_output.tmp ] && rm -f docker_tmp_output.tmp
done



# Cleanup
rm -f tmp_output.ifj25

echo ""
echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BLUE}Summary: $passed/$total tests passed${NC}"

if [ $compiler_errors -gt 0 ]; then
    echo -e "${YELLOW}Compiler errors in OK tests: $compiler_errors${NC}"
fi

if [ $runtime_errors -gt 0 ]; then
    echo -e "${YELLOW}Runtime errors in OK tests: $runtime_errors${NC}"
fi

# Final result
if [ $passed -eq $total ]; then
    echo -e "${GREEN}✅  All tests passed!${NC}"
    exit 0
elif [ $passed -eq 0 ]; then
    echo -e "${RED}❌  All tests failed!${NC}"
    exit 1
else
    success_rate=$((passed * 100 / total))
    echo -e "${BLUE}⚠️  Success rate: $success_rate%${NC}"
    exit 1
fi
