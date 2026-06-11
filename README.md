# MaxMSP BLE

A BLE Object for Max

## Install

Download [the latest release](https://github.com/mhamilt/maxmsp-ble/releases) and add the `max-ble` package folder to your Max Packages directory, for example:

- `~/Documents/Max 8/Packages/`
- `~/Documents/Max 9/Packages/`

The package must include `externals/max-ble.mxo`. If you only copied the examples or docs from the git repository, Max will report `No such object`.

### Apple Silicon (M1/M2/M3 Macs)

The current release archive (`0.1-alpha.6`) ships an **Intel (x86_64)** external. On Apple Silicon you will see `incorrect architecture` unless you build from source (see below).

After building locally, macOS may also block the external with `system security policy` until it is ad-hoc signed. The Xcode post-build script handles this automatically; for a manual install:

```sh
xattr -cr ~/Documents/Max\ 9/Packages/max-ble
codesign --force --deep --sign - ~/Documents/Max\ 9/Packages/max-ble/externals/max-ble.mxo
```

Grant Bluetooth access when prompted (System Settings → Privacy & Security → Bluetooth). Max may be bundled inside Ableton Live rather than installed as a standalone app in `/Applications`.

## Building

### Xcode

Clone with submodules, then open and build the Xcode project:

```sh
git clone --recursive https://github.com/mhamilt/maxmsp-ble.git
# or, after a plain clone:
git submodule update --init --recursive
```

[Open the project in Xcode](xcode://clone?repo=https%3A%2F%2Fgithub.com%2Fmhamilt%2Fmaxmsp-ble) and build the `max-ble` scheme.

The Xcode project performs a couple of extra steps under the hood:

-   The `.mxo` builds under `$DSTROOT` (default: `/tmp/max-ble.dst/max-package/max-ble/externals/`)
-   [`scripts/postbuild-macos.sh`](scripts/postbuild-macos.sh) then:
    -   copies the external into [`max-package`](max-package/)
    -   ad-hoc signs the `.mxo`
    -   zips the package to `max-package/max-ble.zip`
    -   installs into `~/Documents/Max 8/Packages/` and/or `~/Documents/Max 9/Packages/` when those folders exist

### Visual Studio

The Visual Studio Solution tracks the projects in the `vs` directory. You can [open and build the solution directly from the repository](git-client://clone?repo=https%3A%2F%2Fgithub.com%2Fmhamilt%2Fmaxmsp-ble)

The `max-ble.vcxproj` builds the `.mxe` and `.mxe64` directly to the directory `%UserProfile%\Documents\Max 8\Externals\`. The build destination is defined in the [`vs/maxmsp-ble/max_extern_common.props`](vs/maxmsp-ble/max_extern_common.props) file under the `OutDir` tag.

### Change sdk Version

You can change the max-sdk version with `git checkout`:

```sh
cd max-sdk
git checkout v7.0.3 # or v7.1.0 v7.3.3 v8.0.3
```

You can check the [max-sdk-base releases for more tags](https://github.com/Cycling74/max-sdk-base).

* * *

## Usage

For usage of the external, see the package [README](max-package/max-ble/README.md) or the object reference page in Max.

* * *

## Contribution

If you have any suggestions on interface or functionality, please feel free to [flag an issue](https://github.com/mhamilt/maxmsp-ble/issues/new)
