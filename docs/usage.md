# delta Usage Guide

## Input Format

The converter expects input in this form:

```text
value unit
```

Examples:
- `78 fahrenheit`
- `100 c`
- `5 meters`
- `2 liters`
- `1 atm`

Unit matching is case-insensitive.

## Running Modes

### REPL mode

Start the interactive shell:

```bash
./bin/delta
```

Inside REPL:
- `help` prints usage and supported units
- `exit` or `quit` leaves the program

If built with `readline`, command history is stored in `~/.delta_history`.

### CLI mode

You can pass conversion input directly as arguments:

```bash
./bin/delta 100 celsius
./bin/delta 10 km
./bin/delta 1.5 bar
```

Print the program version:

```bash
./bin/delta --version
```

The printed version is generated from `project(... VERSION ...)` in `CMakeLists.txt`.

Units containing spaces must be quoted or passed as separate arguments:

```bash
./bin/delta 12 "fl oz"
./bin/delta 12 fl oz
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

## Error Handling

The tool reports conversion and validation errors, including:
- division by zero in quantity arithmetic internals
- physically invalid negative values for length, volume, and pressure
- temperatures below absolute zero
- unknown units

## Notes

- Temperature arithmetic operators are intentionally not exposed, because adding/subtracting absolute temperatures is ambiguous.
- Output precision is quantity-specific (for example, temperature is printed with 2 decimal places).
