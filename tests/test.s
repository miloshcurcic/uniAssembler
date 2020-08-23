.global _start
.section text
_start:
    mov $0x5, 0xff10
loop:
jmp loop
