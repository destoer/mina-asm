; mem instr test

; register - immediate
ld r3, [r2,4]
ldh r3, [r2,4]
ldb r3, [r2,4]
st r3, [r2,4]
sth r3, [r2,4]
stb r3, [r2,4]

; not sure if im happy with the syntax on these
stc [r2,4]
ldc [r2,4]

; register - register
rld r3, [r2,r4]
rldh r3, [r2,r4]
rldb r3, [r2,r4]
rst r3, [r2,r4]
rsth r3, [r2,r4]
rstb r3, [r2,r4]
pop sp
push sp