
; register - immediate
movi r5, 5 * 5
mti r5, 5
mfi r5, 5

; register - special

; r5 = 0xffffffff
; we should encode a full 32 bit load
; as a pseudo instr using this
movl r5, 0xffff
movu r5, 0xffff

; test register - register
mov r0, r5
mt r2, r3
mf r3, r4
mtoc r8
mfrc r6
mtou r2,r1
mfou r9,r10