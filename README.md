# maxmsp-external-template

Template Projects for building external objects in MaxMSP

<!-- TOC depthFrom:1 depthTo:6 withLinks:1 updateOnSave:1 orderedList:0 -->

- [maxmsp-external-template](#maxmsp-external-template)
	- [Setup](#setup)
	- [Cloning this Repository](#cloning-this-repository)
	- [Configuration](#configuration)
		- [maxmspsdk.xcconfig](#maxmspsdkxcconfig)
		- [Change sdk Version](#change-sdk-version)
	- [Projects](#projects)
		- [max-external](#max-external)
		- [mspcpp](#mspcpp)

<!-- /TOC -->

## Setup

If you want to get started making an external follow these steps. If you want to contribute to this repo, please feel free to fork.

1.  Log in to GitHub.
    (If you do not have an account, you can quickly create one for free.)
    You must be logged in for the remaining steps to work.

2.  Go to <a href="https://github.com/new/import" target="_blank">GitHub's importer</a>.

3.  Paste the url of this repo as the old repository to clone:
    <https://github.com/Edinburgh-College-of-Art/maxmsp-external-template>.

4.  Select the owner for your new repository.
    (This will probably be you, but may instead be an organisation you belong to.)

5.  Choose a name for your repository. Probably should be the intended name of your external.

6.  You can now [clone this repository](#cloning-this-repository)

## Cloning this Repository

To make this repository a little more portable, the max-sdk has been included as a submodule. What this means is that you will need to state `--recurse-submodules` when cloning.

Clone this repo into your Max folder's `developing` directory

```sh
cd "/path/to/Max Folder/developing"
git clone --recurse-submodules REPOSITORY_URL
```

You should be able to build straight away from the Xcode project contained in the `xcode` directory.

## Configuration

### maxmspsdk.xcconfig

The `maxmspsdk.xcconfig` in the `xcode` directory sets a couple of global paths in the Xcode project. Some of these variables you can change others you should leave alone. The ones to change are

- `PRODUCT_VERSION`: which version of the max-sdk are you using?
- `DSTROOT`: destination of the built external (project relative directory)

### Change sdk Version

```sh
cd max-sdk
git checkout v7.0.3 # or v7.1.0 v7.3.3 v8.0.3
```

## Projects

The xcode project has multiple targets to demonstrate how to approach building a maxMSP external `.mxo` file.

### max-external

The vanilla C approach. There are plenty of examples in the sdk, but this target aims to take a more modern approach. Documentation has been added inline as well as some slightly less obtuse variable name choices which will hopefully reduce the barrier of entry.

### mspcpp

If you have already coded a ton of DSP C++ classes for something like JUCE, you may want to start here.

Demonstrates using `extern "C"` to write wrappers for pre-existing c++ classes. you need only write wrapper functions for public class methods or anything that needs to be called directly in `C`
