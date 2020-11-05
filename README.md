# MaxMSP BLE

A BLE Object for Max

## Setup

### Cloning this Repository

To make this repository a little more portable, the max-sdk has been included as a submodule. What this means is that you will need to state `--recurse-submodules` when cloning.

Clone this repo into your Max folder's `developing` directory

```sh
cd "/path/to/Max Folder/developing"
git clone --recurse-submodules REPOSITORY_URL
```

You should be able to build straight away from the Xcode project contained in the `xcode` directory.

### Change sdk Version

```sh
cd max-sdk
git checkout v7.0.3 # or v7.1.0 v7.3.3 v8.0.3
```

## Projects

- Xcode
- VS
