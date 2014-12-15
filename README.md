Assorted Utilities
==================


Overview
--------

An assortment of tools and libraries that are too small to warrant having their own repositories.


Directory Structure
-------------------

- **/libs**<br />Libraries
  - libs/**libsimple-base** - the base library utilised by the majority of the tools and other libraries.
  - libs/**libsimplesock** - lightweight socket wrapper library for working with BSD sockets under Unix-like OSes and WinSock under Windows.
- **/tools**<br />Tools
  - tools/**compile_file** - utility to create C-code byte array representations of binary files, suitable for embedding in executables.
  - tools/**mergelib** - utility to merge one or more static libraries into a single output static library, utilising "libtool" under Darwin and "ar" under all other platforms.
  - tools/**splice** - binary file splicer utility.
