# MaxMSP BLE

A BLE Object for Max

## Install

Add [the `.mxo`](https://github.com/mhamilt/maxmsp-ble/releases/download/0.1.0-alpha/max-ble.mxo.zip) to `~/Documents/Max 8/Library` or define your own folder and add it to the [Search Path](https://docs.cycling74.com/max8/vignettes/search_path)

### Example

Copy and paste the below example into Max:

<pre><code>
----------begin_max5_patcher----------
1030.3ocwX90ahiCD.+Y3Swnn6QVTbBgDt21taOoSp2S20mNsZkIwTxpfcjs
oEtU628a73vVZKIvRKMOPBwL3Y944Odb99vAAyUaDl.32g+EFL36CGLfFxMv
flmGDrhuIuhaHwBxUqVIj1fQ9eyJ1XowQg9v7JA7PYUEjuVqQgp1BFAWmuDV
nzvU2bMTHtuLWX.tr.LVkV.bnpzXgRoUnk7ppsigOWlqtWnEE+TbZRmKfZsS
tBvp.6RA7W7MvmTRipRLFtsVIgbkTJxskx6bxval.R3+9ZM88au8O+7H3SK4
ZdNpST4k4zfjQo4O.y2Z2SmFDDPs1BbSiwNF9mkkFucmykjgw0FztVaHMiZS
i+CAnl+MzZ1sVUUh1lZsjVvhaFrVKbZfaKUxu9DIXrchvs4KwI9qZ2jQ9p3w
gifzD20jogtaSRGGBeo4eTVP9DT8eHhsS8x0qJkUBK4GYONHZo6FMzM3OFNz
cYzqLf3pp0BqRYWB2nd.tVJz2skhCPuVvQgO5HrmPWYYQtaQg6w9BkztfmK1
mx8VOXy5o0ilDjfi.1Dhq3zt7orrNYvsBXJ+OZEHdJNQWZzdVxzUt7mN3LZ5
rwIHcQde3j3W3C2m0z9J9kaQh1B2HtWTQEHdUQsyHGabp2+xXcx7zdl4mPaq
rDS0eRm0IJW5xORwCnddAI95uQe7wzcudraqEdHBBFge9oMen3T1dtroTFYT
TKUYCOHlQGDyny.yUBigem3EbxxB+HhYzDfEl.Yg3sItvK7i66LHIElLo80f
iyeSZZTSZZKK.rjegE.1aseFWE9iWseN9D7yr3KretsLy7JrQJeKGpEPQoYW
KR0XU25kBMupqRtroIiiRbbkQ0elE1YR6jKbRaaQyDkmWvJCcY6Qnu1Tqtwn
KbzZq6T56O00b5A8g.20JbgXC7art1Akk51AMJYpO0LsSuYXOsaBEsd3PUDx
m2AeWQuwdNC8c6kzYCCy5oX2EXS.EmWraF0OTCdwy5JxMsmBbwCqUCF7zNRz
z6p0fzwoHKL+YSxlzkmpu5xwX4Z6o.ieu.lO8paV5sJlMETBOyplI9MB7ERl
x5JzK6BG50FhtPuyitDJUpIVLtS3R5K3vnvyr6rlSOQdNeuKsAWb+za1yNw6
Kvys+vIzXVzLZG8TFUjrscz+Ex+1GQZtn2LyydKXj43F+obaTq046.XWm+vi
lTg.OBrjNZ3dB45NFXvWN3Z6opIWumGUS6LmWmlNEjheKTTzono2.EE+Nomj
2I8j8NomzyRO9jJdcM12moQXREXIguoztGyFQOVJ8ORIpAZw8k6jmd+nAtWd
boEKHrVSlUvlo9cZCVoJDZ45xlDZDNTkTIIIGK1V27F.oJWC+wv+G3lYc1A
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
