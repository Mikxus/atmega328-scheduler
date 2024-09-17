# atmega328-scheduler v0.1.3
scheduler for atmega328p. Hobby project.

## Getting started

Clone the repository and cd in to the repo
```bash
git clone https://github.com/Mikxus/atmega328-scheduler.git --recurse-submodules; cd atmega328-scheduler
```

Create build directory
```bash
mkdir build
```

Generate out of source build system with cmake
```bash
cmake -Bbuild -S. 
```
Now you can build the project inside the build folder, using your preferred build system.
For example.
```bash
make
```

### Running tests
Symlink AVR's header files to simavr since it doesn't come with them.
```bash
ln -s /usr/avr/include/avr/ submodules/simavr/simavr/cores/avr
```

Install simavr to run the tests.
**Remember to run the make commands inside the build directory**
```bash
make simavr-install
```
Now you can run the tests.
```bash
make test
```

## License
This library is under the [MIT license](https://github.com/Mikxus/atmega328-scheduler/blob/main/LICENSE) except the test folder. The code under test folder is licensed under [GPL v3.0](https://github.com/Mikxus/atmega328-scheduler/blob/main/test/LICENSE) since it uses simavr's source code.
