It's all a bit of a dumping ground by mcarter

## **SD card** implemented 2020-11-24

Reading and writing to the SD card (32-bit) works!!! thanks to 
[bztsrc](https://github.com/bztsrc/raspi3-tutorial). Check out the rest of his repo and
other interesting repos he has.

There's no FAT32 filesystem, it's all done with raw blocks. 

A demo is available in sd-test.c.

Safety first! To ensure you don't corrupt your OS filesystem, ensure that it doesn't
take up the whole of the card. Insert the card into a Linux host system, and type:
```
sudo cfdisk /dev/sdd
```
substituting `sdd` for the card (type `lsblk` to identify the card).

Then add a second partition, give it a size of, say, 20M, and a partition type of 0 Empty.
The partition type is really just arbitrary, though.

You'll end up with a table that looks something like this:
```
    Device           Boot              Start            End        Sectors        Size      Id Type
>>  /dev/sdd1                           2048         614399         612352        299M       c W95 FAT32 (LBA)     
    /dev/sdd2                         614400         655359          40960         20M       0 Empty
    Free space                        655360       60751871       60096512       28.7G
```

Take a note of the start number for the empty partition: in out case 614400. Use this number
in sd-test.c to set the block. You can scribble all over the SD card, provided you stick within 
the start and end sector.

To write a file to that partition:
```
sudo dd if=myfile.txt of=/dev/sdd2
```

Compile the kernel, copy it over to the FAT partition, eject the card, and boot the Pi3 with it.
You should see that it prints out some of the file you copied over.

What's more, it replaces that block with it's own text.

Eject the card from the Pi, and insert it into the host machine. To display what has been written:
```
sudo dd if=/dev/sdd2 of=exciting.dat bs=100 count=1
```
Then do a hexdump of the created file:
```
hd exciting.dat | less
```
You should see something like this:
```
00000000  52 41 53 50 42 45 52 52  59 20 50 49 20 57 52 49  |RASPBERRY PI WRI|
00000010  54 45 42 41 43 4b 40 61  01 ee 51 1f 0e 49 0e ee  |TEBACK@a..Q..I..|
...
```
The contents are likely to be a little diferent. The important point is that
you should see the text "RASPBERRY PI WRITEBACK" as proof that the SD card
was written to successfully.




## **setjmp** implemented 2020-11-21

See both the setjmp.S file and the yasetjmp.h files, which define AARCH to be 32. You'll need to tweak it
for it to work on on 64-bit machines.

Sourced from [circle](https://github.com/rsta2/circle/blob/master/include/circle/setjmp.h)

setjmp has been tested, and appears to work.
