# Zeitgeist Core
Implementation of the application layer protocol Zeitgeist.

This repository includes both the client and server system-level implementations of Zeitgeist along with unit tests and documentation on how the protocol works.

## Build
Since this repository only features a core implementation, the main builds include memory safety checks and no compiler optimization.

When running `make` a `build` and `bin` directories are created. The `build` directory contains the object files (`.o`) and `bin` conatains the final linked binaries.

Use the following command to compile using Makefile:
```bash
make
```
See `Makefile` to read or modify compilation flags utilized and all the available commands.

## Tests
Unit tests are implemented with custom macros that run different test functions and detail failures. To run tests first build them with the tests entry point and run the output binary `protocol_tests`:
```bash
make
bin/protocol_tests
```
Note: a specialized library for C testing was not utilized to keep the codebase and dependencies as simple as possible.
