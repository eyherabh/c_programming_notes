/* Example with _Generic used to detect the type of an enumeration constant */
#include <stdio.h>
#include <stdlib.h>

enum t { t1 = -1, t2 = 2 };
enum u { u1 = 1,  u2 = 2 };
#define gettype(x) _Generic((x), enum t: "enum t", enum u: "enum u", default: "other")
#define prt_comp_type(x) printf("'" #x "' is compatible with type: '%s'\n", gettype(x))

#define int_types(X) X(signed char,) X(short,) X(int,) X(long,) X(long long,) X(unsigned char,) X(unsigned short,) X(unsigned int,) X(unsigned long,) X(unsigned long long,)
#define types_spec(x, ...) x : #x,
#define gettype_int(x) _Generic((x), int_types(types_spec) default: "other")

#define prt_int_type(x) printf("'" #x "' is compatible with type: '%s'\n", gettype_int(x))

int main(void)
{
    prt_comp_type(t1);
    prt_comp_type(u1);
    prt_comp_type((enum t)t1);
    prt_comp_type((enum u)u1);

    prt_int_type(t1);
    prt_int_type(u1);
    prt_int_type((enum t)t1);
    prt_int_type((enum u)u1);
    
    return EXIT_SUCCESS;
}
