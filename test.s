.text
.equ a, d - e + c + 0xa
mov %eax, a
mov %eax, a
mov %eax, a
d: .long 0x1
.long 0x2
.long 0x3
e: .long 0x4