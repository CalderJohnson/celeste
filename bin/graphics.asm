mov ax, 255; blue
out ax, 2; color value
mov ax, 0;top left
mov bx, 0
mov dx, 0;write mode
int 2;write
mov ax, 255
int 2
mov bx, 255
int 2
mov ax, 0
int 2; pixel in each corner
mov dx, 1;display mode
int 2;display
jmp hang
hang:
jmp hang
hlt