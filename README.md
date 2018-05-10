# OpenSWE1R
*An Open-Source port of the 1999 Game ["Star Wars: Episode 1 Racer"](https://en.wikipedia.org/wiki/Star_Wars_Episode_I:_Racer)*

OpenSWE1R is a number of projects related to "Star Wars: Episode 1 Racer".
We foster a community which focuses on understanding and enhancing the original game.

Our core-project is an Open-Source port of the game to the C programming language.

---

[![Gitter Chat](https://img.shields.io/badge/Gitter-Chat-red.svg)](https://gitter.im/OpenSWE1R)
[![CLA assistant](https://cla-assistant.io/readme/badge/OpenSWE1R/openswe1r)](https://cla-assistant.io/OpenSWE1R/openswe1r)
[![AppVeyor CI](https://img.shields.io/appveyor/ci/JayFoxRox82949/openswe1r/master.svg?label=AppVeyor%20CI)](https://ci.appveyor.com/project/JayFoxRox82949/openswe1r/branch/master)
[![Travis CI](https://img.shields.io/travis/OpenSWE1R/openswe1r/master.svg?label=Travis%20CI)](https://travis-ci.org/OpenSWE1R/openswe1r)

---

<p align="center">
<img alt="Screenshots of OpenSWE1R running on Linux" src="http://jannikvogel.de/scratchpad/openswe1r.svg" />
</p>

---

### Community

OpenSWE1R hosts [a chat on gitter.im](https://gitter.im/OpenSWE1R).

You only need a GitHub or Twitter account to access the chat.

There is also an [IRC bridge](https://irc.gitter.im/) available.

The chat is used discuss all OpenSWE1R projects, but we also talk about other aspects of "Star Wars: Episode 1 Racer".

---

### Projects

To make the Open-Source port possible, we have to do a lot of research and reverse engineering.
Therefore OpenSWE1R is split into smaller side-projects and repositories.

Most research and reverse engineering happens in the [swe1r-re repository](https://github.com/OpenSWE1R/swe1r-re).
Based on our research, we also work on tools which can be found in the [swe1r-tools repository](https://github.com/OpenSWE1R/swe1r-tools).

To see which side-projects we currently work on, check out the [list of projects](https://github.com/orgs/OpenSWE1R/projects).

We also work with [other resources and projects](https://github.com/OpenSWE1R/openswe1r/wiki/Useful-Resources).

#### Emulation

OpenSWE1R contains emulation which is a mix of console emulation and something like [WINE](https://www.winehq.org/).
We have implemented emulation of the x86 CPU and Windows functionality used by the game.
This emulation makes the game's PC version work on almost any platform.

You still need the original game files to run the game with our emulation.

The emulation is part of the core-project and contained in this repository.

Check out the [related project tracker](https://github.com/orgs/OpenSWE1R/projects/1) to follow our progress.

#### Rewrite

OpenSWE1R aims to replace all of the original game code with Open-Source code.
This is similar to the approach used by [OpenRCT2](https://openrct2.website/).
The goal is to rewrite the PC version of the game ("swep1rcr.exe") under an Open-Source license.
It's possible that support for other versions (Mac / Dreamcast / N64) will be added later.

As this only replaces the code, you still need the original game for its art assets: levels, sounds, ...

The rewrite is part of the core-project and contained in this repository.

Check out the [related project tracker](https://github.com/orgs/OpenSWE1R/projects/2) to follow our progress.

---

## Requirements

To use our Open-Source port, you will need the following software:

**System**

* OpenGL 3.3 Core

**Toolchain**

* [git](https://git-scm.com/)
* [CMake](https://cmake.org/)
* C11 toolchain

**Libraries**

* [Unicorn-Engine](http://www.unicorn-engine.org/)
* [SDL2](https://www.libsdl.org/)
* [GLEW](http://glew.sourceforge.net/)
* [OpenAL](https://www.openal.org/)

## Getting Started

Read our ["Getting Started" guide on the wiki](https://github.com/OpenSWE1R/openswe1r/wiki/Getting-Started) to learn how to build and run OpenSWE1R.

## Contribute

Due to the large scope of the project, there are various tasks which can be worked on.
Some of these tasks are very suitable for beginners and don't involve writing any code at all.
Experienced developers can help by reviewing or submitting Pull-Requests.

[Please join our gitter.im chat if you want to help](https://gitter.im/OpenSWE1R)!

If you want to contribute, you'll have to [sign our Contributor License Agreement (CLA)](https://cla-assistant.io/OpenSWE1R/openswe1r).
The CLA allows us to easily switch to other [licenses the FSF classifies as Free Software License](https://www.gnu.org/licenses/license-list.html) and which are [approved by the OSI as Open Source licenses](https://opensource.org/licenses), if the need should ever arise ([more information](https://github.com/OpenSWE1R/openswe1r/pull/95)).

---

**© 2017 - 2018 OpenSWE1R Maintainers**

The source code in this repository licensed under GPLv2 or any later version.
Binaries which link against the default Unicorn-Engine backend must be licensed under GPLv2.

OpenSWE1R is not affiliated with, endorsed by, or sponsored by The Walt Disney Company, Twenty-First Century Fox, the games original developers, publishers or any of their affiliates or subsidiaries.
All product and company names are trademarks™ or registered® trademarks of their respective holders. Use of them does not imply any affiliation with or endorsement by them.

Reverse engineering of the original software is done to achieve interoperability with other computing platforms.
In the process, excerpts of the reverse engineered source code might be shown for educational purposes.

No copyright infringement is intended at any stage during development of OpenSWE1R.
