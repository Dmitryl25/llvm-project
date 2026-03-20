; RUN: opt -passes="function(my-inliner)" -S < %s | FileCheck %s

; Тест 6: Работа с памятью и модификация глобальной переменной

@total_sum = global i32 100

define void @update_total() {
entry:
  %current = load i32, i32* @total_sum
  %temp = add i32 %current, 10
  %result = mul i32 %temp, 2
  store i32 %result, i32* @total_sum
  ret void
}

define void @main_process() {
entry:
  call void @update_total()
  %local_var = alloca i32
  store i32 500, i32* %local_var
  ret void
}

; CHECK-LABEL: define void @main_process()
; CHECK:       entry:
; CHECK-NOT:   call void @update_total()
; CHECK:       br label %entrycloned

; CHECK:       entrycloned:
; CHECK-NEXT:    %currentcloned = load i32, ptr @total_sum
; CHECK-NEXT:    %tempcloned = add i32 %currentcloned, 10
; CHECK-NEXT:    %resultcloned = mul i32 %tempcloned, 2
; CHECK-NEXT:    store i32 %resultcloned, ptr @total_sum
; CHECK-NEXT:    br label %post.call

; CHECK:       post.call:
; CHECK-NEXT:    %local_var = alloca i32
; CHECK-NEXT:    store i32 500, ptr %local_var
; CHECK-NEXT:    ret void