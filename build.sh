#!/bin/sh

# dw it'll be moved to a more proper build system at some point
# (writing a random script works better for me)

set -e

CC=ia16-elf-gcc
AR=ia16-elf-ar

CFLAGS="-O3 -Ilib/core/include -Ilib/fat/include -DCOMPILER_TYPE=COMPILER_GCC -mcmodel=tiny -std=gnu99 -nostdlib -ffreestanding -Lout/lib -Wall -Wextra"

mkdir -p out
mkdir -p out/obj/lib/core
mkdir -p out/obj/boot
mkdir -p out/boot
mkdir -p out/lib
mkdir -p out/system

echo "Building core library..."
nasm -f elf32    -o out/obj/lib/core/gcc.asm.o lib/core/src/compiler/gcc.asm
$CC $CFLAGS -c   -o out/obj/lib/core/gcc.c.o   lib/core/src/compiler/gcc.c
$CC $CFLAGS -c   -o out/obj/lib/core/memory.o  lib/core/src/memory.c
$CC $CFLAGS -c   -o out/obj/lib/core/fmt.o     lib/core/src/fmt.c
$AR rcs out/lib/libcore.a out/obj/lib/core/*.o

echo "Building bootloader..."
nasm -f bin -o out/boot/floppy.bin boot/s1/floppy.asm

$CC $CFLAGS -c -o out/obj/boot/main.o   boot/src/main.c
$CC $CFLAGS -c -o out/obj/boot/disk.o   boot/src/disk.c
$CC $CFLAGS -c -o out/obj/boot/screen.o boot/src/screen.c
$CC $CFLAGS -c -o out/obj/boot/memory.o boot/src/memory.c
$CC $CFLAGS -c -o out/obj/boot/fat.o    boot/src/fat.c
nasm -f elf32 -o out/obj/boot/entry.o boot/src/entry.asm

$CC $CFLAGS -T boot/boot.ld -o out/system/natboot.sys out/obj/boot/*.o -lcore -lgcc

echo "Building the flop"
dd if=/dev/zero of=out/natdos.flp bs=512 count=2880
dd if=out/boot/floppy.bin of=out/natdos.flp conv=notrunc
mcopy -i out/natdos.flp disk/** ::/
mcopy -i out/natdos.flp out/system/** ::/
