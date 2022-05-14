;;; name: Scanner (Scissors example)
;;; syntax: <label> [opcode].<modifier> <admo>[A], <admo>[B]

scanner:  add.ab  #3,      target  ;; adds 9 to the target search address
target:   sne.i   c_bomb,  3       ;; if the target is not a copy of c-bomb, deploy c-bomb
          jmp     scanner          ;; no target found, run next scan

deploy:   mov.i   c_bomb,  >target ;; deploy the cluster bomb at the target address, then post-inc target
          jmn     deploy,  <reset  ;; loop until reset B == 0
reset:    mov.ab  #3,      #3      ;; set B back to 3
          jmp     scanner          ;; move back to scanner

c_bomb:   dat     #0       #0      ;; cluster bomb for address
