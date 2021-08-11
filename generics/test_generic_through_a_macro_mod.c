/* This example illustrates using _Generic within a macro.
 */
#include <stdio.h>
#include <stdlib.h>

#define COMMA ,
#define type2str_mod(X) _Generic(X, char: "char", short: "short", int: "int", default: "other")

int main(void)
{
    long var = 0;
    printf("Type of 'var': %s\n", type2str_mod(var COMMA long: "long"));
    return EXIT_SUCCESS;
}
