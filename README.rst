.. Copyright (c) 2026 Yuancheng Li
.. SPDX-License-Identifier: Apache-2.0

ESP32-C6 Boids Sim
==================

A C++ boids simulation for the ESP32-C6 built with Zephyr RTOS and the Jet 3D
rendering library. The firmware simulates flocking agents and renders them on
an SPI-connected ILI9341 TFT display.

Target Hardware
---------------

* ESP32-C6 DevKitC, high-performance core
* SparkFun COM-28379 3.2 inch 320x240 ILI9341 TFT LCD, connected over SPI

Dependencies
------------

Jet is included as a Git submodule. Initialize it after cloning::

   git submodule update --init --recursive

Building
--------

Build with the Zephyr virtual environment executable directly::

   C:\Users\cheer\zephyrproject\.venv\Scripts\west.exe build -b esp32c6_devkitc/esp32c6/hpcore .

Project Structure
-----------------

Application features are grouped under ``src/features``. The entry point in
``src/main.cpp`` only initializes and delegates to those features.

Current features:

* ``src/features/boids`` owns flock simulation.
* ``src/features/game`` owns the main update/render loop.
* ``src/features/rendering`` owns Jet scene setup and display upload.

Configuration
-------------

Zephyr options are stored in ``prj.conf``. Board-specific device-tree settings
are stored in ``boards/esp32c6_devkitc_esp32c6_hpcore.overlay``. Jet renderer
options are owned by this frontend in ``src/config/JetConfig.hpp``.

Display Configuration
---------------------

The ILI9341 is configured through Zephyr's MIPI DBI over SPI driver:

* ``compatible = "zephyr,mipi-dbi-spi"`` uses SPI as the physical bus.
* ``spi-dev = <&spi2>`` selects the ESP32-C6 SPI peripheral.
* ``mipi-max-frequency`` on the display node sets the SPI transfer clock used
  by this MIPI DBI path.
* ``pixel-format = <PANEL_PIXEL_FORMAT_RGB_565>`` uses 16-bit RGB565 pixels.

RGB565 is used because Jet renders RGB565 natively and it minimizes SPI
bandwidth compared with 18-bit color. A full 240x320 RGB565 upload is
153,600 bytes per frame.

Rendering Notes
---------------

Jet renders into a half-width framebuffer to save SRAM. The display upload
path expands each stored pixel horizontally before sending it to the LCD.

To reduce SPI traffic, the renderer keeps a previous framebuffer and uploads
only dirty horizontal spans after the first full-screen frame. This follows the
same basic idea used by fast ILI9341 drivers such as ``fbcp-ili9341``: avoid
transmitting unchanged pixels whenever possible.

At the currently configured 20 MHz SPI clock, full-screen 60 FPS RGB565 upload
is not realistic. Smooth output depends on reducing transferred pixels through
dirty updates, lower output resolution, interlacing, or a faster validated SPI
clock.

References
----------

* Jet source and examples: ``libs/Jet``
* ILI9341 datasheet: https://cdn-shop.adafruit.com/datasheets/ILI9341.pdf
* ILI9341 dirty-update reference: https://github.com/juj/fbcp-ili9341

License
-------

Licensed under the Apache License 2.0. See ``LICENSE``.
