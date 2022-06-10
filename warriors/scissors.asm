;;; name:   Scanner (Scissors example)
;;; syntax: <label> [opcode].<modifier> <admo>[A], <admo>[B]

scanner:  add.ab  #6,       target    ;; increase scanning address
target:   sne.ab  -2,       17        ;; skip jump if target address [A] field is not equal to [dat]
          jmp     scanner             ;; no target found, run next scan

          sub.ab  #6,       target    ;; shift target address back for better deployment coverage
deploy:   mov.i   c_bomb,   >target   ;; deploy cluster bomb
          djn     deploy,   counter   ;; decrement 'counter:' [B], then jump to 'deploy:' until [B] = 0

counter:  mov.ab  #16,       #16      ;; counter for cluster bombs deployed, resets [B] on execution
          mov.i   3,         >target  ;; finish deployment with [dat], kills processes after trap
          jmp     scanner             ;; return to address scanner

c_bomb:   spl     -6,        }0       ;; [spl] bomb to trap program in multiple process creation
;;       [dat]                        ;; [dat] kills all processes at the end of the cluster bombs
