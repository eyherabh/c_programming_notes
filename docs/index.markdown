## C pre-processor magic, revisited (W.I.P.)

Macros are almost always discouraged, but they are far from rare in production code. Therefore, it is of interest to explore their pitfalls, limitations, and compliance with the C-standard and others. In this article, I will analyse the macros introduced by the articles [C pre-processor magic][1][[1]], [C Preprocessor tricks, tips, and idioms][2][[2]] and [Is the C preprocessor Turing complete?][3][[2]], complementing the information there provided.

[Read more ...](https://eyherabh.github.io/c_programming_notes/cpp_magic_revisited)

## C generic selection (W.I.P.)

Generic selections provide support for method overloading based on the type of their argument. However, their usage is not entirely straightforward due to type compatibility, misspecifications and differences across compilers and flags [[4]]. In this article, I summarize and illustrate the usage and pitfalls of generic selections.

[Read more ...](https://eyherabh.github.io/c_programming_notes/generics)

## References
[1]: http://jhnet.co.uk/articles/cpp_magic
[2]: https://github.com/pfultz2/Cloak/wiki/C-Preprocessor-tricks,-tips,-and-idioms
[3]: https://github.com/pfultz2/Cloak/wiki/Is-the-C-preprocessor-Turing-complete%3F
[4]: http://www.open-std.org/jtc1/sc22/wg14/www/docs/n2257.htm#dr_481

1. [C pre-processor magic](1)
2. [C Preprocessor tricks, tips and idioms](2)
3. [Is the C preprocessor Turing complete?](3)
4. [Controlling expression of _Generic primary expression (DR481)](4)
