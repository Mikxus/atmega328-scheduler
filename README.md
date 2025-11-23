# atmega328-scheduler v0.1.3
scheduler for atmega328p. Hobby project.
## Architecture
Notes:
- Preemptive scheduler (timer0 interrupt based)
- task CPU context sould be saved on the stack maybe? (need to research more)
  - Or maybe in per task struct?
- Task slice is 1 ms (not configurable without hassle)
- Maybe round robin scheduling

## Features
- Fully working test suite with simavr 
- No preexisting HAL
- UART driver

## Getting started
Dependencies:
* avr-gcc
* gcc
* cmake
* libelf (simavr)
* libdwarf (simavr)

Clone the repository and cd in to the repo
```bash
git clone https://github.com/Mikxus/atmega328-scheduler.git --recurse-submodules; cd atmega328-scheduler
```

```bash
mkdir build
```

Generate out of source build system with cmake
```bash
cmake -Bbuild/release -DCMAKE_BUILD_TYPE=Release
# Or debug build
cmake -Bbuild/debug -DCMAKE_BUILD_TYPE=Debug
```

To build the project as release or debug:

```bash
cmake --build build/release --config CMAKE_BUILD_TYPE=Release
 
cmake --build build/debug --config CMAKE_BUILD_TYPE=Debug
```

### Running tests
**Note**: these commands need to be run at the root of the project

Symlink AVR's header files to simavr since it doesn't come with them.
```bash
ln -s /usr/avr/include/avr/ submodules/simavr/simavr/cores/avr
```

Build the project
```bash
cmake --build build/debug --config CMAKE_BUILD_TYPE=Debug
```
Export LD_LIBRARY_PATH to find simavr's shared library

**Note**: change the path if you are not on x86_64-pc-linux-gnu
```bash
export LD_LIBRARY_PATH=/lib:$(pwd)/submodules/simavr/simavr/obj-x86_64-pc-linux-gnu/
```

Now you can run the tests with:
```bash
ctest --test-dir build/debug --rerun-failed --output-on-failure
```

## License
This library is under the [MIT license](https://github.com/Mikxus/atmega328-scheduler/blob/main/LICENSE) except the test folder. The code under test folder is licensed under [GPL v3.0](https://github.com/Mikxus/atmega328-scheduler/blob/main/test/LICENSE) since it uses simavr's source code.
