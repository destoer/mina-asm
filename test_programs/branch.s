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