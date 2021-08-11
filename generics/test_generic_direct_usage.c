/* This example illustrates using _Generic within a macro.
 */
#include <stdio.h>
#include <stdlib.h>

#define type2str(X) _Generic((X), char: "char", short: "short", int: "int", default: "other")

int main(void)
{
    int var = 0;
    printf("Type of 'var': %s\n", type2str(var));
    return EXIT_SUCCESS;
}
