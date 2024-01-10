# Tests
Tests are run natively on the host using simavr as an simulator

## How does  the tests work?
Test tests are seperated to two different files 

### MCU files
- These files don't have ```test-``` prefix
- Used to generate output which the native file then tests for

### Native files
- These are run on the host.
- These have ```test-``` prefix
- Uses simavr to run the test. Then determines if the test fails or passes.

# LICENSE
Tests are under the GPL V3.0 since they rely on the simavr source code