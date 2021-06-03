# MaxMSP BLE

A BLE Object for Max

## Install

Add the file to ~/Documents/Max 8/Library or define your own folder via Search Path

## Building

### Cloning this Repository

First step, open Terminal then make sure you have `git` by typing:

```sh
which git
```

If you get nothing then type:

```sh
xcode-select --install
```

To make this repository a little more portable, the `max-sdk` has been included as a submodule. What this means is that you will need to state `--recurse-submodules` when cloning.

Clone this repo into your Max folder's `developing` directory

```sh
cd "~/Documents/Max 8/"
mkdir developing
cd "developing"
git clone --recurse-submodules https://github.com/mhamilt/maxmsp-ble
```

You should be able to build straight away from the Xcode project contained in the `xcode` directory.

### Change sdk Version

```sh
cd max-sdk
git checkout v7.0.3 # or v7.1.0 v7.3.3 v8.0.3
```
