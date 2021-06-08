# MaxMSP BLE

A BLE Object for Max

## Install

Add [the `.mxo`](https://github.com/mhamilt/maxmsp-ble/releases/download/0.1.0-alpha/max-ble.mxo.zip) to `~/Documents/Max 8/Library` or define your own folder and add it to the [Search Path](https://docs.cycling74.com/max8/vignettes/search_path)

### Usage

- Send a `scan` message to the object to find available devices.
- Found Devices will be printed to the Max Console Window
- Details logged about a device will include an index.
- send a `connect $1` message to the object, where `$1` is the index of a found device.
- Details of what services and charateristics the device has will be printed to the Max Window
- Available data will be sent out the inlet in `list` form `SERVICE_UUID CHARACTERISTIC_UUID RAW_BYTE_LIST`
- Use a pair of `route` objects to filter which Service / Characteristic pair you need.
- `RAW_BYTE_LIST` can then be assmebled into the original data format.

### Example

Copy and paste the below example into Max:

<pre><code>
----------begin_max5_patcher----------
1089.3ocuX17ahiCE.+L7WwSQyQJJNNewdaZmtRqT2S61SqFsxjXJYTvNx1z
Byn4+809kPKaqgAZC4PCEmG489k2m1+X7nf4xMbc.7av+.iF8iwiFgK4VXT2
2GErhsonloQwBD7mjy+Vvj1aY3aL3xeuFz0UEbfr6VUk3MrBeEMZ2hh0qjqM
0bC9vh5VscIy1FdqkDDLw9G70ta2vLEKqDO7uJdgoUh3XxzvI.kl39HAuFEM
M74eiUQUhm0iaseNdr6xjOFmJqsxAkVW4gynzdmyrLjyrXjy79myUbsl8.+M
fdUVnOBi8SH4fDdT5hnNf59HiNbzUxeDCVOnmLpO4jjOCQKOD47BDsVHWshK
LugSqPWMulCOUUWCEqUJqP0aAMmoJVBKjJ356tEZeanAlnDzFohCLntRafJg
gqDr55sSguTUHejq3kOKN9PmygFkStRvHAyRN7mrMvMRgVVymB22HEPgTHru
LruTbxv5d.nv+0sJ7+u+9+3KSfaVxTrBqNsJup.WDMJE6IX9Vyd5TaAArdAf
o6L1oveurR2Z2ELAZXLk1ZWq0nlsZqM605fctlt2U0UVaStVfuvndBEH9CEB
Or+l9haNIE84wYGxaS5Qu800q4FozrDtS9DbqfqdXK5jstDOA4jYuOxRvqj7
HLNNbOxVHElErB99oJWLZ6hs8AV9wAyYl5puilIMc5uhWr1KglMbNxWkGbsK
z2GmYmsCLJc1zjmK4RhouwGd4hNYFKQag63OxqwbaeHkd1HMKduVjDBY3I5P
rPdurzNVS1r9GkiNVSzmeojv9U.C66wZhH64xRyFt99zTe9oj9rYeKacIXQw
zAdzTRd3u6iQ5ExGRuX9vCk0UTamdosOubATVo2MWRisdYyRthU6sXY7YmMR
RSlFk33BG7lLKr+SHOTjJRoOL52ISstr8HjNfildS6PgtIB85CAla9yR9F3S
91SII7768Qxb89hRRaSMyFtNEXzp+PUKjudrYO3d9ipQnsXF1NlVBc3hcWXG
mtzCDY8YnaNNHSGdzYCWfqcGRMf1tECg0f7P46XBlroYVVHsaYHOd3hK0Flx
bLXN+hlssBHsYWWBVNXEytBJ9pVj2qEMS5RovDrTxvM9hKzyCc857KIXpTWr
HcHgyFE56vC60gy511C54ZGcYPlM6H6U8Lwy0I4DlKKZF1POifEIiNk7OTB7
3Pd0ABiJws9+masbspXmYs63OgWTTI2t4UAyTIE6KjshBPfu58c6opI2AJ6d
HGWSclyGRSjS.IzZdQHopjqvhjeHMGEcBplzCHlOP5IafzSx.oG5.oG21u+0
JJoGRqHmBR6LmOllFpf6SI+8UX+b96aKNxZZryuq694nRsk1+lDEOeB90JQ6
WwJ3AJ9iU6jGOb4.2IuWYrkqWqPCMXSZ6PVAqjVEKVW0oaKtVUhsVDLaSylt
CVE6.M9mi+OfLGRHB
-----------end_max5_patcher-----------
</code></pre>

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
