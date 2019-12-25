# Dropcode - random pieces of useful code

## About

I've decided to collect under one directory, all the various bits and pieces of
marginally useful code I usually carry around from project to project. Each of
these pieces of code are not large or important enough to make a proper library
out of them, yet I find myself needing them again and again.

## License

Author: John Tsiombikas `<nuclear@member.fsf.org>`

This code is free software.

You may consider every source file to be in the public domain, unless
there's an explicit license and copyright statement in it (check the begining of
each file you wish to use). I don't expect there will be any copyrighted files
in here, but if there are any, they will always be under a free software
license.

## Contents

 - sdr.h/sdr.c: low-level OpenGL shader program helpers
 - opengl.h: OpenGL includes for different platrforms
 - mesh/: elaborate mesh class, procedural mesh generators, and bounding volume geometries.
 - cmesh/: port of mesh to C
 - sanegl.h/sanegl.c: immediate mode and matrix stack calls for OpenGL ES2
 - curve.h/curve.cc: hermite/bspline curve class
 - timer.h/timer.c: cross-platform high-resolution timing functions
 - tpool.h/tpool.c: worker thread pool based on POSIX threads
 - threadpool.h/threadpool.cc: C++ 11 worker thread pool
 - ilist.h: intrusive linked list (C++ template class)
 - logger.h/logger.c: message logging of various types and multiple log targets
 - dynarr.h/dynarr.c: C dynamic/resizable array
 - dos/: graphics, input, and timer code for protected mode DOS programs (watcom/dos4gw)
 - md5.h/md5.c: MD5 message digest computation

## Dependencies

Anything which includes "vmath/vmath.h", depends on my math library:
https://github.com/jtsiomb/libvmath

Anything which includes "gmath/gmath.h", depends on my new math library:
https://github.com/jtsiomb/gph-math

## Contact

Feel free to contact me at `<nuclear@member.fsf.org>`.
