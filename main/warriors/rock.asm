;;; name: Bomber (Rock example)
;;; Syntax: <label> [opcode].<modifier> <admo>[A], <admo>[B]
;;; Line 1: <start>    [add].<ab>          <#>[4],    < >[bmb]
start   add    #4,    bmb   ;; {parser} -> add.ab #4,  $3     
        mov    bmb,   @bmb  ;; {parser} -> mov.i  $2,  @2+(#0)
        jmp    start        ;; {parser} -> jmp    $-2, #0     
bmb     dat    #0,    #0    ;; {parser} -> dat    #0,  #0     
