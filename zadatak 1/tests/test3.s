.section jedan
.skip 5
add r1,r2
xor r0,	r3
int r5
div r5, r1
.section druga
.word labela
.equ osmica, 8
push r0
str r3, osmica
halt
.end