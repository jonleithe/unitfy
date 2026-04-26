# delta API Reference

This document describes the public C++ API declared in `inc/delta.h`.

## Namespace

All public symbols are declared under:

```cpp
namespace delta { ... }
```

## Exception Type

### `QuantityError`

```cpp
class QuantityError : public std::runtime_error
```

Used for conversion and validation errors (for example, invalid units, negative values where not allowed, division by zero, or temperatures below absolute zero).

## Unit Enums

### `TemperatureUnit`

- `Celsius`
- `Fahrenheit`
- `Kelvin`
- `Rankine`

### `LengthUnit`

- `Millimeter`
- `Centimeter`
- `Meter`
- `Kilometer`
- `Inch`
- `Foot`
- `Mile`

### `VolumeUnit`

- `Milliliter`
- `Liter`
- `Gallon`
- `FluidOunce`
- `CubicMeter`
- `CubicMillimeter`
- `CubicCentimeter`

### `PressureUnit`

- `Pascal`
- `Kilopascal`
- `Bar`
- `Atmosphere`
- `Psi`
- `Torr`
- `MmHg`

### `CommandResult`

- `Continue`
- `Exit`

Primarily used by the REPL input processor.

## Formatting Helper

### `format_value`

```cpp
std::string format_value(double value,
                         int precision = format_settings::kDefaultPrecision);
```

Formats numeric values for output with a configurable decimal precision.

## Top-Level Conversion Entry Points

These functions parse a unit string and perform conversions for their quantity family.

```cpp
bool try_convert_temperature(double value, const std::string& unit_str);
bool try_convert_length(double value, const std::string& unit_str);
bool try_convert_volume(double value, const std::string& unit_str);
bool try_convert_pressure(double value, const std::string& unit_str);
```

Return value semantics:
- `true`: input unit was recognized and conversion was executed
- `false`: unit string did not match this family

## Quantity Classes

All quantity classes store an internal canonical unit and expose conversion plus formatted output methods.

### `Temperature`

```cpp
class Temperature {
public:
    Temperature(double value, TemperatureUnit unit);
    double to_unit(TemperatureUnit unit) const;
    std::string to_string(TemperatureUnit unit) const;
};
```

Notes:
- Internally stored in Kelvin.
- Values below absolute zero throw `QuantityError`.
- Arithmetic operators are intentionally not provided for absolute temperatures.

### `Length`

```cpp
class Length {
public:
    Length(double value, LengthUnit unit);
    double to_unit(LengthUnit unit) const;
    std::string to_string(LengthUnit unit) const;

    Length operator+(const Length& other) const;
    Length operator-(const Length& other) const;
    Length operator*(double scalar) const;
    Length operator/(double scalar) const;
    friend Length operator*(double scalar, const Length& len);
};
```

Notes:
- Internally stored in meters.
- Negative lengths are rejected.

### `Volume`

```cpp
class Volume {
public:
    Volume(double value, VolumeUnit unit);
    double to_unit(VolumeUnit unit) const;
    std::string to_string(VolumeUnit unit) const;

    Volume operator+(const Volume& other) const;
    Volume operator-(const Volume& other) const;
    Volume operator*(double scalar) const;
    Volume operator/(double scalar) const;
    friend Volume operator*(double scalar, const Volume& vol);
};
```

Notes:
- Internally stored in liters.
- Negative volumes are rejected.

### `Pressure`

```cpp
class Pressure {
public:
    Pressure(double value, PressureUnit unit);
    double to_unit(PressureUnit unit) const;
    std::string to_string(PressureUnit unit) const;

    Pressure operator+(const Pressure& other) const;
    Pressure operator-(const Pressure& other) const;
    Pressure operator*(double scalar) const;
    Pressure operator/(double scalar) const;
    friend Pressure operator*(double scalar, const Pressure& p);
};
```

Notes:
- Internally stored in Pascal.
- Negative pressures are rejected.

## CLI Application Wrapper

### `MDelta`

```cpp
class MDelta {
public:
    MDelta();
    ~MDelta();
    int run(int argc, char* argv[]);
};
```

`run` supports:
- REPL mode when no conversion arguments are provided
- one-shot conversion mode when arguments provide `value unit`
