# atmega328-scheduler v0.1.3
scheduler for atmega328p. Hobby project.

## Getting started

Clone the repository and cd in to the repo
```bash
git clone https://github.com/Mikxus/atmega328-scheduler.git --recurse-submodules; cd atmega328p-scheduler
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

## License
This library is under the [MIT license](https://github.com/Mikxus/atmega328-scheduler/blob/main/LICENSE) except the test folder. The code under test folder is licensed under [GPL v3.0](https://github.com/Mikxus/atmega328-scheduler/blob/main/test/LICENSE) since it uses simavr's source code.
