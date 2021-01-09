# OSC Gateway

OSC Gateway allows one to map a list of OSC messages to commands to be executed
on a given computer (server).

## Installation

### Prerequisites

* [libosc++](https://github.com/dimitry-ishenko-casparcg/liboscpp) >= 0.8

Follow installation instructions on their home page. `libosc++-dev` package is
only needed when compiling oscgard from source.

### Binary

Debian/Ubuntu/etc:

```shell
$ ver=0.0
$ url=https://github.com/dimitry-ishenko-casparcg/oscgard/releases/download/v${ver}
$ wget ${url}/oscgard_${ver}_amd64.deb
$ sudo apt install ./oscgard_${ver}_amd64.deb
```

RaspberryPi:

```shell
$ ver=0.0
$ url=https://github.com/dimitry-ishenko-casparcg/oscgard/releases/download/v${ver}
$ wget ${url}/oscgard_${ver}_armhf.deb
$ sudo apt install ./oscgard_${ver}_armhf.deb
```

### From source

Stable version (requires [CMake](https://cmake.org/) >= 3.1):

```shell
$ ver=0.0
$ wget https://github.com/dimitry-ishenko-casparcg/oscgard/archive/v${ver}.tar.gz
$ tar xzf v${ver}.tar.gz
$ mkdir oscgard-${ver}/build
$ cd oscgard-${ver}/build
$ cmake ..
$ make
$ sudo make install
```

Latest master (requires[git](https://git-scm.com/) and
[CMake](https://cmake.org/) >= 3.1):

```shell
$ git https://github.com/dimitry-ishenko-casparcg/oscgard.git
$ mkdir oscgard/build
$ cd oscgard/build
$ cmake ..
$ make
$ sudo make install
```

## Authors

* **Dimitry Ishenko** - dimitry (dot) ishenko (at) (gee) mail (dot) com

## License

This project is distributed under the GNU GPL license. See the
[LICENSE.md](LICENSE.md) file for details.
