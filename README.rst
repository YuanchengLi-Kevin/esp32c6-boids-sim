.. Copyright (c) 2026 Yuancheng Li
.. SPDX-License-Identifier: Apache-2.0

ESP32-C6 Jet Game
=================

A C++ game for the ESP32-C6 built with Zephyr RTOS and the Jet 3D rendering
library.

Target Hardware
---------------

* ESP32-C6 DevKitC, high-performance core

Dependencies
------------

Jet is included as a Git submodule. Initialize it after cloning::

   git submodule update --init --recursive

Project Structure
-----------------

Application features are grouped under ``src/features``. The entry point in
``src/main.cpp`` only initializes and delegates to those features.

Configuration
-------------

Zephyr options are stored in ``prj.conf``. Board-specific device-tree settings
are stored in ``boards/esp32c6_devkitc_esp32c6_hpcore.overlay``. Jet renderer
options are owned by this frontend in ``src/config/JetConfig.hpp``.

License
-------

Licensed under the Apache License 2.0. See ``LICENSE``.
