Enemy Territory: Legacy [![Build Status](https://travis-ci.org/etlegacy/etlegacy.png?branch=master)](https://travis-ci.org/etlegacy/etlegacy) [![Analysis Status](https://scan.coverity.com/projects/1160/badge.svg)](https://scan.coverity.com/projects/1160)
==========

*A second breath of life for Wolfenstein: Enemy Territory*

* Website: [http://www.etlegacy.com](http://www.etlegacy.com)
* Release downloads: [http://dev.etlegacy.com/download](http://dev.etlegacy.com/download)
* Wiki/FAQ: [http://dev.etlegacy.com/projects/etlegacy/wiki](http://dev.etlegacy.com/projects/etlegacy/wiki)
* Forums: [http://dev.etlegacy.com/projects/etlegacy/boards](http://dev.etlegacy.com/projects/etlegacy/boards)
* Development (bug reports and feature requests): [http://dev.etlegacy.com](http://dev.etlegacy.com)
* Repository: [https://github.com/etlegacy/etlegacy](https://github.com/etlegacy/etlegacy)
* Translation: [https://www.transifex.com/projects/p/etlegacy/](https://www.transifex.com/projects/p/etlegacy/)
* IRC: [\#etlegacy](http://webchat.freenode.net/?channels=#etlegacy) on irc.freenode.net

INTRODUCTION
============

Enemy Territory: Legacy is based on the [raedwulf-et](https://bitbucket.org/tcmreastwood/raedwulf-et/)
project which in turn is based on the GPL'd source code of Wolfenstein: Enemy Territory.

The main goals of the project are fixing bugs, cleaning up the codebase and adding useful features
while remaining compatible with the ET 2.60b version.

The Legacy mod is the default mod shipped with ET: Legacy. It aims to add many useful features and
improvements, while staying close to the original gameplay, as well as being lightweight and fully
extensible through Lua scripts.

For more information consult our [changelog](http://dev.etlegacy.com/projects/etlegacy/wiki/Changelog).

ET: Legacy development is a collaborative effort done in an open, transparent and friendly manner.
Anyone is welcome to join our efforts!

GENERAL NOTES
=============

Game data
-----------------------------------------------------------------------------

Wolfenstein: Enemy Territory is a free release, and can be downloaded from [Splash Damage](http://www.splashdamage.com/content/download-wolfenstein-enemy-territory).

This source release contains only the engine and mod code but not any game data,
which is still covered by the original EULA and must be obeyed as usual.

In order to run ET: Legacy you will need to copy the original assets files
(*pak0.pk3*, *pak1.pk3* and *pak2.pk3*) to the etmain folder.

Compatibility with Enemy Territory 2.60b
----------------------------------------------------------------------------

ET: Legacy remains compatible with the ET 2.60b version as much as possible.

Please note that ET: Legacy is *not* compatible with PunkBuster enabled servers.
ET: Legacy clients also cannot connect to servers running the ETPro mod.

Linux 64 bit
------------

Please remember that 64 bit ET: Legacy clients can only connect to servers running
mods providing a 64 bit version. You will be able to play 32 bit-only mods only if
you compile ET: Legacy on a 32 bit system or crosscompile it for 32 bit architecture
on a 64 bit system.

At the moment, only the Legacy mod is available in 64 bit version, while all other
existing mods are available in 32 bit only version.

In case you are a running a 64 bit system, you probably might want to use the
**bundled libraries** which are located in a separate *etlegacy-libs* repository and
can be automatically downloaded using the `git submodule` command. See the next
section for more details.

Dependencies
-----------------------------------------------------------------------------

* **CMake** (compile-time only)
* **OpenGL**
* **GLEW** version 1.10
* **SDL** version 2.0.3
* **libjpeg-turbo** version 1.3, or **libjpeg** version 8
* **libcurl** (optional, enabled by default)
* **Lua** version 5.2 (optional, enabled by default)
* **Ogg Vorbis** (optional, enabled by default)
* **Freetype** version 2 (optional, enabled by default)
* **OpenAL** (optional)
* **Jansson** (optional)

To get the latest source code install [git](http://git-scm.com/) and
clone our repository hosted at [Github.com](https://github.com/etlegacy/etlegacy):

    $ git clone git://github.com/etlegacy/etlegacy.git

If the required dependencies are not installed on your system run:

    $ git submodule init
    $ git submodule update

This downloads the essential dependencies into the `libs/`directory. You can choose
whether to use bundled libraries instead of the system ones by changing the
`BUNDLED_LIBS` variable in the CMakeList.txt configuration file. You can then select
which bundled libraries to use by toggling the respective `BUNDLED_XXX` variable.

Compile and install
-----------------------------------------------------------------------------

To install the binaries system-wide, you need to compile ET: Legacy with hardcoded
fs_basepath.

The following variables can be adjusted in CMake:

  * **INSTALL_DEFAULT_BASEDIR**: sets default *fs_basepath*, i.e. where etl and etlded
    executables look for data files. In most cases it is CMAKE_INSTALL_PREFIX+INSTALL_DEFAULT_MODDIR.
    Defaults to empty value, because we want *fs_basepath* to be the current working directory
    when not installing the game system-wide.

  * (optional) **INSTALL_DEFAULT_BINDIR**: Location for executables. Appended to CMAKE_INSTALL_PREFIX.
    Defaults to "bin".

  * (optional) **INSTALL_DEFAULT_MODDIR**: Location for libraries and paks. Appended to
    CMAKE_INSTALL_PREFIX. Defaults to "share/etlegacy" and then "legacy" is appended to it.


### Linux

* option A: **command line**

In terminal, run:

    $ mkdir build && cd build && cmake ..

To compile, run:

    $ make

If you wish to install ET: Legacy system-wide, run:

    # make install

Be sure to set the CMake variables (see above) beforehand.

* option B: **easybuild**

In terminal, run:

    $ ./easybuild.sh

To install, run:

    $ cd build
    $ make install

ET: Legacy will be installed in `~/etlegacy`. To change it, set the CMake variables
(see above) in the easybuild.sh file beforehand.

**NOTE:**

  * Even if you have a 64 bit linux distribution which provides 32 bit versions of all
  the required libraries, you might also need the development libraries (-devel packages)
  installed on your system.

  * In order to compile the jpeg-turbo library properly you will need the **nasm** assembler.


### Crosscompiling on Linux with MinGW-w64

In terminal, run:

    $ mkdir build && cd build
    $ cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-cross-mingw-linux.cmake ..
    $ make

By default, MinGW name is set to *i686-w64-mingw32*. You may have to change it in
`cmake/Toolchain-cross-mingw-linux.cmake` depending on how it is called on your system.


### Windows

* option A: **Visual Studio**

    1. install the free [Visual Studio](http://www.visualstudio.com/) (C++ Express 2010 or newer)
    2. install [CMake](http://www.cmake.org/) and make sure it is added to your system PATH
    3. create a `build` directory inside the directory which contains ET: Legacy sources
    4. open *Visual Studio Command Prompt* in the Start menu
    5. change directory with `cd` to the newly created build directory

In the command prompt, run:

    cmake -G "NMake Makefiles" -DBUNDLED_LIBS=YES .. && nmake

or

    cmake -G "Visual Studio 10" -DBUNDLED_LIBS=YES ..

and open the resulting project in Visual Studio.

* option B: **QtCreator**

    1. install the free [QtCreator](http://www.qt.io/download-open-source/)
    2. install [CMake](http://www.cmake.org/) and make sure it is added to your system PATH
    3. open the CMakeLists.txt file in QtCreator.

* option C: **easybuild**

    1. install the free [Visual Studio](http://www.visualstudio.com/) (C++ Express 2010 or newer)
    2. install [CMake](http://www.cmake.org/) and make sure it is added to your system PATH
    3. run easybuild.bat

ET: Legacy will be installed in `My Documents\ETLegacy-Build`. To change it, set the CMake variables
(see above) in the easybuild.bat file beforehand.

**NOTES:**

  * If compilation of bundled libraries is aborted for any reason, you will probably need to clean the
  libs directory and start over. This can be done by executing `git clean -df && git reset --hard HEAD`
  inside `libs/` directory.

  * If the build fails during libcurl compilation because of missing *sed* utility, download it from
  [GnuWin](http://gnuwin32.sourceforge.net/packages/sed.htm) and place it into your system path or
  copy it into `MSVC/VC/bin`. It also comes with Git and can be placed into your system path
  automatically if you select that option during Git installation.

  * In order to compile the jpeg library properly there is a need for a file named 'win32.mak'.
  Unfortunately this file isn't shipped with Windows 8.0 and 8.1 SDK versions.
  Solution: Get the Windows SDK 7 and copy 'win32.mak' to `libs/jpeturbo/`.


### Mac OS X

* option A: **command line**

TBD

* option B: **easybuild**

In Terminal, run:

    $ ./easybuild.sh
    $ cd build && make install

This will put an 'etlegacy' folder into your user folder.

**NOTE**:

In the legacy mod folder, the cgame_mac and ui_mac files are redundant since they are in the
etl_bin.pk3 and will be extracted at runtime, so you can delete those. The client is named etl.app
(and can safely be renamed), while the dedicated server is just a command-line binary named "etlded".



LICENSE
=======

Enemy Territory: Legacy
-----------------------------------------------------------------------------

Wolfenstein: Enemy Territory GPL Source Code
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.

OpenWolf GPL Source Code
Copyright (C) 2011 Dusan Jocic

ET: Legacy
Copyright (C) 2012-2015 Jan Simek <mail@etlegacy.com>

  ET: Legacy is free software: you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or (at your option) any later
  version.

  ET: Legacy is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with
  ET: Legacy (see COPYING.txt). If not, see <http://www.gnu.org/licenses/>.

  ADDITIONAL TERMS:  The Wolfenstein: Enemy Territory GPL Source Code is also
  subject to certain additional terms. You should have received a copy of these
  additional terms immediately following the terms and conditions of the GNU GPL
  which accompanied the Wolf ET Source Code.  If not, please request a copy in
  writing from id Software at id Software LLC, c/o ZeniMax Media Inc., Suite 120,
  Rockville, Maryland 20850 USA.

  EXCLUDED CODE:  The code described below and contained in the Wolfenstein:
  Enemy Territory GPL Source Code release is not part of the Program covered by
  the GPL and is expressly excluded from its terms.  You are solely responsible
  for obtaining from the copyright holder a license for such code and complying
  with the applicable license terms.

zLib - general purpose compression library
-----------------------------------------------------------------------------
Copyright (C) 1995-2013 Jean-loup Gailly and Mark Adler

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Jean-loup Gailly        Mark Adler
  jloup@gzip.org          madler@alumni.caltech.edu


  The data format used by the zlib library is described by RFCs (Request for
  Comments) 1950 to 1952 in the files http://tools.ietf.org/html/rfc1950
  (zlib format), rfc1951 (deflate format) and rfc1952 (gzip format).

Minizip - IO on .zip files using portions of zlib
-----------------------------------------------------------------------------

Copyright (c) 1990-2000 Info-ZIP.  All rights reserved.
Copyright (C) 1998-2009 Gilles Vollant

This unzip package allow extract file from .ZIP file, compatible with PKZip 2.04g
WinZip, InfoZip tools and compatible.

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

Decryption code comes from crypt.c by Info-ZIP but has been greatly reduced in
terms of compatibility with older software. The following is from the original
crypt.c. Code woven in by Terry Thorsen 1/2003.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html

  crypt.c (full version) by Info-ZIP.

  The encryption/decryption parts of this source code (as opposed to the
  non-echoing password parts) were originally written in Europe.  The
  whole source package can be freely distributed, including from the USA.
  (Prior to January 2000, re-export from the US was a violation of US law.)

  This encryption code is a direct transcription of the algorithm from
  Roger Schlafly, described by Phil Katz in the file appnote.txt.  This
  file (appnote.txt) is distributed with the PKZIP program (even in the
  version without encryption capabilities).

MD4 Message-Digest Algorithm
-----------------------------------------------------------------------------
Copyright (C) 1991-1992, RSA Data Security, Inc. Created 1991. All rights reserved.

  License to copy and use this software is granted provided that it is identified
  as the "RSA Data Security, Inc. MD4 Message-Digest Algorithm" in all mater
  ial mentioning or referencing this software or this function.

  License is also granted to make and use derivative works provided that such work
  s are identified as "derived from the RSA Data Security, Inc. MD4 Message-Digest
  Algorithm" in all material mentioning or referencing the derived work.

  RSA Data Security, Inc. makes no representations concerning either the merchanta
  bility of this software or the suitability of this software for any particular p
  urpose. It is provided "as is" without express or implied warranty of any
  kind.

MD5 Message-Digest Algorithm
-----------------------------------------------------------------------------

The MD5 algorithm was developed by Ron Rivest. The public domain C language
implementation used in this program was written by Colin Plumb in 1993, no copyright
is claimed.

  This software is in the public domain. Permission to use, copy, modify, and
  distribute this software and its documentation for any purpose and without fee is
  hereby granted, without any conditions or restrictions. This software is provided
  "as is" without express or implied warranty.
