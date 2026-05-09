# unitfy Usage Guide

## Input Format

The converter expects input in this form:

```text
value from_unit [to_unit]
```

Examples:
- `78 fahrenheit`
- `100 c`
- `5 meters`
- `2 liters`
- `1 atm`
- `12 km mi`
- `2026-05-09 date jd`

Unit matching is case-insensitive.

## Running Modes

### REPL mode

Start the interactive shell:

```bash
./bin/unitfy
```

Inside REPL:
- `help` prints usage and supported units
- `exit` or `quit` leaves the program

If built with `readline`, command history is stored in `~/.unitfy_history`.

If built with `readline`, tab completion is available in REPL:
- first token: command completion (`help`, `exit`, `quit`)
- later tokens: unit alias completion (for example, `celsius`, `km`, `kpa`, `floz`)
- calendar dates use `YYYY-MM-DD` with the `date` unit

### CLI mode

You can pass conversion input directly as arguments:

```bash
./bin/unitfy 100 celsius
./bin/unitfy 10 km
./bin/unitfy 12 km mi
./bin/unitfy 1.5 bar
./bin/unitfy 60439 mjd date
```

Behavior:
- With `to_unit`, only that target conversion is printed.
- Without `to_unit`, all supported conversions in that unit family are printed.
- Inputs with more than one target token are rejected (for example, `./bin/unitfy 2 km m mi`).

Print the program version:

```bash
./bin/unitfy --version
```

The printed version is generated from `project(... VERSION ...)` in `CMakeLists.txt`.

Units containing spaces must be quoted or passed as separate arguments:

```bash
./bin/unitfy 12 "fl oz"
./bin/unitfy 12 fl oz
```

For targeted conversions from a multi-word source unit, use a single-token
alias for the source unit:

```bash
./bin/unitfy 12 floz ml
```

## Supported Units and Aliases

### Temperature

- `c`, `celsius`
- `f`, `fahrenheit`
- `k`, `kelvin`
- `r`, `rankine`

### Length

- `mm`, `millimeter`, `millimeters`
- `cm`, `centimeter`, `centimeters`
- `m`, `meter`, `meters`
- `km`, `kilometer`, `kilometers`
- `in`, `inch`, `inches`
- `ft`, `foot`, `feet`
- `usft`, `us ft`, `us foot`, `us feet`, `survey foot`, `survey feet`
- `ukft`, `uk ft`, `uk foot`, `uk feet`, `imperial foot`, `imperial feet`
- `mi`, `mile`, `miles`

### Volume

- `ml`, `milliliter`, `milliliters`
- `l`, `liter`, `liters`
- `gal`, `gallon`, `gallons`
- `fl oz`, `fluidounce`, `fluidounces`, `floz`
- `m3`, `m³`, `cubicmeter`, `cubicmeters`
- `mm3`, `mm³`, `cubicmm`, `cubicmillimeter`
- `cm3`, `cm³`, `cubiccm`, `cubiccentimeter`

### Pressure

- `pa`, `pascal`, `pascals`
- `kpa`, `kilopascal`, `kilopascals`
- `bar`
- `atm`, `atmosphere`, `atmospheres`
- `psi`
- `torr`
- `mmhg`

### Time

- `date`, `calendar`, `gregorian` (`YYYY-MM-DD`)
- `jd`, `julianday`, `julian day`
- `mjd`, `modifiedjulian`, `modifiedjulianday`, `modified julian day`
- `unix`, `epoch`, `unixs`, `epochseconds`
- `unixms`, `epochms`, `epochmilliseconds`

## Error Handling

The tool reports conversion and validation errors, including:
- division by zero in quantity arithmetic internals
- physically invalid negative values for length, volume, and pressure
- temperatures below absolute zero
- unknown units

## Notes

- Temperature arithmetic operators are intentionally not exposed, because adding/subtracting absolute temperatures is ambiguous.
- Plain `ft` uses the international foot. Use `usft` for US survey feet and `ukft` for UK imperial feet.
- Output precision is quantity-specific (for example, temperature is printed with 2 decimal places).
- Time conversions treat calendar dates as UTC midnight in the proleptic Gregorian calendar.
