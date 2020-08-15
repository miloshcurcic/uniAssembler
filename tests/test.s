.extern unknown
.equ sym, unknown + 5
.section text
    jmp *sym(%pc)
.end