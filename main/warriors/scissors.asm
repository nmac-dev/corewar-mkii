;;; name:   Scanner (Scissors example)
;;; syntax: <label> [opcode].<modifier> <admo>[A], <admo>[B]

scanner:  add.ab  #6,       target    ;; increase scanning address
target:   sne.ab  *c_bomb,  16        ;; skip jump if target address [A] field is not equal to [dat]
          jmp     scanner             ;; no target found, run next scan

          sub.ab  #7,       target    ;; shift target address back for better deployment coverage
deploy:   mov.i   c_bomb,   >target   ;; target found deploy cluster bomb [spl] * 'counter:' -> [dat]
          djn     deploy,   counter   ;; decrement 'counter:', then jump to 'deploy:' until [B] = 0

counter:  mov.ab  #15,       #15        ;; sets number of cluster bombs deployed, resets [B] on execution
          mov.i   *c_bomb,  >target   ;; finish deployment with [dat], *c_bomb points to c_bomb + [A]
          jmp     scanner             ;; return to address scanner

c_bomb:   spl     $1                  ;; [spl] bomb to traps program in process creation
;;       [dat]  <--  is here *c_bomb  ;; [dat] kills process 
