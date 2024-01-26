#!/bin/bash

qemu-system-aarch64 -m 2048M -cpu cortex-a57 -machine type=virt,virtualization=on -display none -serial stdio -kernel rhv -s
