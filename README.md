# Disassembler Coursework
This is a simple disassembler written in C along with an additional nearly full implementation for MIPS assembly.

Part of my coursework at university. Mark 50/50

## Description
The aim of this Assessed Coursework Exercise is to implement & test a disassembler in C. A disassembler is a computer program that translates machine language into assembly language.

Optional Task: Enhance your implementation so that it works for a “real-world” MIPS assembly language.

The full excercise can be found in the <b>disassembler-coursework.pdf</b>.

## How to use it
The program requires one of the following arguments in order to run correctly:
- If the argument is <b>-d</b>, the program loads the default content inside it into the memory.
- If the argument is <b>-c</b>, the program loads user input from the console into the memory.
- If the argument is <b>-f</b>, the program loads the contents of a file into the memory.

The program then executes the stored program, starting at address location 0 and prints the contents of the memory and the value of the Accumulator after execution on the screen.

## Requirements:
- GCC

## Compile:
````
gcc disassembler.c
gcc disassembler-mips.c
````
