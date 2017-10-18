FabricUI Library Source
==========================

Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

Introduction
---------------

Fabric Software makes available the source code for the FabricUI library, used in various parts of Fabric Engine.  The primary purpose of making the source code available is to allow users to rebuild the library in a different environment than the one the Fabric Engine binary distribution assumes.  For instance, FabricUI can be rebuilt against a custom version of Qt.

Licensing
------------

The FabricUI library source is made available under the terms of a new (3-clause) BSD open-source license; see the file `LICENSE.txt` for details.

Prerequisites
----------------

- You must have the C++ compiler toolchain installed for your platform

  - Windows: currently the scripts only build against Visual Studio 2013 (at Fabric we use Visual Studio 2013 Express for Windows Desktop).  However, it should be possible to use a different version of Visual Studio by changing the value of MSVC_VERSION in SConstruct

  - CentOS 6: current Linux looks for a build of gcc located under /opt/centos5/usr/bin/.  However, it should be possible to remove the lines in SConscript that reference these paths to make it instead use the default system compiler (or change them to a different compiler)

  - Mac OS X: the compiler that comes with Xcode since at least 2013, along with the command lines tools package, should work fine

- You must have Qt installed. For C++ applications, any version of Qt4.x or Qt5.x should work. However, for the python bindings, only PySide1 (Qt4.x) currently works, PySide2 (Qt5.x) is not yet supported.

- You must have Python installed to generate the `canvas.py` target (only PySide1 and Python2 are currently supported):
  
  - Windows: Download the latest Python 2.7.x from https://www.python.org/downloads/

  - CentOS 6: install Python 2.6 from the built-in CentOS repository

  - Mac OS X: already comes with Python installed

- You must have SCons installed.  On all platforms, use the either easy_install or pip to install the SCons package for Python. Version 2.5.1 is recommended, but version 3.0.0 should work.

Performing the Build
-----------------------------

You need to have the Fabric environment set up.  Refer to http://docs.fabric-engine.com/FabricEngine/latest/HTML/GettingStartedGuide/baseinstallation.html for instructions on installing Fabric and setting up the environment. 

There are 2 targets :

- The default target (`scons`) only builds the C++ library

- The target `scons canvas.py` will generate Python bindings, usable by the Canvas.py application.

The build scripts will ask for several environment variables :

- `FABRIC_DIR` : the top level of an unpacked Fabric distribution.

- `QT_DIR` : the directory of a Qt distribution (contains `bin/`, `include/`, etc..)

- `SHIBOKEN_PYSIDE_DIR` (optional) : the directory of a PySide(1) distribution. It is only required for the `canvas.py` target, and is not used by the default target (it can be an empty string then).

- `FABRIC_VERSION_MAJ` and `FABRIC_VERSION_MIN` : they should correspond to the version number of `FabricCore-x.x` in `FABRIC_DIR` (so, for example, they should be `2` and `6` respectively for `FabricCore-2.6`)

- `FABRIC_BUILD_OS` = { Windows; Linux; Darwin }, `FABRIC_BUILD_ARCH`= { x86; x86_64 }, `FABRIC_BUILD_TYPE` = { Debug; Release }

After running `scons` or `scons canvas.py` successfully, the resulting distribution will be located in a new directory `FabricUI/stage/`.
