#!/bin/bash

if [ "$1" == "gdb" ]
then
	gdb="-S -s"
fi

qemu-system-aarch64 -m 2048M -cpu cortex-a57 -machine type=virt,virtualization=on -display none -serial stdio -kernel rhv $gdb
#qemu-system-aarch64 -M virt -m 2048M -cpu cortex-a57 -machine type=virt,virtualization=on -display none -serial stdio -drive if=pflash,format=raw,file=rhv.bin $gdb
