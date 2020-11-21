It's all a bit of a dumping ground by mcarter

## **setjmp** implemented 2020-11-21

See both the setjmp.S file and the yasetjmp.h files, which define AARCH to be 32. You'll need to tweak it
for it to work on on 64-bit machines.

Sourced from [circle](https://github.com/rsta2/circle/blob/master/include/circle/setjmp.h)

setjmp has been tested, and appears to work.
