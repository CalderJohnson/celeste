mov r1, 1;     current direction of travel
mov r2, 1;     len
mov r3, 65280; color
mov ax, 128;   x
mov bx, 128;   y
out r3, 2
call draw
jmp end
draw:
int 2
end:
jmp end
hlt
