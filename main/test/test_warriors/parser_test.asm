;;; Warrior used to test several aspects of the parser and OS::Core
;;; MUST follow exact syntax: "[code].<mod> <admo>[A],<admo>[B]"

;;; All Instructions

;; System
nop 0,0     ; .b
dat 0,0     ; .f

mov 0,0     ; .i
mov #0,0    ; .ab
mov 0,#0    ; .b

;; Comparision
seq 0,0     ; .i
sne 0,0

seq #0,0    ; .ab
sne #0,0
slt #0,0

seq 0,#0    ; .b
sne 0,#0
slt 0,#0

;; Arithmatic
add #0,0    ; .ab
sub #0,0
mul #0,0
div #0,0
mod #0,0

add 0,#0    ; .b
sub 0,#0
mul 0,#0
div 0,#0
mod 0,#0

add 0,0     ; .f
sub 0,0
mul 0,0
div 0,0
mod 0,0

;; Jump
jmp 0,0     ; .b
jmz 0,0
jmn 0,0
djn 0,0
spl 0,0

;;; All Addressing Modes
nop #5, $15     ; Immidiate/Direct

nop *5, @15     ; Indirect

nop {5, <15     ; Pre/Dec

nop }5, >15     ; Post/Inc

;;; All Modifiers
mov.a  0,0      ; Single
mov.b  0,0
mov.ab 0,0
mov.ba 0,0

mov.f  0,0      ; Double
mov.x  0,0

mov.i  0,0      ; Full

;;; Labels
abcdefghijklmnopqrstuvwxyz: nop 0   ;; a -> z
ABCDEFGHIJKLMNOPQRSTUVWXYZ: nop 0   ;; A -> Z
_0123456789:                nop 0   ;; _, 0 -> 9
no_colon                    nop 0   ;; No Colon (valid)
