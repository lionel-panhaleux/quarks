# Usage

C++ 17 required. If your compiler does not support the `-std=c++17` option, please modify the `Makefile` accordingly.

```bash
$ make
$ build/asm factorial.txt
9
362880
```

# Notes

The proposed code computes factorial with constant complexity in respect to the stack size.
Sadly, computing a factorial over 12 overflows the int32 format,
so a condition was added so that all numbers above 12 return 1 (in the absence of a character output capacity).

In absence of a division instruction, using multiple values on the stack for higher factorials is impossible.
Moreover, the absence of shift instructions prevents the use of
[reciprocal multiplication](http://homepage.cs.uiowa.edu/~jones/bcd/divide.html)
to this effect.

Arguably, just using a switch on hard coded results would have been simpler and could have allowed more results,
but it did not feel like it was in the spirit of this exercise.
