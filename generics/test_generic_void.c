/* This example illustrates the use of void as a type-name in a generic association list. It should not compile because that is not allowed (see 6.5.1.1 in C11 standard).
 */
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int var = 0;
    printf("Type of var: %s\n", _Generic(var, void: "void", default: "other"));
    return EXIT_SUCCESS;
}
