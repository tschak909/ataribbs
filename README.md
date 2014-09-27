ataribbs
========

A BBS for the Atari 8-bit - Written in CC65

## Purpose

Very simple, really. I want to see what's possible with the excellent [CC65](http://cc65.github.io/) compiler. It's also a _mental cucumber_ for me, a way to clear my head in between large projects (. o O (yeah, by starting another one.))

## Why CC65? 

Previously, all my Atari 8-bit programming experience was either in Atari BASIC, or in various assemblers. I had tried ACTION! at one point, and while I did like the ALGOL derived syntax, the simplifications that were made to make ACTION! work well on a 6502, based on knowledge in the community at that time made the language a bit limiting...

When I returned to doing things on the Atari 8-bit world, I quickly realized the powerful combination of cross assembling to an emulated Atari (I currently use Avery Lee's excellent [Altirra](http://www.virtualdub.org/altirra.html) package, made both the compilation process so fast, and the debugging process so intimate, that it literally made development turn-around sky-rocket. The very advantages of 30 years ago have now been completely reversed.

My previous experiences with C compilers on 6502 machines, were nothing short of disasterous. They were tiny subsets of C (informally I always heard people refer to these hobbled dialects of C as 'pidgin' C), that often produced very suboptimal code (certain dialects were literally ports of Intel 8080 compilers that generated 8080 intermediate code, that was transcoded to C in another intermediate pass!), and given what I knew about the 6502 (small stack, few registers, lack of certain instructions), I was _certain_ that there wouldn't be a decent dialect of C.

...CC65 has, so far, blown past all of the shortcomings inherent to a C compiler on 6502, and thus far as done far better than I ever expected. It has allowed me to write code in a very decent subset of C89/C99, and produce code that is good enough, with the option of quickly dropping down into 6502 assembler, exactly when I need it.

So We'll see where this goes.

## Goals

* Produce a modular BBS system, capable of building any interactive terminal experience. 
* Produce an API for others to make modules in C.
* Produce a BBS capable of running under SpartaDOS X, taking advantage of its feature set (sorry, no MyDOS)
* Produce a _sane default_ configuration, which can be used to bring up a BBS quickly.
* Produce Documentation on the use of the system.
* Be memory efficient, speed is less important. 

## Non-Goals

* Compatibility with existing BBS data, or executables. Sorry, If you want that, please add it. It's not a priority for me, and I have limited time to dedicate to this project. I have to keep my goals very clear and compact. 

## Contribution

I will accept and review any patches, additional utilties, code, etc. that anyone wishes to contribute.

## Licensing

This code is, and will always be licensed by the GNU Public License (GPL), starting with Version 3. I do not intend to make any money off this code, as I am doing it for fun, and for my own personal mental acuity (I do these things to keep my mind sharp). Please abide by the rules of the license.

## Binaries

At some point, sure. Not yet though. 

## Building

To build, you need a copy of CC65, installed on a UNIX (including Linux and Mac OS X) system, or a Windows system running Cygwin. You need CC65_HOME defined in your .profile, and cc65's bin directory in your path. 

After which, you can build the individual bits, by going to each directory, and typing 'make'

The resulting binaries can then be copied to an ATR image, or Atari disk (out of the scope of this document), and run either in emulation, or on a real Atari 8-bit computer.

## Requirements

You will need:

* Atari 8-bit computer, either real or emulated.
* Atari 850 Interface (either real, or emulated)
* MODEM or Terminal interface of some kind (e.g. Lantronix UDS-1100), (either real or emulated)
* SpartaDOS X. No. I will not support anything else. Don't ask. 
* Big disk of some sort, either a large 65535 sector ATR, a hard disk, whatever. (If you're trying to get this to work on a single density 810, come here so I can thwap you.)

More to come later.
