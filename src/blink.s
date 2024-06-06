movz x1, #0xf
movk x4, #0x40
movz x2, #0xffff, lsl #32
movz x3, #0x3f20, lsl #16

str x4, [x3]

start:
str x1, [x3, #0x001e]

movz x0, #0
wait:
add x0, x0, #1
cmp x0, x2
b.ne wait

str x1, [x3, #0x002a]
b start
