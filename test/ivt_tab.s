.extern _start, _terminal
.section ivt_tab
    .word _start
    .word 0x0
    .word 0x0
    .word _terminal
.end