mov ax, 72; 'H'
out ax, 4; write to port
int 4; call serial output interrupt
mov ax, 101; 'e'
out ax, 4; write to port
int 4; call serial output interrupt
mov ax, 108; 'l'
out ax, 4; write to port
int 4; call serial output interrupt
mov ax, 108; 'l'
out ax, 4; write to port
int 4; call serial output interrupt
mov ax, 111; 'o'
out ax, 4; write to port
int 4; call serial output interrupt
mov ax, 32; ' '
out ax, 4; write to port
int 4; call serial output interrupt
mov ax, 87; 'W'
out ax, 4; write to port
int 4; call serial output interrupt
mov ax, 111; 'o'
out ax, 4; write to port
int 4; call serial output interrupt
mov ax, 114; 'r'
out ax, 4; write to port
int 4; call serial output interrupt
mov ax, 108; 'l'
out ax, 4; write to port
int 4; call serial output interrupt
mov ax, 100; 'd'
out ax, 4; write to port
int 4; call serial output interrupt
hlt