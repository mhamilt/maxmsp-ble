# maxmsp-external-template
Template Projects for building external objects in MaxMSP

## Cloning this Repository

To make this repository a little more portable, the max-sdk has been included as a submodule. What this means is that you will need to state `--recurse-submodules` when cloning.

Clone this repo into your Max folder's `developing` directory

```sh
cd "/path/to/Max Folder/developing"
git clone --recurse-submodules https://github.com/mhamilt/maxmsp-external-template.git
```

You should be able to build straight away form the xcode project contained in the `xcode` directory.

## Projects

The xcode project has multiple targets to demonstrate how to approach building a maxMSP external `.mxo` file.

### max-external

The vanilla C approach. There are plenty of examples in the sdk, but this target aims to take a more modern approach.  Documentation has been added inline as well as some slightly less obtuse variable name choices which will hopefully reduce the barrier of entry.

### mspcpp

If you have already coded a ton of DSP C++ classes for something like JUCE, you may want to start here.

Demonstrates using `extern "C"` to write wrappers for pre-existing c++ classes. you need only write wrapper functions for public class methods or anything that needs to be called directly in `C`
