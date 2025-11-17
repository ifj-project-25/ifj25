#!/bin/bash

# Farbové kódy
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo "Building parser and semantic connection tests..."
# Kompilácia sa už vykonala v Makefile, takže len oznamujeme
echo "Build completed."

echo "Running parser and semantic connection tests..."
total=0
passed=0

for file in test/codes/*.wren; do
    [ -f "$file" ] || continue  # Preskočí ak súbor neexistuje
    total=$((total + 1))
    echo -n "Testing $file... "
    ./main < "$file" > /dev/null 2>&1
    exit_code=$?
    if [ $exit_code -eq 0 ]; then
        echo -e "${GREEN}✓ PASSED${NC}"
        passed=$((passed + 1))
    else
        echo -e "${RED}✗ FAILED (exit code: $exit_code)${NC}"
    fi
done

echo -e "${BLUE}Summary: $passed/$total tests passed${NC}"

# Voliteľne: farebný výsledok podľa úspešnosti
if [ $passed -eq $total ]; then
    echo -e "${GREEN}✅  All tests passed!${NC}"
elif [ $passed -eq 0 ]; then
    echo -e "${RED}❌  All tests failed!${NC}"
else
    success_rate=$((passed * 100 / total))
    echo -e "${BLUE}⚠️  Success rate: $success_rate%${NC}"
fi