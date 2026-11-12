# Aristocrat MK6 in MAME: work in progress

This fork is where I'm getting Aristocrat MK6 (MAV500/MKVI) poker machines working in MAME. The finished pieces go to MAME itself as small pull requests. This branch, `mk6-play`, has everything that works so far in one place, for anyone who wants to try it. The fork's other MK6 branches are old experiments or PR branches; build this one.

No ROMs are included or linked. You need your own dumps of the sets in MAME's `aristmk6.cpp`. Not affiliated with Aristocrat.

## What works (tested by me, Nov 2026)

- 61 US games tested from set-up saves, and all 61 take coins, spin and pay. Touch features and bonuses (afterhrs, jefffox) are played with the mouse. Most need setting up once with the setchip BIOS first (below). Mr Cashman family games (5c per credit) need about 25 coins before the first spin.
- 11 NSW (Australian) games play, set up from their own operator menu: amzqueen, bigben, bigbena, bigheart, gldnflc, 50lionrr, 100lions, 5dragsp, aduskcm, alchemy, amztempl.
- bobdolly and mrwoou (two-screen games) boot with their top screens, but stop on a video error.

The status of every set, including the 178 that boot to their game screen but nobody has set up and played yet, is in [docs/mk6_games.md](docs/mk6_games.md). Those are where testing helps most.

## Known issues

- Sound: now plays at its real rate (21484 Hz mono, through the new CS4341 DAC device). Thanks to sho-gun1 for reporting it ran at double speed.
- Touch works in play (with the mouse), but games that need touch during setup (for example 5koi) can't finish it yet.
- Some games need buttons the default panel doesn't have (for example beavbuk), and on NSW games some button names don't match the cabinet.
- Queensland and NZ games need a comms link that isn't emulated.
- Some US sets fail setup or have bad dumps.

## Building

Standard MAME build. For a quicker build with just this driver:

```
make SUBTARGET=mk6 SOURCES=src/mame/aristocrat/aristmk6.cpp
```

## Setting games up

Games keep their set-up in NVRAM, and a blank NVRAM stops on a memory error.

US games (`-bios us1`): run the setchip once with `-bios set-us1`, then copy its NVRAM folder `nvram/<game>_41` to `nvram/<game>_27` and run the game on `-bios us1`. The full steps, including Target Version 6.03.04 and a table of game types, are in `docs/mk6_setchip_guide.md` on this branch.

NSW games (`-bios au-nsw1`, NVRAM folder `nvram/<game>`): no setchip. In the operator menu (Operator key O; Bet 1 next, Bet 2 previous, Bet 3 choose, Service back):
1. RAM reset: Operator key on, Main Door (M) open, hold Bet 1 + Service.
2. Machine Options (under 5.1): set Hopper to None and Touch Screen to None.
3. Open the Logic Door (Z, called "Security Cage" on NSW) to save.
4. Clear lockups with the Reset key, which is MAME's Jackpot key (J).
The machine error log in the operator menu shows what's wrong when something stops.

## Testing and reports

With 300+ sets there's far more to test than I can do alone, so reports are very welcome. Open an issue at https://github.com/MrBaristaNZ/mame/issues with the set name, the BIOS, what happened, and a screenshot if you can. Please don't attach or link ROMs.

## Status in MAME

Merged: SH-4 timer fixes (#16681, #16716), hostless PCI root (#16724), SH-4 clocks (#16806).
Open: PLX PCI9054 (#16744), DS1302 clock (#16745), SH-4 DMA for sound (#16829), save states (#16834), set comment fixes (#16839).
Next: PMX1 graphics device, sound, inputs and NSW hardware, touch, button panels, set fixes, then working flags.

This work is done with AI help (Claude, Anthropic), and every PR says so.

---

# MAME

## What is MAME?

MAME is a multi-purpose emulation framework.

MAME's purpose is to preserve decades of software history. As electronic technology continues to rush forward, MAME prevents this important "vintage" software from being lost and forgotten. This is achieved by documenting the hardware and how it functions. The source code to MAME serves as this documentation. The fact that the software is usable serves primarily to validate the accuracy of the documentation (how else can you prove that you have recreated the hardware faithfully?). Over time, MAME (originally stood for Multiple Arcade Machine Emulator) absorbed the sister-project MESS (Multi Emulator Super System), so MAME now documents a wide variety of (mostly vintage) computers, video game consoles and calculators, in addition to the arcade video games that were its initial focus.

## Where can I find out more?

* [Official MAME Development Team Site](https://www.mamedev.org/) (includes binary downloads, wiki, forums, and more)
* [MAME Testers](https://mametesters.org/) (official bug tracker for MAME)

### Community

* [r/MAME](https://www.reddit.com/r/MAME/) on Reddit
* [MAMEdev Forum](https://forum.mamedev.org/)
* [MAMEdev Discussions](https://github.com/orgs/mamedev/discussions) on GitHub

## Development

![Alt](https://repobeats.axiom.co/api/embed/8461d8ae4630322dafc736fc25782de214b49630.svg "Repobeats analytics image")

### CI status and code scanning

[![CI (Linux)](https://github.com/mamedev/mame/workflows/CI%20(Linux)/badge.svg)](https://github.com/mamedev/mame/actions/workflows/ci-linux.yml) [![CI (Windows](https://github.com/mamedev/mame/workflows/CI%20(Windows)/badge.svg)](https://github.com/mamedev/mame/actions/workflows/ci-windows.yml) [![CI (macOS)](https://github.com/mamedev/mame/workflows/CI%20(macOS)/badge.svg)](https://github.com/mamedev/mame/actions/workflows/ci-macos.yml) [![Compile UI translations](https://github.com/mamedev/mame/workflows/Compile%20UI%20translations/badge.svg)](https://github.com/mamedev/mame/actions/workflows/language.yml) [![Build documentation](https://github.com/mamedev/mame/workflows/Build%20documentation/badge.svg)](https://github.com/mamedev/mame/actions/workflows/docs.yml)  [![Coverity Scan Status](https://scan.coverity.com/projects/5727/badge.svg?flat=1)](https://scan.coverity.com/projects/mame-emulator)

### How to compile?

If you're on a UNIX-like system (including Linux and macOS), it could be as easy as typing

```
make
```

for a full build,

```
make SUBTARGET=tiny
```

for a build including a small subset of supported systems.

See the [Compiling MAME](http://docs.mamedev.org/initialsetup/compilingmame.html) page on our documentation site for more information, including prerequisites for macOS and popular Linux distributions.

For recent versions of macOS you need to install [Xcode](https://developer.apple.com/xcode/) including command-line tools and [SDL 2.0](https://github.com/libsdl-org/SDL/releases/latest).

For Windows users, we provide a ready-made [build environment](http://www.mamedev.org/tools/) based on MinGW-w64.

Visual Studio builds are also possible, but you still need [build environment](http://www.mamedev.org/tools/) based on MinGW-w64.
In order to generate solution and project files just run:

```
make vs2022
```
or use this command to build it directly using msbuild

```
make vs2022 MSBUILD=1
```

### Coding standard

MAME source code should be viewed and edited with your editor set to use four spaces per tab. Tabs are used for initial indentation of lines, with one tab used per indentation level. Spaces are used for other alignment within a line.

Some parts of the code follow [Allman style](https://en.wikipedia.org/wiki/Indent_style#Allman_style); some parts of the code follow [K&R style](https://en.wikipedia.org/wiki/Indent_style#K.26R_style) -- mostly depending on who wrote the original version. **Above all else, be consistent with what you modify, and keep whitespace changes to a minimum when modifying existing source.** For new code, the majority tends to prefer Allman style, so if you don't care much, use that.

All contributors need to either add a standard header for license info (on new files) or inform us of their wishes regarding which of the following licenses they would like their code to be made available under: the [BSD-3-Clause](http://opensource.org/licenses/BSD-3-Clause) license, the [LGPL-2.1](http://opensource.org/licenses/LGPL-2.1), or the [GPL-2.0](http://opensource.org/licenses/GPL-2.0).

See more specific [C++ Coding Guidelines](https://docs.mamedev.org/contributing/cxx.html) on our documentation web site.

## License

The MAME project as a whole is made available under the terms of the
[GNU General Public License, version 2](http://opensource.org/licenses/GPL-2.0)
or later (GPL-2.0+), since it contains code made available under multiple
GPL-compatible licenses.  A great majority of the source files (over 90%
including core files) are made available under the terms of the
[3-clause BSD License](http://opensource.org/licenses/BSD-3-Clause), and we
would encourage new contributors to make their contributions available under the
terms of this license.

Please note that MAME is a registered trademark of Gregory Ember, and permission
is required to use the "MAME" name, logo, or wordmark.

<a href="http://opensource.org/licenses/GPL-2.0" target="_blank">
<img align="right" width="100" src="https://opensource.org/wp-content/uploads/2009/06/OSIApproved.svg">
</a>

    Copyright (c) 1997-2026  MAMEdev and contributors

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License version 2, as provided in
    docs/legal/GPL-2.0.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details.

Please see [COPYING](COPYING) for more details.
