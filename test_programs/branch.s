; branch test
some_label_neg:
    mov r0, r5
some_label_eq:
    bra some_label_eq
    bra some_label_neg
    bra some_label_pos
    ; padding
    mov r0,r5
    mov r0,r6
    bra 550000
some_label_pos:

; pc rel branch

bra 0x20
bt 0x20
bf 0x20

; pc rel call
call 0x20
ct 0x20
cf 0x20