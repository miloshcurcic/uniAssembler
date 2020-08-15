.extern _start
.global data_in, data_out
.section ivt_tab
    .word _start
    .word _invalid_instruction
    .word _timer
    .word _terminal

.section ivt_tab.text
.equ data_in, 0xFF00
.equ data_out, 0xFF02
_invalid_instruction:
    iret
_timer:
    iret
_terminal:
    cmp $'q', data_in
    jeq quit
    iret
quit: 
    halt
.end

