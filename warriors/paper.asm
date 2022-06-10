;;; name:   Cloner  (Paper example)
;;; syntax: <label> [opcode].<modifier> <admo>[A], <admo>[B]

cloner:  add.f  #1,     1      ;; increase both [mov.i] fields ([A],[B])
copy:    mov.i  -2,     1023   ;; copy entire program into address 1024 + index
         seq.ab copy,   split  ;; skip jump once clone is complete
         jmp    cloner         ;; return to 'cloner:' to copy next line into next address

         mov.i  reset,  1022   ;; reset new process 'copy:'
split:   spl    1020,   7      ;; create new process when 'copy:' [mov.i] [A] is equal to [spl] [B]

fire:    mov.i  3,      -10    ;; after the new process is made, spread a 'fire:' of [dat] codes backwards
         jmp    fire,   <fire

reset:   mov.i  -2,     1023   ;; never executed, stores original 'copy'
