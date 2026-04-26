/*
 * filename:    temperature.cpp
 * created:     26 April 2026
 * author:      jole
 * description: Temperature quantity type and unit conversion implementation.
 */

#include <cstdio>

#include "delta.h"
#include "unit_helpers.h"



namespace unitfy {

Temperature::Temperature(double value, TemperatureUnit unit){
    switch (unit){
        case TemperatureUnit::Celsius:      kelvin_ = value + 273.15;
                                            break;
        case TemperatureUnit::Fahrenheit:   kelvin_ = (value - 32.0) * 5.0 / 9.0 + 273.15;
                                            break;
        case TemperatureUnit::Kelvin:       kelvin_ = value;
                                            break;
        case TemperatureUnit::Rankine:      kelvin_ = value * 5.0 / 9.0;
                                            break;
        default:                            throw QuantityError("Invalid temperature unit");
    } // ———  END OF switch(unit)———————————————————————————————————————————————

    if (kelvin_ < 0.0) {
        throw QuantityError(quantity_error_messages::kTempBelowAbsoluteZero);
    } // ———  END OF if(kelvin_ < 0)————————————————————————————————————————————
} // ———  END OF function Temperature::Temperature——————————————————————————————



double Temperature::to_unit(TemperatureUnit unit) const{
    switch (unit){
        case TemperatureUnit::Celsius:      return kelvin_ - 273.15;
        case TemperatureUnit::Fahrenheit:   return (kelvin_ - 273.15) * 9.0 / 5.0 + 32.0;
        case TemperatureUnit::Kelvin:       return kelvin_;
        case TemperatureUnit::Rankine:      return kelvin_ * 9.0 / 5.0;
        default:                            throw QuantityError("Invalid temperature unit");
    } // ———  END OF switch(unit)———————————————————————————————————————————————
} // ———  END OF function Temperature::to_unit——————————————————————————————————



std::string Temperature::to_string(TemperatureUnit unit) const{
    static const char* names[] = {"Celsius", "Fahrenheit", "Kelvin", "Rankine"};

    return format_value(to_unit(unit),
                        format_settings::kTemperaturePrecision)
                        + " "
                        + names[static_cast<int>(unit)];
} // ———  END OF function Temperature::to_string————————————————————————————————



static constexpr UnitAlias<TemperatureUnit> kTemperatureAliases[] = {
    {TemperatureUnit::Celsius, "c"},
    {TemperatureUnit::Celsius, "celsius"},
    {TemperatureUnit::Fahrenheit, "f"},
    {TemperatureUnit::Fahrenheit, "fahrenheit"},
    {TemperatureUnit::Kelvin, "k"},
    {TemperatureUnit::Kelvin, "kelvin"},
    {TemperatureUnit::Rankine, "r"},
    {TemperatureUnit::Rankine, "rankine"},
};

static constexpr UnitDisplay<TemperatureUnit> kTemperatureDisplay[] = {
    {TemperatureUnit::Celsius, "\xC2\xB0" "C"},
    {TemperatureUnit::Fahrenheit, "\xC2\xB0" "F"},
    {TemperatureUnit::Kelvin, "K"},
    {TemperatureUnit::Rankine, "\xC2\xB0" "R"},
};

static constexpr TemperatureUnit kTemperatureOutputOrder[] = {
    TemperatureUnit::Celsius,
    TemperatureUnit::Fahrenheit,
    TemperatureUnit::Kelvin,
    TemperatureUnit::Rankine,
};

static bool parse_temperature_unit(const std::string& unit_str,
                                   TemperatureUnit* out_unit){
    return parse_unit_aliases(unit_str, kTemperatureAliases, out_unit);
} // ———  END OF function parse_temperature_unit————————————————————————————————



static void convert_temperature(double value, TemperatureUnit from_unit){
    Temperature temp(value, from_unit);
    printf("Temperature conversion of %.2f %s:\n", value,
           display_symbol(from_unit, kTemperatureDisplay));
    print_all_conversions(temp, kTemperatureOutputOrder);
} // ———  END OF function convert_temperature———————————————————————————————————



bool try_convert_temperature(double value, const std::string& unit_str, const std::string& to_unit_str){
    TemperatureUnit from_unit;
    if (!parse_temperature_unit(unit_str, &from_unit)) {
        return false;
    }

    if (to_unit_str.empty()) {
        convert_temperature(value, from_unit);
        return true;
    }

    TemperatureUnit to_unit;
    if (!parse_temperature_unit(to_unit_str, &to_unit)) {
        printf("Incompatible units: '%s' is a temperature unit, '%s' is not\n",
               unit_str.c_str(), to_unit_str.c_str());
        return true;
    }

    Temperature temp(value, from_unit);
    printf("%s = %s\n", temp.to_string(from_unit).c_str(), temp.to_string(to_unit).c_str());
    return true;
} // ———  END OF function try_convert_temperature———————————————————————————————

}  // namespace unitfy