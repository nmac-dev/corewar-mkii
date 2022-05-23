;;; name:   Bomber  (Rock example)
;;; Syntax: <label> [opcode].<modifier> <admo>[A], <admo>[B]

rock:  add.ab  #4,    bomb  ;; increase the 'bomb:' [B] field
       mov.i   bomb,  @bomb ;; copy the 'bomb:' to the addressed pointed at by it's [B] field
       jmp     rock         ;; return to 'rock:'

bomb:  dat     #0,    #12   ;; if a warrior's process executes a [dat] code, the process is killed
