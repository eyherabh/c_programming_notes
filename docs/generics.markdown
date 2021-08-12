# C generic selection (W.I.P.)

The following is a short incomplete summary of the specification for generic selections:

1. `_Generic` is a **primary expression**, not a macro. Hence, it is not expanded by the preprocessor (see also [[3]]).
2. Syntax: `_Generic( controlling-expression, type-name-1: result-expression[, type-name-2: result-expression, ...][, default: result-expression])`. 
3. For all `i`, `type-name-i` must be a complete type (see 6.2.5-1 in [[2]]) other than a VLA (see section 6.7.6.2 in [[1],[2]]). Recall that `void` is incomplete (see 6.2.5-19 in [[1],[2]]).
4. For all `i` and `j`, `type-name-i` and `type-name-j` must be neither compatible nor simultaneously compatible with `controlling-expression`. 
5. If `default` is absent, one `i` must exist such that `type-name-i` is compatible with `controlling-expression`.

The complete specification is given in 6.5.1 and 6.5.1.1 of [[2]]. It can also be found in [[1]], but [[2]] includes the clarifications of [[5]] on what sort of conversions the controlling expression undergoes (see [[4]] and [Conversions](#conversions)). When reading [[4]] and [[5]], take into account that **composite types** actually refers to **aggregate types** (see 6.2.7-3 and 6.2.5-20 in [[1],[2]]).


## Examples

All the examples below are compiled using GCC 9.3.0 in Ubuntu 20.04 on a AMD Ryzen 7 processor. A typical compilation call is
```
gcc [-std=c11] [-Wall] [-Wextra] [-pedantic] <source1.c> [<source2.c> ...]
```
where the square brackets indicate that their enclosed parameters are optional. This call compiles all sources, links them and produces the executable a.out. 

The flags are used to control the diagnostics, warnings and errors reported during compilation. The flag `-std=c11` is also used to turn off GNU extensions that conflict with the specified standard. For further details, see the man page (`man gcc`) or the [GCC manual](6).

### Direct usage

The file [test_generic_direct_usage.c](generics/test_generic_direct_usage.c) contains the following line
```
printf("Type of 'var': %s\n", _Generic(var, char: "char", short: "short", int: "int", default: "other"));
```
After compiling with gcc and running in my box, it shows
```
Type of 'var': int
```

### Through a macro

The file [test_generic_through_a_macro.c](generics/test_generic_through_a_macro.c) contains the following line
```
#define type2str(X) _Generic((X), char: "char", short: "short", int: "int", default: "other")
printf("Type of 'var': %s\n", type2str(var));
```
which expand to (`cpp <filename>` or `gcc -E <filename>`)
```
printf("Type of 'var': %s\n", _Generic((var), char: "char", short: "short", int: "int", default: "other"));
```

#### On protecting the macro argument in the definition

The macro arguement `X` is enclosed within parentheses in the macro definition to force it being evaluated as an expression. Without them, one could add elements to the generic association list. To illustrate this, the file [test_generic_through_a_macro_mod.c](generics/test_generic_through_a_macro_mod.c) contains the following lines
```
#define COMMA ,
#define type2str_mod(X) _Generic((X), char: "char", short: "short", int: "int", default: "other")
long var = 0;
printf("Type of 'var': %s\n", type2str_mod(var COMMA long: "long"));
```
which add the element `long: "long"` to the generic association list through the argument of `type2str_mod`. The lines expand to  (`cpp <filename>` or `gcc -E <filename>`)
```
printf("Type of 'var': %s\n", _Generic(var, long: "long", char: "char", short: "short", int: "int", default: "other"));
```
After compiling with gcc and running in my box, it shows
```
Type of 'var': long
```
as expected.

The suffix `_mod` is used to distinguish it from `type2str`. Oftentimes, the suffix for macros with unprotected arguments in its definition is `_unsafe`. 

The macro `COMMA` is required. Although surrounding commas with parentheses is sufficient to prevent them from being interpreted as argument separators, those parentheses are preserved in the macro expansion and prevent the element from being added to the generic association list. To illustrate this, the file [test_generic_through_a_macro_mod_comma_required.c](generics/test_generic_through_a_macro_mod_comma_required.c) contains the following lines
```
printf("Type of 'var': %s\n", type2str_mod(var (,) long: "long"));
printf("Type of 'var': %s\n", type2str_mod((var , long: "long")));
```
These lines expand to
```
printf("Type of 'var': %s\n", _Generic(var (,) long: "long", char: "char", short: "short", int: "int", default: "other"));
printf("Type of 'var': %s\n", _Generic((var , long: "long"), char: "char", short: "short", int: "int", default: "other"));
```
Showing that the parentheses were preserved in the expansion. Further, no error or warning was shown about extra arguments, thereby implying that the comma was protected and not interpreted as an argument separator. When compiling, GCC 9.3.0 shows the following errors
```
test_generic_through_a_macro_mod_comma_required.c: In function ‘main’:
test_generic_through_a_macro_mod_comma_required.c:12:53: error: expected expression before ‘,’ token
   12 |     printf("Type of 'var': %s\n", type2str_mod(var (,) long: "long"));
      |                                                     ^
test_generic_through_a_macro_mod_comma_required.c:13:55: error: expected expression before ‘long’
   13 |     printf("Type of 'var': %s\n", type2str_mod((var , long: "long")));
      |                                                       ^~~~
```

### `void` is not allowed in the generic association list

The file [test_generic_void.c](generics/test_generic_void.c) contains the following line
```
printf("Type of var: %s\n", _Generic(var, void: "void", default: "other"));
```
which should prevent the file form compiling because void is an incomplete type, and incomplete types are not allowed as type-names (see rule 4 above). As expected, when compiling (`gcc -c <filepath>`), GCC 9.3.0 shows the following error
```
test_generic_void.c: In function ‘main’:
test_generic_void.c:9:47: error: ‘_Generic’ association has incomplete type
    9 |     printf("Type of var: %s\n", _Generic(var, void: "void", default: "other"));
      |                                               ^~~~
```

### Distinguishing enumerations

Generic selections have limited support for enumerations because:

A. Enumeration constants have type `int` (see 6.4.4.3 in [[1],[2]]). 
B. Enumeration types are compatible with signed or unsigned integer types (chosen by the compiler, see 6.7.2.2-4 in [[1],[2]]; note that `char` is an integer type, see 6.2.5 in [[1],[2]]). 
C. The choice of compatible type may vary with the enumeration type.
D. GCC chooses signed types when negative values exist, and unsigned otherwise. The chosen base type is `char`, `short` or `int` when GCC is passed flag `-fshort-enums`, and `int` when not (see section 4.9 in [[6]]). 

As a result, using enumeration and integer types in the type-name list can violate rule 4 depending on the compiler flags (see first section and [[7]]). Even if the type-name list contains only enumeration types, rule 4 may be violated when the controlling expression is an enumeration constant or an integer depending on the compiler flats. Finally, GCC does not behave exactly as the manual indicates (rule D) and produces limited warnings and errors. The first two cases are illustrated in the examples below; the latter, in another article. 


#### Based on their compatible integer type

Although all enumeration constants are compatible with `int`, but they can be casted to their corresponding enumeration type. Doing so enables to distinguish those belonging to different enumeration types provided that they are compatible with different integer types when GCC is given the `-fshort-enums. To illustrate this, the file [test_generic_enum_const_int.c](generics/test_generic_enum_const_int.c) contains the following lines

```
enum t { t1 = -1, t2 = 2 };
#define gettype(x) _Generic((x), signed char: "signed char", int: "int", default: "other")
#define prt_comp_type(x) printf("'" #x "' is compatible with type: '%s'\n", gettype(x))
...
prt_comp_type(t1);
prt_comp_type((enum t)t1);
```
After compiling without the `-fshort-enums` flag, it produces
```
't1' is compatible with type: 'int'
'(enum t)t1' is compatible with type: 'int'
```
Both `t1` and `(enum t)t1` are identified as `int` as expected, the former because of rule A and the latter because of rules B and C.

The result changes after compiling with the `-fshort-enums` flag as follows
```
't1' is compatible with type: 'int'
'(enum t)t1' is compatible with type: 'signed char'
```
Now, `(enum t)t1` was not identified as `int` but as `signed char` because GCC chose a narrower integer after being passed the flag `-fshort-enums`.

##### On char signedness

In the previous example, the generic selection containts the type-name `signed char` instead of just `char` because of portability. Whether `char` is equivalent to `signed char` or `unsigned char` is upto the compiler (see 6.2.5-15 in [[1],[2]]). In my box, GCC sets `char` equivalent to `unsigned char`. Nevertheless, the signedness of `char` can be changed in GCC by passing the flags `-fsigned-char` or `-funsigned-char` (see sections 3.4 and 4.4 in [[6]]).

#### Based on their enumeration type

While enumeration types are compatible with `int`, they are not compatible between them. Hence, generic selections can contain multiple enumeration types without breaking rule 4. However, that rule may be broken depending on the type of the controlling expression (after conversions; see [[4]] and 6.5.1.1-2 of [[2]]). To illustrate this, the file [test_generic_enum_const_enum.c](generics/test_generic_enum_const_enum.c) contains the following lines

```
enum t { t1 = -1, t2 = 2 };
enum u { u1 = 1,  u2 = 2 };
#define gettype(x) _Generic((x), enum t: "enum t", enum u: "enum u", default: "other")
#define prt_comp_type(x) printf("'" #x "' is compatible with type: '%s'\n", gettype(x))
...
prt_comp_type(t1);
prt_comp_type(u1);
prt_comp_type((enum t)t1);
prt_comp_type((enum u)u1);
```
After compiling without the `-fshort-enums` flag, it produces
```
't1' is compatible with type: 'enum t'
'u1' is compatible with type: 'enum t'
'(enum t)t1' is compatible with type: 'enum t'
'(enum u)u1' is compatible with type: 'enum u'
```
The first two lines show that both enumeration constants `t1` and `u1` are identified as of type `enum t`. The behaviour is actually undefined in both cases (see 4-2 in [[1],[2]]) because of violating rule 4. The violation stems from the fact that both `t1` and `u1` are compatible with both `enum t` and `enum u` because all are compatible with `int`. This can be avoided by casting them as shown by the last two lines.

The result changes after compiling with the `-fshort-enums` flag as follows
```
't1' is compatible with type: 'other'
'u1' is compatible with type: 'other'
'(enum t)t1' is compatible with type: 'enum t'
'(enum u)u1' is compatible with type: 'enum u'
```
Now the issue does not occur because, by passing the flag `-fshort-enums`, GCC made `enum t` compatible with `signed char` and `enum u` compatible with `unsigned char` (not shown here; run the example).


### Conversions

Compilers have perform the conversion of the controlling expression differently (see [[4]]). The misspecification in [[1]] has been clarified in [[2]] and recent versions of GCC and CLANG follow suit. However, when working with outdated systems, the conversions can be forced, for example, as illustrated in [[7]].



# References

[1]: http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1548.pdf
[2]: http://www.open-std.org/jtc1/sc22/wg14/www/docs/n2310.pdf
[3]: https://stackoverflow.com/questions/36050446/generic-macro-not-expanding/36050491
[4]: https://gustedt.wordpress.com/2015/05/11/the-controlling-expression-of-_generic/
[5]: http://www.open-std.org/jtc1/sc22/wg14/www/docs/n2257.htm#dr_481
[6]: https://gcc.gnu.org/onlinedocs/gcc
[7]: https://stackoverflow.com/questions/29157438/c11-enums-in-generic-selections

1. [C11 standard draftd](1)
2. [C2X standard draftd](2)
3. [_Generic() macro not expanding](3)
4. [The controlling expression of _Generic](4)
5. [Controlling expression of _Generic primary expression (DR481)](5)
6. [Using the GNU Compiler Collection](6)
7. [C11 - enums in Generic Selections](7)
