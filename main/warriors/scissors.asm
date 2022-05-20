;;; name:   Scanner (Scissors example)
;;; syntax: <label> [opcode].<modifier> <admo>[A], <admo>[B]

scanner:  add.ab  #6,       target    ;; increase scanning address
target:   sne.a   c_bomb,   16        ;; skip jump if target address [A] field is not equal to 'c_bomb:'
          jmp     scanner             ;; no target found, run next scan

          sub.ab  #7,       target    ;; shift target address back for better deployment coverage
deploy:   mov.i   c_bomb,   >target   ;; target found deploy cluster bomb [dat] + [spl]
          mov.i   @c_bomb,  >target   ;; [spl]
          djn     deploy,   counter   ;; decrement 'counter:', then jump to 'deploy:' until [B] = 0

counter:  mov.ab  #7,       #7        ;; sets number of cluster bombs deployed, resets [B] on execution
          jmp     scanner             ;; return to address scanner

c_bomb:   dat     #0,       $1        ;; [dat] kills process, [spl] traps process in loop
          spl     #0
