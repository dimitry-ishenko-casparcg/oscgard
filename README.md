# OSC Gateway

OSC Gateway allows one to map a list of OSC messages to commands to be executed
on a given computer (server).

## Installation

### Binary

Requires [libosc++](https://github.com/dimitry-ishenko-cpp/liboscpp) >= 1.0.

Debian/Ubuntu/etc:

```shell
$ p=oscgard
$ v=1.0
$ wget https://github.com/dimitry-ishenko-casparcg/${p}/releases/download/v${v}/${p}_${v}_amd64.deb
$ sudo apt install ./${p}_${v}_amd64.deb
```

RaspberryPi:

```shell
$ p=oscgard
$ v=1.0
$ wget https://github.com/dimitry-ishenko-casparcg/${p}/releases/download/v${v}/${p}_${v}_armhf.deb
$ sudo apt install ./${p}_${v}_armhf.deb
```

### From source

Stable version (requires [CMake](https://cmake.org/) >= 3.1,
[asio](https://think-async.com/Asio/) and
[libosc++-dev](https://github.com/dimitry-ishenko-cpp/liboscpp) >= 1.0):

```shell
$ p=oscgard
$ v=1.0
$ wget https://github.com/dimitry-ishenko-casparcg/${p}/archive/v${v}.tar.gz
$ tar xzf v${v}.tar.gz
$ mkdir ${p}-${ver}/build
$ cd ${p}-${ver}/build
$ cmake ..
$ make
$ sudo make install
```

Latest master (requires [git](https://git-scm.com/),
[CMake](https://cmake.org/) >= 3.1, [asio](https://think-async.com/Asio/) and
[libosc++-dev](https://github.com/dimitry-ishenko-cpp/liboscpp) >= 1.0):

```shell
$ p=oscgard
$ git clone --recursive https://github.com/dimitry-ishenko-casparcg/${p}.git
$ mkdir ${p}/build
$ cd ${p}/build
$ cmake ..
$ make
$ sudo make install
```

## Authors

* **Dimitry Ishenko** - dimitry (dot) ishenko (at) (gee) mail (dot) com

## License

This project is distributed under the GNU GPL license. See the
[LICENSE.md](LICENSE.md) file for details.
