; RUN: opt -passes="function(my-inliner)" -S < %s | FileCheck %s

; Тест 1: void функция без аргументов — должна быть заинлайнена
define void @callee_simple() {
  ret void
}

; CHECK-LABEL: @caller_simple
; CHECK-NOT: call void @callee_simple
define void @caller_simple() {
  call void @callee_simple()
  ret void
}