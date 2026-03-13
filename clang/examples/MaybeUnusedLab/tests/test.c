// RUN: clang-18 -Xclang -load -Xclang %S/../MaybeUnused.so -Xclang -ast-dump -fsyntax-only %s 2>&1 | FileCheck %s

// CHECK: UnusedAttr {{.*}} Implicit maybe_unused

int foo(int a, int b, int c) {
    double value = 0.0;
    return a + b;
}

int bar(int x, int y, int z) {
    int used = x + y;
    int unused1 = 42;
    int unused2 = 100;
    return used;
}

double baz(double a, float b, int c, char d) {
    double result = a + b;
    float temp1 = 3.14;
    int temp2 = 42;
    char temp3 = 'A';
    return result;
}