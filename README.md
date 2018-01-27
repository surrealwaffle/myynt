# myynt
*myynt* implements a means of composing *Callable*s as a sequence of invocations,
omitting the return type and performing no operation on *Callable*s for which
no overload exists given some arguments, in the order that the callable objects are supplied.

This library is a small, header-only library.

# Requirements
This library makes use of C++17 language features such as fold-expressions.

# Sample Usage
```c++
#include <cstdio>

#include <string>

#include <myynt/overload_chain.hpp>

void Foo(int i) {
    printf("%22s: %d\n", "Foo(int)", i);
}

struct Bar {
    void operator()(std::string const& s) {
        printf("%22s: %s\n", "Bar(std::string const)", s.c_str());
    }
};

int main(int argc, char const* argv[]) {
    using namespace myynt;

    overload_chain m {
        [] (int i)         {printf("%22s: %X\n", "lambda(int)", i);},
        [] (char const* s) {printf("%22s: %s\n", "lambda(char const*)", s);},
        Foo,
        Bar{}
    };

    printf("----------------------------\n");
    m(0x539); // calls the first lambda and Foo
    printf("----------------------------\n");
    m("foo"); // calls the second lambda and the Bar callable object
    printf("----------------------------\n");
    m();      // nothing is called
    printf("----------------------------\n");

    return 0;
}
```

Running this program produces the following output:

```
----------------------------
           lambda(int): 539
              Foo(int): 1337
----------------------------
   lambda(char const*): foo
Bar(std::string const): foo
----------------------------
----------------------------
```

# License
*myynt* is licensed under the [Boost Software License](https://github.com/surrealwaffle/myynt/blob/master/LICENSE_1_0.txt).