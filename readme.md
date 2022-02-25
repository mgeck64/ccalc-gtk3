# gcalc
## Synopsis
An advanced text-based GUI calculator app. Unlike a typical calculatior apps
that mimics a physical calculator, this app presents a minimalist user
interface with a modest screen footprint.
- Supports complex arithmetic, integer arithmetic and bitwise operations
- Supports implied multiplication (multiplication by justaposition)
- Supports 8, 16, 32, 64 and 128 bit integer types
- Supports binary, octal, decimal and hexadecimal numbers, both integer and
floating point (and complex)
- Supports floating point numbers with 100 decimal significant digits (+ guard
digits)
## Development Notes
This project, developed for Gtk 3 (gtkmm-3), was originally developed for
Gtk 4 (gtkmm-4) but that version is having issues related to Gtk 4 (still new)
so I ported it to work with the more stable Gtk 3 (gtkmm-3). This was developed
under Manjaro 21.2, Gnome edition; basic functionality has been tested on all
other editions of Manjaro 21.2 and on Ubuntu 21.10. (2/25/22)
## Dependencies
This project depends on another of my projects, ccalc-lib; that project provides
the backend to this project's GUI frontend.
- ccalc-lib's header files are assumed to be discoverable in a "ccalc" directory
in the include search path; e.g., /usr/local/include/ccalc.
- The lib files libccalc-rel.a (release library) and libccalc-dbg.a (debugging
library) are assumed to be discoverable in the lib path; e.g., located in
/usr/local/lib.

This project also depends on gtkmm-3 for the GUI frontend.

This project also depends on Boost because ccalc-lib does so; see ccalc-lib for
details.

This project has GNU extensions to C++ enabled because ccalc-lib does so.
## Build Quick Help
- 'make' or 'make install' builds the release executable as described below for
'make release', and then
    - installs the executable under /usr/bin
    - installs the .desktop file under /usr/share/applications
- 'make debug' builds the debug executable in a "debug" directory under the
current working directory
- 'make release' builds the release executable in a "release" directory under
the current working directory
- 'make uninstall'
    - deletes the execuable from /usr/bin
    - deletes the .desktop file from /usr/share/applications
- The makefile provides more make targets and options; peruse it for details