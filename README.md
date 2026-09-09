# Fish Count

![Fish Count at high tide on a color Pebble](evidence/basalt-tide-high.png)

A little aquarium on your wrist. Fish cruise through open water above a stone
castle, moving kelp, and a gravel bed. The tank fills a rectangular screen;
round watches get a circular aquarium window.

Count the fish for the local hour, from one to twelve. Warm fish mark AM and
cool fish mark PM on color watches. Tall kelp clumps count minute tens, and the
bright nuggets along the gravel count minute ones. Bubbles rise with the
seconds.

Tap the watch for six seconds of smoother motion at eight frames per second.
The tank normally updates once per second and pauses animation when another
screen covers it. The original clock runs offline. In settings, optionally enable NOAA tides and
enter a seven-digit US coastal harmonic station number. The waterline then
follows hourly tide predictions, interpolating between samples. The lowest
and highest values in the fetched 48-hour forecast map to the tank's low and
high waterlines; this is a relative tide display, not a water-depth gauge.
Fish stay below the surface as the tank rises and falls.

Only the selected station number goes to NOAA; there is no automatic location.
Predictions refresh every six hours while the phone companion is available.
The watch caches the forecast through its 48-hour coverage. When disabled,
unavailable, or expired, the surface line disappears and the offline aquarium
continues. A station without hourly harmonic predictions does not produce a
substitute tide. Source: [NOAA CO-OPS API](https://api.tidesandcurrents.noaa.gov/api/prod/).

## Watches and build

The project targets Basalt, Chalk, Diorite, Emery, Flint, and Gabbro. Color
watches use Pebble's 64-color palette. Monochrome watches use white fish,
outlined stonework, and a dark tank.

Install Pebble SDK 4.33.1, then run:

```sh
npm install
make test
pebble build --sdk 4.33.1
pebble install --emulator basalt build/fish-tank.pbw
```

The host tests need Clang. They check every second of a day, midnight/noon
encodings, and fish positions across all school sizes.
`test/emulator.py` uses the Python environment installed with `pebble-tool` to
capture native screens, check natural rollovers, and measure the tap cadence.

```sh
python test/emulator.py basalt --fresh
```

Reports and captures go to `build/evidence/`. Each report records the SHA-256
of the installed package. Emulator results do not measure physical-watch
battery life or daylight readability.

## Source

Luke Steuber created the original
[Fish Tank Screensaver](https://datapoems.io/clocks/fish-tank-screensaver/)
and this native adaptation. Both use the MIT license. The supplied HTML and
its checksum remain in `reference/`.

The native composition retains the local fish, kelp, nugget, and bubble time
mappings. It keeps every fish within the tank and adjusts the composition
for each screen shape. The water's daylight tint
uses fixed local hours, 07:00 through 18:59. The aquarium does not fetch
weather or calculate the source's location-based solar lighting.
