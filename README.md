# MaxMSP BLE

A BLE Object for Max

## Install

-   Add External [macOS: `.mxo`, Windows: `.mxe64`](https://github.com/mhamilt/maxmsp-ble/releases/download/0.1.0-alpha/max-ble.mxo.zip) to `~/Documents/Max 8/Library` or define your own folder and add it to the [Search Path](https://docs.cycling74.com/max8/vignettes/search_path)

**OR**

-   Add [the package folder](https://github.com/mhamilt/maxmsp-ble/releases/download/0.1-alpha.6/max-ble.zip) to `~/Documents/Max 8/Packages`

## Building

### Xcode

All things being well, you should be able to [open and build the Xcode project directly from this repo](xcode://clone?repo=https%3A%2F%2Fgithub.com%2Fmhamilt%2Fmaxmsp-ble)

The Xcode project performs a couple of extra steps under the hood. These steps live in different locations and are listed below

-   The `.mxo` builds to `max-package/max-ble/externals` which lives in this repository
    -   `max-package/max-ble/externals` is contain in the environment `$DSTROOT`
    -   `$DSTROOT` is defined within [`src/maxmspsdk.xcconfig`](src/maxmspsdk.xcconfig)
-   A post-build script in Build Phases performs the folowing steps
-   `.mxo` is signed
-   the package in [`max-package`](max-package/) is copied to the user max directory `~/Documents/Max 8/Packages/`
-   the package in [`max-package`](max-package/) is zipped in the same directory ready for upload as a release or for distribution.

### Visual Studio

The Visual Studio Solution tracks the projects in the `vs` directory. You can [open and build the solution directly from the repository](git-client://clone?repo=https%3A%2F%2Fgithub.com%2Fmhamilt%2Fmaxmsp-ble)

The `max-ble.vcxproj` builds the `.mxe` and `.mxe64` directly to the directory `%UserProfile%\Documents\Max 8\Externals\`. The build destination is defined in the [`vs/maxmsp-ble/max_extern_common.props`](vs/maxmsp-ble/max_extern_common.props) file under the `OutDir` tag.

### Change sdk Version

You can change the max-sdk version with `git checkout`:

```sh
cd max-sdk
git checkout v7.0.3 # or v7.1.0 v7.3.3 v8.0.3
```

You can check the [max-sdk releases for more tags](https://github.com/Cycling74/max-sdk/releases).

* * *

## Usage

For usage of the external, see the package [README](max-package/max-ble/README.md) or the object reference page in Max.

* * *

## Contribution

If you have any suggestions on interface or functionality, please feel free to [flag an issue](issues/new)
