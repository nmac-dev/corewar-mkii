;;; DO NOT EDIT
;;; (please mark as readonly)

;;; 'syntax_defaults' contains a dictionary of all the default syntax
;;;  It should NOT be parsed...
;;;     but instead compared against the PARSED result of 'defaults_test'


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;    All Syntax Defaults    ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;
;;    [code]
;;;;

;;; System
nop.b $0, $0    ;; .b
dat.f #0, #0    ;; .f

mov.i $0, $0    ;; .i
mov.ab #0, $0   ;; .ab
mov.b $0, #0    ;; .b

;;; Comparision
seq.i $0, $0    ;; .i
sne.i $0, $0

seq.ab #0, $0   ;; .ab
sne.ab #0, $0
slt.ab #0, $0

seq.b $0, #0    ;; .b
sne.b $0, #0
slt.b $0, #0

;;; Arithmatic
add.ab #0, $0   ;; .ab
sub.ab #0, $0
mul.ab #0, $0
div.ab #0, $0
mod.ab #0, $0

add.b $0, #0    ;; .b
sub.b $0, #0
mul.b $0, #0
div.b $0, #0
mod.b $0, #0

add.f $0, $0    ;; .f
sub.f $0, $0
mul.f $0, $0
div.f $0, $0
mod.f $0, $0

;;; Jump
jmp.b $0, $0    ;; .b
jmz.b $0, $0
jmn.b $0, $0
djn.b $0, $0
spl.b $0, $0


;;;;
;;    <admo>
;;;;

nop.b #5, $15   ;; Immidiate | Direct

nop.b *5, @15   ;; Indirect       ( A | B )

nop.b {5, <15   ;; Pre-Decrement  ( A | B )

nop.b }5, >15   ;; Post-Increment ( A | B )


;;;;
;;    <mod>
;;;;

mov.a $0, $0    ;; Single
mov.b $0, $0
mov.ab $0, $0
mov.ba $0, $0

mov.f $0, $0    ;; Double
mov.x $0, $0

mov.i $0, $0    ;; Full
