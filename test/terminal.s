.global _terminal, data_out
.equ data_in, 0xFF00
.equ data_out, 0xFF02
.section text
_terminal:
    mov data_in, data_out
    iret
.end