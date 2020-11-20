SD = /media/pi/8BC8-8129

install : flash
	
flash:
	cp kernel7.img $(SD)
	umount $(SD)

