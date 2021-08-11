/* Example with _Generic used to detect the type of an enumeration constant. */
#include <stdio.h>
#include <stdlib.h>

enum t { t1 = -1, t2 = 2 };
#define gettype(x) _Generic((x), signed char: "signed char", int: "int", default: "other")
#define prt_comp_type(x) printf("'" #x "' is compatible with type: '%s'\n", gettype(x))

int main(void)
{
    prt_comp_type(t1);
    prt_comp_type((enum t)t1);
    return EXIT_SUCCESS;
}
