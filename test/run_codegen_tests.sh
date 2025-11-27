#!/usr/bin/env bash
set -euo pipefail

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RESET='\033[0m'

INPUT_DIR="test/codegen_tests"
EXP_DIR="test/codegen_tests/expected"
TMP_DIR="test/codegen_tests/tmp"
BIN="./main"

mkdir -p "$TMP_DIR"

pass=0
fail=0

printf "Running codegen golden tests...\n"

for f in "$INPUT_DIR"/*.wren; do
  name=$(basename "$f" .wren)
  exp="$EXP_DIR/$name.ifj25"
  out="$TMP_DIR/$name.out"
  if [[ ! -f "$exp" ]]; then
    printf "${YELLOW}SKIP${RESET} %-30s (missing expected)\n" "$name"
    continue
  fi
  if ! $BIN < "$f" > "$out" 2>&1; then
    printf "${RED}ERR ${RESET} %-30s (compiler failed)\n" "$name"
    fail=$((fail+1))
    continue
  fi
  # Normalize (strip trailing spaces) for diff robustness
  sed -i 's/[[:space:]]*$//' "$out"
  sed -i 's/[[:space:]]*$//' "$exp"
  if diff -u "$exp" "$out" > "$TMP_DIR/$name.diff"; then
    printf "${GREEN}PASS${RESET} %-30s\n" "$name"
    pass=$((pass+1))
    rm "$TMP_DIR/$name.diff"
  else
    printf "${RED}FAIL${RESET} %-30s (diff saved)\n" "$name"
    fail=$((fail+1))
  fi
done

total=$((pass+fail))
printf "\nSummary: %d total | %d passed | %d failed\n" "$total" "$pass" "$fail"
if (( fail > 0 )); then
  printf "Details of failures (unified diff):\n"
  for d in "$TMP_DIR"/*.diff; do
    [[ -f "$d" ]] || continue
    printf "--- %s ---\n" "$(basename "$d" .diff)"
    cat "$d"
  done
  exit 1
fi
exit 0
