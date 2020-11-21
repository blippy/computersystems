

SD = /media/pi/8BC8-8129

install : flash
	
flash:
	cp kernel7.img $(SD)
	umount $(SD)

clean:
	-find . -name "kernel*.img" -print -delete
	-find . -name "*.o" -print -delete
	-find . -name "kernel*.map" -print -delete
	-find . -name "*.elf" -print -delete
	-find . -name "*.elf" -print -delete
	-find . -name "*.bin" -print -delete

