__Celeste comes equipped with a dedicated, simple assembly language for ease of writing programs__

Syntax is as follows: (only lowercase letters work)

Define a label with:
`label:`
This will reference an address that can be used to jump to code or reference a value. 10 characters max.

Define a value with:
`dw 5`
In conjunction with a label referencing it, this will create a value you can use in your program.

Dereference a value with *:
`mov ax, *5`
This will use the address of that number rather than the value itself.

Define an instruction with:
`<opcode> <args>`
Arguments are seperated by commas. Instructions are divided by newlines. Destination goes before source.

Write a comment with:
`;<comment>`
The assembler will ignore that text.

Instructions that can be used can be found in **ISA.md**. Their mnemonics and arguments are the same as described in that document.

Currently limited to max 50 labels and max 1000 tokens, but feel free to modify to suit your needs.

End all programs with a `hlt` instruction

Happy coding :)