;;; name: Scanner (Scissors example)
;;; syntax: <label> [opcode].<modifier> <admo>[A], <admo>[B]

scanner  add.ab  #9,      target  ;; adds 9 to the target address
target   sne.i   d-op,    9       ;; if the target is not a copy of d-op, deploy c-bomb
         jmp     scanner          ;; no target found, run next scan

deploy   mov.i   c-bomb,  >target ;; deploy the cluster bomb at the target address, then post-inc target
         jmn     deploy,  <reset  ;; loop until reset B == 0
reset    mov.ab  #3,      #3      ;; set B back to 3
         jmp     scanner          ;; move back to scanner

c-bomb   spl     0                ;; cluster bomb for address
d-op     dat     #0,      #0      ;; Default Core Opcode (not a target)
