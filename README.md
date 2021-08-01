# OSC Gateway

**oscgard** is an OSC gateway, which allows one to map OSC messages to actions
to be executed by the computer running the gateway (a.k.a. the OSC server).

By default **oscgard** will listen for messages on IP address `127.0.0.1` and
port `6260`. These can be overridden with the `--address` and `--port` options
respectively.

Path to file containing list of actions to be executed must be specified as the
one and only positional argument.

**oscgard** can run either as user or system [systemd](https://systemd.io/)
service. When running as user service, the recommended path for the actions file
is `$HOME/.config/oscgard/actions.conf`. For system service, the recommended
path is `/etc/oscgard/actions.conf`.

The actions file consists of simple `<address> = <action>` entries, where
`<address>` is an address pattern (in regex form) against which incoming
messages are compared, and `<action>` is a command to be executed in case of a
match. For example:

```ini
# actions.conf

# control motor speed
/set/motor/speed = set-speed.py {0}

# turn on/off LEDs: {0} = "on" or "off"
# NB: led.sh will receive /set/led/<N>/state as the first argument
# and will strip N out of it
/set/led/.*/state = /home/pi/bin/led.sh {A} {0}

# set volume level in dB: {0} = Lch , {1} = Rch
/set/volume/level = set-volume {0} {1}

# display message
/display/info  = zenity --info  --text="{0}"
/display/error = zenity --error --text="{0}"
```

Entries are compared in the order they appear in the actions file, and each
matching entry's action is executed asynchronously via `fork/exec`.

Empty lines and lines starting with `#` are ignored.

Each `<action>` consists of an optionally quoted path to an executable file,
followed by zero or more arguments. If the path does not contain any slash (`/`)
characters, it will be searched for using the `PATH` environment variable.
Arguments may likewise be quoted and can contain special tokens, such as:

- `{A}`, which will be replaced with the value of the matched address; and

- `{0}`, `{1}`, ..., `{n}`, which will be replaced with corresponding values of
the received message.

Messages received with time tag in the future (i.e., in an OSC bundle) will have
their actions scheduled for execution at the appropriate time. In order to
cancel these actions prior to their execution, one can send a special `/cancel`
message containing time tag of the action(s) to cancel.

## Usage

As mentioned above, **oscgard** can run as either user or system service.

User service will run with given user privileges and will enable one to execute
actions within the user's environment. To set up user service, perform the
following steps:

1. Create and edit `actions.conf`:

   ```shell
   mkdir -p ~/.config/oscgard
   editor ~/.config/oscgard/actions.conf
   ```

2. Change address and/or port to listen on:

   ```shell
   systemctl --user edit oscgard.service
   ```

   Edit the override file, e.g.:

   ```ini
   [Service]
   Environment="args=--address=0.0.0.0 --port=1234 %h/.config/oscgard/actions.conf"
   ```

   NB: The above will make **oscgard** listen on port `1234` on __all__ IP
   addresses and has potential security implications.

3. Enable and start user service:

   ```shell
   systemctl --user enable oscgard.service
   systemctl --user start oscgard.service
   ```

System service will run with root privileges and will be able execute
system-wide actions. To set up system service, perform the following steps:

1. Create and edit `actions.conf`:

   ```shell
   mkdir -p /etc/oscgard
   editor /etc/oscgard/actions.conf
   ```

2. Change address and/or port to listen on:

   ```shell
   sudo systemctl edit oscgard.service
   ```

   Edit the override file, e.g.:

   ```ini
   [Service]
   Environment="args=--address=0.0.0.0 --port=1234 /etc/oscgard/actions.conf"
   ```

   NB: The above will make **oscgard** listen on port `1234` on __all__ IP
   addresses and has potential security implications.

3. Enable and start the service:

   ```shell
   sudo systemctl enable oscgard.service
   sudo systemctl start oscgard.service
   ```

Share and enjoy.

## Installation

### Binary

Requires [libosc++](https://github.com/dimitry-ishenko-cpp/liboscpp) >= 1.0.

Debian/Ubuntu/etc:

```shell
$ p=oscgard
$ v=1.1
$ wget https://github.com/dimitry-ishenko-casparcg/${p}/releases/download/v${v}/${p}_${v}_amd64.deb
$ sudo apt install ./${p}_${v}_amd64.deb
```

RaspberryPi:

```shell
$ p=oscgard
$ v=1.1
$ wget https://github.com/dimitry-ishenko-casparcg/${p}/releases/download/v${v}/${p}_${v}_armhf.deb
$ sudo apt install ./${p}_${v}_armhf.deb
```

### From source

Stable version (requires [CMake](https://cmake.org/) >= 3.1,
[asio](https://think-async.com/Asio/) and
[libosc++-dev](https://github.com/dimitry-ishenko-cpp/liboscpp) >= 1.0):

```shell
$ p=oscgard
$ v=1.1
$ wget https://github.com/dimitry-ishenko-casparcg/${p}/archive/v${v}.tar.gz
$ tar xzf v${v}.tar.gz
$ mkdir ${p}-${v}/build
$ cd ${p}-${v}/build
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
