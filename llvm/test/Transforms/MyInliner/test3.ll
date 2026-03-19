; RUN: opt -passes="function(my-inliner)" -S < %s | FileCheck %s

; Тест 3: Функция с не-void возвращаемым типом не должна инлайниться

define i32 @callee_returns_int() {
  ret i32 0
}

; CHECK-LABEL: @caller_no_inline_ret
; CHECK: call i32 @callee_returns_int
define void @caller_no_inline_ret() {
  call i32 @callee_returns_int()
  ret void
}