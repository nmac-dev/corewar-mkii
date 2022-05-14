;;; name: Cloner (Paper example)
;;; syntax: <label> [opcode].<modifier> <admo>[A], <admo>[B]

counter:  nop     #0,        #6       ;; counts loop iterations, A = line number, B = lines left to clone

cloner:  mov.i   }counter,  #987     ;; post-inc line number, clone line to B address
         jmn     loop,      counter  ;; skip split until counter B == 0
         spl     #987                ;; create new process once cloner has finished copying
         mov.i   reset,     counter  ;; reset counter to original state        
loop:    jmp     cloner,    <counter ;; jump to top of loop and decrement cloner B field

reset:   nop     #0         #7       ;; saved orginal counter state (pre-dec: 7 - 1 = 6)

;; 0 | nop
;; 1 | mov.i
;; 2 | jmn
;; 3 | spl ----------------------
;; ..                           |
;; ..                           |
;; 987 | Clone goes here     <- |
