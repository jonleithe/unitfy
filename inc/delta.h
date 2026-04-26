/*
 * filename:    delta.h
 * created:     26 April 2026
 * author:      jole
 * description: Public API: quantity types, unit enums, conversion functions,
 *              and the MDelta application class.
 */

#pragma once

#include <string>
#include <stdexcept>

#include "delta_version.h"



// Quantity types and unit conversions
namespace unitfy {

class QuantityError : public std::runtime_error{
public:
    explicit QuantityError(const std::string& msg) : std::runtime_error(msg) {}
}; // ———  END OF class QuantityError ——————————————————————————————————————————



namespace quantity_error_messages {
inline constexpr const char* kDivisionByZero = "Division by zero";

inline constexpr const char* kTempBelowAbsoluteZero = "Temperature cannot be below absolute zero";
inline constexpr const char* kTempMulBelowAbsoluteZero = "Multiplication resulted in temperature below absolute zero";
inline constexpr const char* kTempDivBelowAbsoluteZero = "Division resulted in temperature below absolute zero";

inline constexpr const char* kLengthNegative = "Length cannot be negative";
inline constexpr const char* kLengthSubNegative = "Subtraction resulted in negative length";
inline constexpr const char* kLengthMulNegative = "Multiplication resulted in negative length";
inline constexpr const char* kLengthDivNegative = "Division resulted in negative length";

inline constexpr const char* kVolumeNegative = "Volume cannot be negative";
inline constexpr const char* kVolumeSubNegative = "Subtraction resulted in negative volume";
inline constexpr const char* kVolumeMulNegative = "Multiplication resulted in negative volume";
inline constexpr const char* kVolumeDivNegative = "Division resulted in negative volume";

inline constexpr const char* kPressureNegative = "Pressure cannot be negative";
inline constexpr const char* kPressureSubNegative = "Subtraction resulted in negative pressure";
inline constexpr const char* kPressureMulNegative = "Multiplication resulted in negative pressure";
inline constexpr const char* kPressureDivNegative = "Division resulted in negative pressure";

inline constexpr const char* kUnknownTemperatureUnitPrefix = "Unknown temperature unit: ";
inline constexpr const char* kUnknownLengthUnitPrefix = "Unknown length unit: ";
inline constexpr const char* kUnknownVolumeUnitPrefix = "Unknown volume unit: ";
inline constexpr const char* kUnknownPressureUnitPrefix = "Unknown pressure unit: ";
} // ———  END OF namespace quantity_error_messages —————————————————————————————



namespace cli_messages {
inline constexpr const char* kVersionFmt = "%s v%s";
inline constexpr const char* kGoodbyeFmt = "Thank you for using %s! Have a nice day! /jole";
inline constexpr const char* kInvalidInputFormat = "Invalid input format. Use: value unit";
inline constexpr const char* kReplBannerFmt = "%s v%s - unit conversion REPL";
inline constexpr const char* kReplHint = "Type 'help' for usage or 'exit' to quit.";
inline constexpr const char* kCliUsageFmt = "Usage: %s [value unit [target_unit]]";
inline constexpr const char* kCliUsageReplFmt = "Or:    %s (no args for REPL mode)";
inline constexpr const char* kCliUsageVersionFmt = "Or:    %s --version";
inline constexpr const char* kHelpTitleFmt = "%s unit conversion utility";
inline constexpr const char* kHelpUsage = "Usage: value unit [target_unit(s)]";
inline constexpr const char* kHelpExamples = "Examples: 78 fahrenheit, 100 celsius, 5 meters, 2 liters, 1 atm, 12 km mi";
inline constexpr const char* kHelpTemperatureUnits = "Temperature units: C, F, K, R (or full names)";
inline constexpr const char* kHelpLengthUnits = "Length units: mm, cm, m, km, in, ft, usft, ukft, mi";
inline constexpr const char* kHelpVolumeUnits = "Volume units: mL, L, gal, fl oz, m3, mm3, cm3";
inline constexpr const char* kHelpPressureUnits = "Pressure units: Pa, kPa, bar, atm, psi, torr, mmHg";
inline constexpr const char* kHelpExitHint = "Type 'exit' or 'quit' to leave REPL.";
} // ———  END OF namespace cli_messages ————————————————————————————————————————



namespace format_settings {
inline constexpr int kDefaultPrecision      = 5;
inline constexpr int kTemperaturePrecision  = 2;
inline constexpr int kLengthPrecision       = 5;
inline constexpr int kVolumePrecision       = 5;
inline constexpr int kPressurePrecision     = 5;
} // ———  END OF namespace format_settings ———————————————————————————————————



enum class TemperatureUnit{
    Celsius,
    Fahrenheit,
    Kelvin,
    Rankine
}; // ———  END OF enum class TemperatureUnit ———————————————————————————————————



enum class LengthUnit{
    Millimeter,
    Centimeter,
    Meter,
    Kilometer,
    Inch,
    Foot,
    USSurveyFoot,
    UKFoot,
    Mile
}; // ———  END OF enum class LengthUnit ————————————————————————————————————————



enum class VolumeUnit{
    Milliliter,
    Liter,
    Gallon,
    FluidOunce,
    CubicMeter,
    CubicMillimeter,
    CubicCentimeter
}; // ———  END OF enum class VolumeUnit ————————————————————————————————————————



enum class PressureUnit{
    Pascal,
    Kilopascal,
    Bar,
    Atmosphere,
    Psi,
    Torr,
    MmHg
}; // ———  END OF enum class PressureUnit ——————————————————————————————————————



enum class CommandResult{
    Continue,
    Exit
}; // ———  END OF enum class CommandResult ———————————————————————————————————



std::string format_value(double value, int precision = format_settings::kDefaultPrecision);

bool try_convert_temperature(double value, const std::string& unit_str, const std::string& to_unit_str = "");
bool try_convert_length(double value, const std::string& unit_str, const std::string& to_unit_str = "");
bool try_convert_volume(double value, const std::string& unit_str, const std::string& to_unit_str = "");
bool try_convert_pressure(double value, const std::string& unit_str, const std::string& to_unit_str = "");



class Temperature{
private:
    double kelvin_ = 0.0;  // Store internally in Kelvin for conversions

public:
    Temperature(double value, TemperatureUnit unit);
    
    double to_unit(TemperatureUnit unit) const;
    std::string to_string(TemperatureUnit unit) const;
    
    // Note: Arithmetic operators removed. Adding/subtracting absolute temperatures
    // is semantically meaningless. Use a separate TemperatureDelta class if needed.
}; // ———  END OF class Temperature ————————————————————————————————————————————



class Length {
private:
    double meters_ = 0.0;  // Store internally in meters

public:
    Length(double value, LengthUnit unit);
    
    double to_unit(LengthUnit unit) const;
    std::string to_string(LengthUnit unit) const;
    
    Length operator+(const Length& other) const;
    Length operator-(const Length& other) const;
    Length operator*(double scalar) const;
    Length operator/(double scalar) const;
    
    friend Length operator*(double scalar, const Length& len);
}; // ———  END OF class Length —————————————————————————————————————————————————



class Volume{
private:
    double liters_ = 0.0;  // Store internally in liters

public:
    Volume(double value, VolumeUnit unit);
    
    double to_unit(VolumeUnit unit) const;
    std::string to_string(VolumeUnit unit) const;
    
    Volume operator+(const Volume& other) const;
    Volume operator-(const Volume& other) const;
    Volume operator*(double scalar) const;
    Volume operator/(double scalar) const;
    
    friend Volume operator*(double scalar, const Volume& vol);
}; // ———  END OF class Volume —————————————————————————————————————————————————



class Pressure{
private:
    double pascal_ = 0.0;  // Store internally in pascal

public:
    Pressure(double value, PressureUnit unit);

    double to_unit(PressureUnit unit) const;
    std::string to_string(PressureUnit unit) const;

    Pressure operator+(const Pressure& other) const;
    Pressure operator-(const Pressure& other) const;
    Pressure operator*(double scalar) const;
    Pressure operator/(double scalar) const;

    friend Pressure operator*(double scalar, const Pressure& p);
}; // ———  END OF class Pressure ———————————————————————————————————————————————



// CLI handler
class MDelta{
public:
    MDelta();
    ~MDelta();
    
    int run(int argc, char* argv[]);

private:
    std::string program_name_ = "unitfy";

    void repl(void);
    CommandResult process_input(const std::string& input);
}; // ———  END OF class MDelta —————————————————————————————————————————————————

}  // namespace unitfy
