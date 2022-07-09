.equ co	, 0x33
.section sekcijaa
.word poc
ldr r1, poc
ldr r2, co
add r1, r2
labela: sub r3, r2
cmp r1, r3
jne labela
or r4,r0
halt
.end
.equ etf, 053