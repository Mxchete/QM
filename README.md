# QM
quine-mccluskey implementation in c++

## Compiling
For compilation, I have provided a start_container.sh script, which uses podman to create a debian 12 container. This container includes all dependencies needed for building & running the project.

To start the container, run:
```
./scripts/start_container.sh
```

Once inside the container, run the following command to build:
```
./scripts/build
```

For building & running outside the container, just ensure that the required dependencies are available and the correct versions or above are available.

Building and running the program on Eustis does work. To build on Eustis, just run the build script without running the start_container script, since podman is not available on Eustis.

- Note: This program was developed & tested on Linux, & there is no guarantee that it will compile on windows or MacOS

## Running
After compiling the program, the binary will appear in the ./bin directory at the project root level.

Run:
```
./bin/qm --help
```

To view a list of program arguments. Most importantly, running:
```
./bin/qm -f path/to/input/file.pla -o path/to/output/file.pla
```

Will run the program reading input from the file at the given path, and printing output to the file at the other given path
