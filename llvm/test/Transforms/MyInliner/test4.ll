; RUN: opt -passes="function(my-inliner)" -S < %s | FileCheck %s

; Тест 4: Рекурсия

define void @callee_recursive() {
  call void @callee_recursive()
  ret void
}

; CHECK-LABEL: @caller_recursive
; CHECK: call void @callee_recursive
define void @caller_recursive() {
  call void @callee_recursive()
  ret void
}