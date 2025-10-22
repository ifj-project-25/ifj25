/**
 * @file main.c
 * @author xcernoj00,xmikusm00
 * @brief Main function of interpret
 */

#include "error.h"
#include "scanner.h"
#include <stdio.h>
#include "parser.h"

int main() {

    FILE *source_file;
    source_file = stdin;

    set_source_file(source_file);
    int return_code = parser();
    if (return_code != NO_ERROR){
        printf("%s,%d\n","error",return_code);
    }
    else{
        printf("%s\n","no_error");
    }

    return NO_ERROR;
}
