plasma-workspace is based on plasma-workspace [gitlab](https://invent.kde.org/plasma/plasma-workspace) 

## Features

* plasma-work is a touch friendly and beautiful application. It offers:

* Run on JingOS platform

* Brand new UI & UE with JingOS-style , based on JingUI Framework

* Support keyboard & touchpad & mouse & screen touch

* All keys support pressed / hovered effects

* Well-designed interface material:

  * Font
  * Icon
  * Picture

## Links

* Home page: https://www.jingos.com/

* Project page: https://invent.kde.org/jingosdev/plasma-workspace

* Issues: https://invent.kde.org/jingosdev/plasma-workspace/issues

* Development channel: https://forum.jingos.com/


## Dependencies

* Qt5 

* Cmake

* KI18n

* Kirigami (JingOS Version)

* DBusAddons

* PlasmaQuick

* KWorkspace

* DBus

* KWorkspace

* ConfigCore



## Build

To build plasma-work from source on Linux, execute the below commands.



### Compile

```sh
cd plasma-work
mkdir build
cd build
cmake ..
make -j$(nproc)
```


#### Run

```
bin/plasmashell
```
 

#### Install

```
sudo make install
```