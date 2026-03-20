; RUN: opt -passes="function(my-inliner)" -S < %s | FileCheck %s

; Тест 7: Проверка двойного инлайна

; Функция, которую инлайнится дважды
define void @callee_twice() {
entry:
  %val = alloca i32, align 4
  ret void
}

define void @main_double() {
entry:
  call void @callee_twice()
  call void @callee_twice()
  ret void
}

; CHECK-LABEL: define void @main_double()
; CHECK:       entry:
; CHECK-NOT:   call void @callee_twice
; CHECK:       br label %[[BLOCK1:entrycloned.*]]

; CHECK:       [[BLOCK1]]:
; CHECK:         alloca i32, align 4
; CHECK-NEXT:    br label %[[POST1:post.call.*]]

; CHECK:       [[POST1]]:
; CHECK-NEXT:    br label %[[BLOCK2:entrycloned.*]]

; CHECK:       [[BLOCK2]]:
; CHECK:         alloca i32, align 4
; CHECK-NEXT:    br label %[[POST2:post.call.*]]

; CHECK:       [[POST2]]:
; CHECK-NEXT:    ret void
