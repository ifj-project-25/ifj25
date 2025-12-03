#!/bin/sh
# Script to count lines of code by category

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m' # No Color

# Animation delay
DELAY=0.05

# Function to print with delay
print_slow() {
    printf "%b" "$1"
    sleep $DELAY
}

# Clear screen and show header
clear
echo ""
print_slow "${BOLD}${CYAN}╔════════════════════════════════════════════╗${NC}\n"
print_slow "${BOLD}${CYAN}║                                            ║${NC}\n"
print_slow "${BOLD}${CYAN}║  ${YELLOW}█▀▀ █▀█ █▀▄ █▀▀   █▀ ▀█▀ ▄▀█ ▀█▀ █▀${CYAN}       ║${NC}\n"
print_slow "${BOLD}${CYAN}║  ${YELLOW}█▄▄ █▄█ █▄▀ ██▄   ▄█  █  █▀█  █  ▄█${CYAN}       ║${NC}\n"
print_slow "${BOLD}${CYAN}║                                            ║${NC}\n"
print_slow "${BOLD}${CYAN}╚════════════════════════════════════════════╝${NC}\n"
echo ""
sleep 0.3

print_slow "${BOLD}${MAGENTA}[*] ${NC}Analyzing codebase...\n"
sleep 0.2

# Lexikálna analýza (scanner)
print_slow "${CYAN}    ▸ ${NC}Scanning lexical analyzer... "
LEX=$(wc -l src/scanner.c src/scanner.h src/dynamic_string.c src/dynamic_string.h 2>/dev/null | tail -1 | awk '{print $1}')
sleep 0.1
print_slow "${GREEN}✓${NC}\n"

# Syntaktická analýza (parser + precedence parser)
print_slow "${CYAN}    ▸ ${NC}Parsing syntax analyzer... "
SYN=$(wc -l src/parser.c src/parser.h src/expr_precedence_parser.c src/expr_precedence_parser.h src/expr_precedence_stack.c src/expr_precedence_stack.h 2>/dev/null | tail -1 | awk '{print $1}')
sleep 0.1
print_slow "${GREEN}✓${NC}\n"

# Sémantická analýza (semantic)
print_slow "${CYAN}    ▸ ${NC}Checking semantic analyzer... "
SEM=$(wc -l src/semantic.c src/semantic.h 2>/dev/null | tail -1 | awk '{print $1}')
sleep 0.1
print_slow "${GREEN}✓${NC}\n"

# Generátor kódu
print_slow "${CYAN}    ▸ ${NC}Generating code generator stats... "
GEN=$(wc -l src/generator.c src/generator.h 2>/dev/null | tail -1 | awk '{print $1}')
sleep 0.1
print_slow "${GREEN}✓${NC}\n"

# Ostatné (main, error, ast, symtable)
print_slow "${CYAN}    ▸ ${NC}Processing utilities... "
OST=$(wc -l src/main.c src/error.h src/ast.c src/ast.h src/symtable.c src/symtable.h src/expr_ast.c src/expr_ast.h 2>/dev/null | tail -1 | awk '{print $1}')
sleep 0.1
print_slow "${GREEN}✓${NC}\n"

# Celkovo
TOTAL=$(wc -l src/*.c src/*.h 2>/dev/null | tail -1 | awk '{print $1}')

echo ""
sleep 0.2
print_slow "${BOLD}${YELLOW}╔═══════════════════════════════════════════════╗${NC}\n"
print_slow "${BOLD}${YELLOW}║           LINE COUNT STATISTICS               ║${NC}\n"
print_slow "${BOLD}${YELLOW}╠═══════════════════════════════════════════════╣${NC}\n"

printf "${BOLD}${YELLOW}║${NC} ${BLUE}%-30s${NC} ${BOLD}${GREEN}%13d${NC} ${BOLD}${YELLOW}   ║${NC}\n" "Lexikálna analýza" $LEX
sleep 0.15
printf "${BOLD}${YELLOW}║${NC} ${BLUE}%-30s${NC} ${BOLD}${GREEN}%13d${NC} ${BOLD}${YELLOW}   ║${NC}\n" "Syntaktická analýza" $SYN
sleep 0.15
printf "${BOLD}${YELLOW}║${NC} ${BLUE}%-30s${NC} ${BOLD}${GREEN}%13d${NC} ${BOLD}${YELLOW}    ║${NC}\n" "Sémantická analýza" $SEM
sleep 0.15
printf "${BOLD}${YELLOW}║${NC} ${BLUE}%-30s${NC} ${BOLD}${GREEN}%13d${NC} ${BOLD}${YELLOW}   ║${NC}\n" "Generátor kódu" $GEN
sleep 0.15
printf "${BOLD}${YELLOW}║${NC} ${BLUE}%-30s${NC} ${BOLD}${GREEN}%13d${NC} ${BOLD}${YELLOW}  ║${NC}\n" "Ostatné" $OST

print_slow "${BOLD}${YELLOW}╠═══════════════════════════════════════════════╣${NC}\n"
sleep 0.2
printf "${BOLD}${YELLOW}║${NC} ${BOLD}${RED}%-30s${NC} ${BOLD}${MAGENTA}%13d${NC} ${BOLD}${YELLOW} ║${NC}\n" "TOTAL LINES OF CODE" $TOTAL
print_slow "${BOLD}${YELLOW}╚═══════════════════════════════════════════════╝${NC}\n"
echo ""

