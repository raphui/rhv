define load_linux
	restore ~/dev/linux/arch/arm64/boot/Image binary 0xA0000000
end

tar rem 127.0.0.1:1234
load_linux
file rhv
