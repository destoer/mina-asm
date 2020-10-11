; arith test program

; register - immediate
addi r4, r7, 5
addi r4, r7, -5
multi r7,r8, -0x20
divi r5, r5, 2
remi r5, r5, 2
slti, r5, r5, 8
sltiu r5, r5, 8
nop
pcaddi r13, 4

; register - register
add r4, r7, r8
mult r7,r8, r9
div r5, r5, r10
rem r5, r5, r2
slt, r5, r5, r1
sltu r5, r5, r4
pcadd r3, r4