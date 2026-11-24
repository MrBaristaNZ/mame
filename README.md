# Sega Naomi in MAME: work in progress

This fork is where I'm fixing Sega Naomi graphics (MAME's PowerVR2 renderer) and speed in MAME. The finished pieces go to MAME itself as small pull requests. This branch, `naomi-play`, has all of them together on current MAME master, for anyone who wants to try them. The fork's other Naomi branches are the separate PR branches; build this one.

No ROMs are included or linked. You need your own dumps of the sets in MAME's `naomi.cpp`. Not affiliated with Sega.

## What's fixed (tested by me, Nov 2026)

- Cosmic Smash (`csmasho`): plays. The stage was a white wall after the start screen (back-face culling was missing).
- Marvel vs Capcom 2 (`mvsc2`): the hyper gauges fill, hyper move effects show in full instead of inside a box, and fights run at about 100%.
- Capcom vs SNK (`capsnk`): the timer digits are clean, the squares around super moves and the sun glare are gone, and it holds about 100%.
- Crazy Taxi (`crzytaxi`): traffic cars are solid instead of see-through, and the money counter shows.
- Slashout (`slasho`): the radar stays inside its window, and it runs at 80-95% (was 50-65%).
- Alien Front (`alienfnta`): the map stays in its corner instead of covering the screen.
- Asian Dynamite (`asndynmt`): the mission select previews show the right scenes.
- Azumanga Daioh Puzzle Bobble (`azumanga`): runs well.
- Street Fighter Zero 3 Upper (`sfz3ugd`): the title screen's white panel and the attract gallery portraits draw properly (reported in issue #1, thanks john-iv).
- World Kicks (tested on `wldkicksu`; the other World Kicks sets run the same code): players have their kits, numbers and faces, and normal arms and legs instead of stretched spikes (16-bit texture coordinates, and the SH-4's cache-as-RAM layout).
- Virtua NBA (`virnbao`, `virnbap`): boots to the match demo and title. MAME crashed while decrypting the cartridge data.
- Virtua Athletics (`vathlete`, `vathletep`), La Keyboard (`keyboard`) and Psyvariar 2 (`psyvar2`): boot to their attract modes. MAME stopped with "SH2DRC: invalid opcode".
- Rhythm Tengoku (`rhytngk`): boots to the title and high-score table instead of a black screen.
- Initial D (`initd`, `initdo`): was black after the boot screen, because the sound chip's MIDI port always reported a byte waiting from a drive board that isn't there. It now reaches its title and attract mode (see known issues for the card reader setting).

Speed comes from drawing each frame on several CPU cores and from removing waits where the CPU stopped for the renderer for no reason. Each fix is in the renderer, not in one game, so other games may look better too.

## Known issues

- Marvel vs Capcom 2: a little lag around the start menu.
- Alien Front: calibrate the wheel and pedals first (Test button > GAME TEST MODE > CONTROLLER SETTINGS; a fresh NVRAM has them all at 0, so the controls do nothing). Press Start and Shot together to skip the network check.
- Fog isn't emulated, and tiles that a game asks to draw in several passes are drawn once.
- GD-ROM games are mostly untested so far (I'm still gathering the disc images).
- Dragon Treasure (`dragntr`, `dragntr2`) now finds its game file but stops at NOW LOADING: it needs the satellite network hardware, which MAME doesn't emulate.
- Initial D (`initd`, `initdo`, `initdexp`, `initdv2j`, `initdv3j`, `initdv3e`): reaches the title screen and attract mode once CARD R/W is set to OFF in the game's test mode (GAME ASSIGNMENTS), then test mode is left through the BIOS menu's EXIT; with it ON, the game stops at CARD R/W ERROR! (the card reader isn't emulated). Some versions first need the BIOS SYSTEM ASSIGNMENTS set (Caution 51). There's no force feedback.
- Club Kart (`clubkrt`, `clubkrta`, `clubkrtc`, `clubkrto`): set CARD SYSTEM to DISABLE in the game's test mode (GAME SETTING), then press Start on the STEERING DRIVE BD. IS NOT CONNECTED screen; it then plays (no force feedback). With CARD SYSTEM on, it stops at CONNECTION ERROR. Club Kart 2003 (`clubk2k3`) has no card setting and plays once the BIOS SYSTEM ASSIGNMENTS are set (Caution 51). The prize versions wait for a hopper, and Club Kart for Cycraft waits for its motion-seat PC; neither is emulated.
- Some games stop on a Naomi "Caution" or "Error" screen on first boot. So far these look like settings (for example a game region that doesn't match the BIOS, or the 31 kHz monitor setting), not graphics faults.

## Download

A Windows x64 build of this branch (Naomi driver only, `naomi.exe`) is on the [naomi-play-latest release](https://github.com/MrBaristaNZ/mame/releases/tag/naomi-play-latest). It's rebuilt automatically each time this branch changes. It contains MAME only, no ROMs.

## Building

Standard MAME build. For a quicker build with just Naomi:

```
make SUBTARGET=naomi SOURCES=src/mame/sega/naomi.cpp
```

## Testing and reports

There are far more Naomi games than I can test alone, so reports are very welcome. Open an issue at https://github.com/MrBaristaNZ/mame/issues with the set name, what's wrong and when (attract, a menu, which stage), and a screenshot if you can. Please say which commit you built, and don't attach or link ROMs.

## Status in MAME

Merged: texture filtering around the texel centres (#16953), ROM board DMA at a bus rate instead of a disc drive's (#16958, ends mvsc2's 10 s white screen after VERSUS).
Open: back-face culling (#16875), drawing by the hardware's 32x32 tiles on several threads with the user tile clip (#16876, also fixes #8997), alpha blending fixes (#16928), 16-bit texture coordinates (#16962), SH-4 operand cache RAM layout (#16963).
Ready, not opened yet (fork branches): `powervr2-texture-pages`, `powervr2-span-colours`, `powervr2-modifier-volumes`, `powervr2-translucent-sort`, `naomi-arm7-sound-ram`.
Each is its own commit here; a commit with an "Adapted for naomi-play" line was changed a little to fit with the others.

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
