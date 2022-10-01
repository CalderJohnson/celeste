mov sp, 100
mov ax, 72; 'H'
call print
mov ax, 105; 'i'
call print
jmp end
print:
out ax, 4
int 4
ret
end:
hlt