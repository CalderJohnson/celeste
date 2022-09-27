__Celeste's memory is structured in the following manner__

Addresses 0x0      (12288) -> 0xFEFF (65279) - Available for user code
Addresses 0x0xFF00 (65280) -> 0xFFFF (65536) - Device register addresses

__Celeste supports the following instructions__

Misc:
NOP  -  no operation
HLT  -  halt

Data transfer:
LEA  -  load effective address
MOV  -  move
PUSH -  push
POP  -  pop

Control flow:
JMP  -  unconditional jump
CMP  -  compare
JZ   -  jump if zero
JC   -  jump if carry
JS   -  jump if sign
JP   -  jump if parity
JE   -  jump if equal
JNE  -  jump if unequal
JG   -  jump if greater than
JGE  -  jump if greater than or equal to
JL   -  jump if less than
JLE  -  jump if less than or equal to
INT  -  call to interrupt
CALL -  call to subroutine
RET  -  return

Transformative operations:
INC  -  increment
DEC  -  decrement
ADD  -  add
SUB  -  subtract
MUL  -  multiply
DIV  -  divide
AND  -  and
OR   -  or
XOR  -  xor
NOT  -  invert
SHL  -  shift left
SHR  -  shift right
ROL  -  rotate left
ROR  -  rotate right

IO:
IN   -  IO word in
OUT  -  IO word out

__Instructions are encoded in the following format__

in general:
    [1 byte opcode][4 bits specification][4 bits register][2 bytes address/immv]

For misc instructions:
    [1 byte opcode][000000000000000000000000]

For mov:
    [1 byte opcode][0000][4 bits destination register][4 bits source register][000000000000]
    [1 byte opcode][0001][4 bits destination register][4 bits source register (dereferenced)][000000000000]
    [1 byte opcode][0010][4 bits destination register (dereferenced)][4 bits source register][000000000000]
    [1 byte opcode][0011][4 bits destination register (dereferenced)][4 bits source register (dereferenced)][000000000000]
    [1 byte opcode][0100][4 bits destination register][2 bytes src addr]
    [1 byte opcode][0101][4 bits destination register (dereferenced)][2 bytes addr]
    [1 byte opcode][0110][4 bits destination register][2 bytes immv]
    [1 byte opcode][0111][4 bits destination register (dereferenced)][2 bytes immv]
    [1 byte opcode][1000][4 bits source register][2 bytes dest addr]
    [1 byte opcode][1001][4 bits source register (dereferenced)][2 bytes dest addr]

For lea:
    [1 byte opcode][0000][4 bits destination register][2 bytes addr]
    [1 byte opcode][0001][4 bits destination register (dereferenced)][2 bytes addr]

For push/pop:
    [1 byte opcode][0000][4 bits register][0000000000000000]
    [1 byte opcode][0001][4 bits register (dereferenced)][0000000000000000]
    [1 byte opcode][0010][0000][2 bytes addr]
    [1 byte opcode][0011][0000][2 bytes immv] (push only)

For int:
    [1 byte opcode][00000000][8 bits interrupt code][00000000]

For jump/call instructions:
    [1 byte opcode][0000][4 bits register][0000000000000000]
    [1 byte opcode][0001][4 bits register (dereferenced)][0000000000000000]
    [1 byte opcode][0010][0000][2 bytes addr]

For cmp:
    [1 byte opcode][0000][4 bits operand register 1][4 bits operand register 2][000000000000]
    [1 byte opcode][0001][4 bits register][2 bytes addr]
    [1 byte opcode][0010][4 bits register][2 bytes immv]

For binary mathemetical/logical instructions:
    [1 byte opcode][0000][4 bits destination register][4 bits operand register 1][4 bytes operand register 2][00000000]
    [1 byte opcode][0001][4 bits destination register][4 bits source register][000000000000]
    [1 byte opcode][0010][4 bits destination register][2 bytes addr]
    [1 byte opcode][0011][4 bits destination register][2 bytes immv]


For unary mathemetical/logical instructions:
    [1 byte opcode][0000][4 bits destination register][0000000000000000]
    [1 byte opcode][0001][4 bits destination register (dereferenced)][0000000000000000]
    [1 byte opcode][0002][0000][2 bytes addr]


For IO:
    [1 byte opcode][0000][4 bits register][1 byte port][00000000]


__Celeste is equipped with the following IO devices__:

Timer (Port 0x0, interrupt 0x0):
    Usage: Call interrupt 0x0 to have time since startup (in ms) stored in port 0x0

Keyboard (Port 0x1, interrupt 0x1):
    Implementation pending

Screen (Port 0x2, interrupt 0x2):
    Implementation pending

Disk (Port 0x3, interrupt 0x3):
    To write to disk:
        Set register `dx` to `1` to write to disk
        32 bit value to write goes in port 0x3
        Offset to write to goes in register `ax`
        Call interrupt 0x3 to have 32 bit value stored in port 0x3 written at offset
    To read from disk:
        Set register `dx` to `0` to read from disk
        Offset to read from goes in register `ax`
        Call interrupt 0x3 to have 32 bit data at offset stored in port 0x3

Serial debugging (Port 0x4, interrupt 0x4):
	Usage: Call interrupt 0x4 to have character stored in port 0x4 outputted to standard output
