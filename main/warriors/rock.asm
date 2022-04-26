;;; name: Bomber (Rock example)
;;; Syntax: <label> [opcode].<modifier> <admo>[A], <admo>[B]

loop  add.ab  #4,    bomb  ;; add 4 to the bomb's B value
      mov.i   bomb,  @bomb ;; copy the bomb 12 + (4 * i) addresses ahead
      jmp     loop         ;; repeat loop indefinitely

bomb  dat     #0,    #12   ;; each loop iteration copies the bomb 4 addresses ahead of the previous
