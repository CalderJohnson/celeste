#include <stdio.h>
#include <vm.h>
#include <instructions.h>
#include <interrupts.h> 

int main (int argc, char **argv) {
    if (argc != 2) { 
        printf("Usage: celeste <diskfile.bin>\n");
        return 1;
    }
    run(argv[1]);
    printf("Powering off. If this was unexpected, try debugging with single step mode\n");
    return 0;
}