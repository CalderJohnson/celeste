**DEFUNCT PROJECT, WORKING ON A BETTER ASSEMBLER/VM FROM WHAT I'VE LEARNED FROM THIS PROJECT**

__Celeste__

A simple, custom 32 bit virtual machine with a dedicated assembler. A personal exploration into system design and low level programming. See the ISA and the assembly guide for detailed information on how it works!

__Specs__
    - Ram: 64kb
    - Clock speed: 1mhz
    - Instruction count: 36

__Building__

You'll notice there are two main source folders, one for the assembler and one for the emulator. Change into one of them and run `make all` to build the executable which will reside in ./bin. Also in bin are example programs!

To build you will need:
    - Gnumake or equivalent
    - clang
    - csfml
