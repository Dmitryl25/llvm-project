; RUN: opt -passes="function(my-inliner)" -S < %s | FileCheck %s

; Тест 2: Функция с аргументами — НЕ должна инлайниться
define void @callee_with_args(i32 %x) {
  ret void
}

; CHECK-LABEL: @caller_no_inline
; CHECK: call void @callee_with_args
define void @caller_no_inline() {
  call void @callee_with_args(i32 42)
  ret void
}