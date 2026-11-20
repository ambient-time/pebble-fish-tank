# Fish Count

A native Pebble watchface: a little aquarium on your wrist. Fish cruise through
open water above a stone castle, swaying kelp and a gravel bed. The tank fills
a rectangular screen; round watches get a circular aquarium window.

![Fish Count at high tide on a color Pebble Time](evidence/basalt-tide-high.png)

Count the fish for the local hour, from one to twelve. On color watches, warm
fish mark AM and cool fish mark PM. Tall kelp clumps count the tens of
minutes, and the bright nuggets along the gravel count the ones. Bubbles rise
with the seconds.

The tank updates once a second and pauses when another screen covers it. Tap
the watch for six seconds of smoother motion at eight frames a second. The
clock works offline. The water's daylight tint follows fixed local hours,
07:00 through 18:59.

## Tides (optional)

In the face's settings in the Pebble phone app, turn on **Enable tide
waterline** and enter a seven-digit NOAA station number for a US coastal
station with harmonic predictions (for example 9414290, San Francisco). The
waterline then follows hourly tide predictions and interpolates between
them. The lowest and highest values in the fetched 48-hour forecast map to
the tank's low and high waterlines, so this is a relative tide display, not a
depth gauge. Fish stay below the surface as the water rises and falls.

Only the station number goes to NOAA; the face never looks up your location.
The phone refreshes predictions every six hours while it is connected, and
the watch keeps the forecast for its 48 hours. When tides are off, unavailable
or expired, the surface line disappears and the aquarium carries on. A station
without hourly harmonic predictions gets no substitute tide. Data comes from
the [NOAA CO-OPS API](https://api.tidesandcurrents.noaa.gov/api/prod/).

## Watches

| Platform | Watch | Display |
| --- | --- | --- |
| basalt | Pebble Time, Time Steel | 144 × 168, color |
| chalk | Pebble Time Round | 180 × 180, color, round |
| diorite | Pebble 2 | 144 × 168, monochrome |
| emery | Pebble Time 2 | 200 × 228, color |
| flint | Pebble 2 Duo | 144 × 168, monochrome |
| gabbro | Pebble Round 2 | 260 × 260, color, round |

Color watches use Pebble's 64-color palette. Monochrome watches use white
fish, outlined stonework and a dark tank.

## Install

Fish Count is not on the Pebble Appstore. The source here is version 0.2.0,
which adds the tide waterline; build it from source. `release/` holds the
earlier 0.1.0 and 0.1.1 PBWs, without tides, with their source ZIPs and
SHA-256 checksums.

## Build and test

Install Pebble SDK 4.33.1, then run:

```sh
npm install
make test
pebble build --sdk 4.33.1
pebble install --emulator basalt build/fish-tank.pbw
```

Pebble names the output after the project directory, so the last line assumes
a directory named `fish-tank`. The host tests need Clang and Node.js. They
check every second of a day, the midnight and noon encodings, fish positions
for every school size, and the tide feed.

`test/emulator.py` uses the Python environment that has `pebble-tool` to
capture native screens, check natural rollovers and measure the tap cadence:

```sh
python test/emulator.py basalt --fresh
```

Reports and captures go to `build/evidence/`, and each report records the
SHA-256 of the installed package. The committed reports and screenshots are in
[evidence/](evidence/). Build logs in earlier commits, and the 0.1.0 and 0.1.1
source ZIPs, contain absolute paths from the original build machine.

## How the port works

I made the original
[Fish Tank Screensaver](https://datapoems.io/clocks/fish-tank-screensaver/) as
a web clock. Its HTML and checksum are in `reference/`.

The watchface keeps the web version's fish, kelp, nugget and bubble time
mappings. It keeps every fish inside the tank and adjusts the composition for
each screen shape. It does not fetch weather or calculate the web version's
location-based solar lighting.

## License

Copyright 2026 Luke Steuber. [MIT License](LICENSE). Phone settings use
[@rebble/clay](https://github.com/pebble-dev/clay) (MIT), installed by `npm
install` and bundled into the built PBW. Tide predictions are NOAA data. No
third-party fonts or images are bundled.
