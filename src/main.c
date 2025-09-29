/**
 * @file main.c
 * @author xcernoj00
 * @brief Main function of interpret
 */

#include "error.h"
#include "scanner.h"
#include <stdio.h>

int main() {

    FILE *source_file;
    source_file = stdin;

    set_source_file(source_file);

    print_token_types();

    return NO_ERROR;
}
