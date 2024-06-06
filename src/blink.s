; set signal
movz w1, #0x4
; set output mode
movk w4, #0x40
; loop end
movz x2, #0xf, lsl #16
; GPIO "register" address
movz x3, #0x3f20, lsl #16

; set GPIO to out mode
str w4, [x3]

start:
; set high signal
str w1, [x3, #0x001c]

; reset counter
movz x0, #0

; do some addition to buy some time
wait1:
add x0, x0, #1
cmp x0, x2
b.ne wait1

; set low signal
str w1, [x3, #0x0028]

; reset counter
movz x0, #0

; do some addition to buy some time
wait2:
add x0, x0, #1
cmp x0, x2
b.ne wait2

b start
