// RUN: clang-18 -Xclang -load -Xclang %S/../MaybeUnused.so -Xclang -ast-dump -fsyntax-only %s 2>&1 | FileCheck %s

int foo(int foo_a, int foo_b, int foo_c) {
    double foo_val = 0.0;
    return foo_a + foo_b;
}

// CHECK: FunctionDecl {{.*}} foo
// CHECK: ParmVarDecl{{.*}} foo_a 'int'
// CHECK-NOT: UnusedAttr
// CHECK: ParmVarDecl{{.*}} foo_b 'int'
// CHECK-NOT: UnusedAttr
// CHECK: ParmVarDecl{{.*}} foo_c 'int'
// CHECK: UnusedAttr
// CHECK: VarDecl{{.*}} foo_val 'double'
// CHECK: UnusedAttr

int bar(int bar_x, int bar_y, int bar_z) {
    int bar_used = bar_x + bar_y;
    int bar_unused1 = 42;
    int bar_unused2 = 100;
    return bar_used;
}

// CHECK: FunctionDecl {{.*}} bar
// CHECK: ParmVarDecl{{.*}} bar_x 'int'
// CHECK-NOT: UnusedAttr
// CHECK: ParmVarDecl{{.*}} bar_y 'int'
// CHECK-NOT: UnusedAttr
// CHECK: ParmVarDecl{{.*}} bar_z 'int'
// CHECK: UnusedAttr
// CHECK: VarDecl{{.*}} bar_used 'int'
// CHECK-NOT: UnusedAttr
// CHECK: VarDecl{{.*}} bar_unused1 'int'
// CHECK: UnusedAttr
// CHECK: VarDecl{{.*}} bar_unused2 'int'
// CHECK: UnusedAttr

double baz(double baz_a, float baz_b, int baz_c, char baz_d) {
    double baz_res = baz_a + baz_b;
    float baz_tmp1 = 3.14;
    int baz_tmp2 = 42;
    char baz_tmp3 = 'A';
    return baz_res;
}

// CHECK: FunctionDecl {{.*}} baz
// CHECK: ParmVarDecl{{.*}} baz_a 'double'
// CHECK-NOT: UnusedAttr
// CHECK: ParmVarDecl{{.*}} baz_b 'float'
// CHECK-NOT: UnusedAttr
// CHECK: ParmVarDecl{{.*}} baz_c 'int'
// CHECK: UnusedAttr
// CHECK: ParmVarDecl{{.*}} baz_d 'char'
// CHECK: UnusedAttr
// CHECK: VarDecl{{.*}} baz_res 'double'
// CHECK-NOT: UnusedAttr
// CHECK: VarDecl{{.*}} baz_tmp1 'float'
// CHECK: UnusedAttr
// CHECK: VarDecl{{.*}} baz_tmp2 'int'
// CHECK: UnusedAttr
// CHECK: VarDecl{{.*}} baz_tmp3 'char'
// CHECK: UnusedAttr
