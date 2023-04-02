# Hydrogen

## Description

A hobby programming language.

Hydrogen... because it's simple and lightweight yet also very flammable if handled improperly 🔥

## Build Instructions

Linux only for now.

Dependencies:

* CMake
* NASM
* ld

```console
$ git clone https://github.com/orosmatthew/hydrogen
$ cd hydrogen
$ cmake -S . -B build
$ cmake --build build
```

## Compiler Usage

```console
$ hydro <program.hy>
$ ./program
```

The following files will be generated when compiled:
* `program` - The executable
* `program.asm` - The assembly
* `program.o` - Object file