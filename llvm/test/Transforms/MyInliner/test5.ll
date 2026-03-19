; RUN: opt -passes="function(my-inliner)" -S < %s | FileCheck %s

;; Тест 5: Цепочка вызовов
;; Проверяем, что B инлайнится в A, а C инлайнится в B.

declare void @external_work()

; Уровень 3: Самая глубокая функция
define void @callee_C() {
entry:
  call void @external_work()
  ret void
}

; Уровень 2: Вызывает C и сама будет вызвана из A
define void @callee_B() {
entry:
  call void @callee_C()
  ret void
}

; Уровень 1: Основная функция
; CHECK-LABEL: @caller_A
; CHECK-NOT:   call void @callee_B
; CHECK-NOT:   call void @callee_C
; CHECK:       call void @external_work
define void @caller_A() {
entry:
  call void @callee_B()
  ret void
}
