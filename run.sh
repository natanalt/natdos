#!/bin/sh

qemu-system-i386 -drive file=out/natdos.flp,if=floppy,format=raw $*

