# atmega328-scheduler v0.3.0

Simple scheduler for atmega328p. Is this the fastest possible? No. I've created this to study how operating systems & schedulers work.

## Features

- Preemptive scheduler
- Custom built test suite with simavr
- Mutex with priority inheritance
- Ipc fifo
- gpio driver
- timer0 driver

### TODO

- Make the default scheduler priority and time slice aware
- Add more core ipc semantics
- implement sleep functionality
- uart output task aware

## Layout

```bash
atmega328-scheduler/
  include/kernel -- Public header files
  src/           -- Core scheduler code
  test/avr       -- Avr test files
  test/native_test_runners -- Avr test file runners
```

## Building the project

Dependencies:
* avr-gcc
* avr-libc (<= 2.2.0)
* gcc
* cmake
* make
* pkg-config
* libelf (simavr)
* libdwarf (simavr)

### Setting up development environment

#### Installing dependencies

Ubuntu/debian:

```bash
sudo apt-get install git gcc-avr avr-libc gcc g++ cmake make libelf-dev libdwarf-dev pkg-config
```

Arch linux:

```bash
sudo pacman -S --needed git avr-gcc avr-libc gcc cmake make libelf libdwarf base-devel
```

#### Building the project

Clone the repository and cd in to the repo

```bash
git clone https://github.com/Mikxus/atmega328-scheduler.git --recurse-submodules; cd atmega328-scheduler
```

Symlink AVR's header files to simavr since it doesn't come with them.

Arch linux:

```bash
ln -s /usr/avr/include/avr/ submodules/simavr/simavr/cores/avr
```

Ubuntu/debian:

```bash
ln -s /usr/lib/avr/include/avr/ submodules/simavr/simavr/cores/avr
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
