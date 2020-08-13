.global _start
.equ len, end - start
.extern data_out
.section text
_start:
    add $1, %r1
loop:
    mov var, %r0
    sub $1, var
    mov start(%r0), data_out
    cmp $0x00, var
    jne loop 
    halt
var:
    .word len
.section str
start:
    .byte 'T'
    .byte 'E'
    .byte 'S'
    .byte 'T'
    .byte 'I'
    .byte 'R'
    .byte 'A'
    .byte 'N'
    .byte 'J'
end:
    .byte 'E'
.end