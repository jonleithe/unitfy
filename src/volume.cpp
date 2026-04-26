/*
 * filename:    volume.cpp
 * created:     26 April 2026
 * author:      jole
 * description: Volume quantity type and unit conversion implementation.
 */

#include <cstdio>

#include "delta.h"
#include "unit_helpers.h"



namespace unitfy {

Volume::Volume(double value, VolumeUnit unit){
    switch (unit){
        case VolumeUnit::Milliliter:        liters_ = value / 1000.0;
                                            break;
        case VolumeUnit::Liter:             liters_ = value;
                                            break;
        case VolumeUnit::Gallon:            liters_ = value * 3.78541;
                                            break;
        case VolumeUnit::FluidOunce:        liters_ = value * 0.0295735;
                                            break;
        case VolumeUnit::CubicMeter:        liters_ = value * 1000.0;
                                            break;
        case VolumeUnit::CubicMillimeter:   liters_ = value / 1000000.0;
                                            break;
        case VolumeUnit::CubicCentimeter:   liters_ = value / 1000.0;
                                            break;
        default:                            throw QuantityError("Invalid volume unit");
    } // ———  END OF switch(unit)———————————————————————————————————————————————

    if (liters_ < 0.0) {
        throw QuantityError(quantity_error_messages::kVolumeNegative);
    }
} // ———  END OF function Volume::Volume————————————————————————————————————————



double Volume::to_unit(VolumeUnit unit) const{
    switch (unit){
        case VolumeUnit::Milliliter:        return liters_ * 1000.0;
        case VolumeUnit::Liter:             return liters_;
        case VolumeUnit::Gallon:            return liters_ / 3.78541;
        case VolumeUnit::FluidOunce:        return liters_ / 0.0295735;
        case VolumeUnit::CubicMeter:        return liters_ / 1000.0;
        case VolumeUnit::CubicMillimeter:   return liters_ * 1000000.0;
        case VolumeUnit::CubicCentimeter:   return liters_ * 1000.0;
        default:                            throw QuantityError("Invalid volume unit");
    } // ———  END OF switch(unit)———————————————————————————————————————————————
} // ———  END OF function Volume::to_unit———————————————————————————————————————



std::string Volume::to_string(VolumeUnit unit) const{
    static const char* names[] = {"mL",
                                  "L",
                                  "gal",
                                  "fl oz",
                                  "m\xC2\xB3",
                                  "mm\xC2\xB3",
                                  "cm\xC2\xB3"};
    
    return format_value(to_unit(unit),
                        format_settings::kVolumePrecision)
                        + " "
                        + names[static_cast<int>(unit)];
} // ———  END OF function Volume::to_string—————————————————————————————————————



Volume Volume::operator+(const Volume& other) const{
    Volume result(liters_, VolumeUnit::Liter);
    result.liters_ = this->liters_ + other.liters_;
    
    return result;
} // ———  END OF function Volume::operator+—————————————————————————————————————



Volume Volume::operator-(const Volume& other) const{
    Volume result(liters_, VolumeUnit::Liter);
    result.liters_ = this->liters_ - other.liters_;
    if (result.liters_ < 0.0) {
        throw QuantityError(quantity_error_messages::kVolumeSubNegative);
    }

    return result;
} // ———  END OF function Volume::operator-—————————————————————————————————————



Volume Volume::operator*(double scalar) const{
    Volume result(liters_, VolumeUnit::Liter);
    result.liters_ = this->liters_ * scalar;
    if (result.liters_ < 0.0) {
        throw QuantityError(quantity_error_messages::kVolumeMulNegative);
    }
    
    return result;
} // ———  END OF function Volume::operator*—————————————————————————————————————



Volume Volume::operator/(double scalar) const{
    if (scalar == 0.0) {
        throw QuantityError(quantity_error_messages::kDivisionByZero);
    }

    Volume result(liters_, VolumeUnit::Liter);
    result.liters_ = this->liters_ / scalar;
    if (result.liters_ < 0.0) {
        throw QuantityError(quantity_error_messages::kVolumeDivNegative);
    }

    return result;
} // ———  END OF function Volume::operator/—————————————————————————————————————



Volume operator*(double scalar, const Volume& vol){
    return vol * scalar;
} // ———  END OF function operator*(Volume)—————————————————————————————————————



static constexpr UnitAlias<VolumeUnit> kVolumeAliases[] = {
    {VolumeUnit::Milliliter, "ml"},
    {VolumeUnit::Milliliter, "milliliter"},
    {VolumeUnit::Milliliter, "milliliters"},
    {VolumeUnit::Liter, "l"},
    {VolumeUnit::Liter, "liter"},
    {VolumeUnit::Liter, "liters"},
    {VolumeUnit::Gallon, "gal"},
    {VolumeUnit::Gallon, "gallon"},
    {VolumeUnit::Gallon, "gallons"},
    {VolumeUnit::FluidOunce, "fl oz"},
    {VolumeUnit::FluidOunce, "fluidounce"},
    {VolumeUnit::FluidOunce, "fluidounces"},
    {VolumeUnit::FluidOunce, "floz"},
    {VolumeUnit::CubicMeter, "m3"},
    {VolumeUnit::CubicMeter, "m\xC2\xB3"},
    {VolumeUnit::CubicMeter, "cubicmeter"},
    {VolumeUnit::CubicMeter, "cubicmeters"},
    {VolumeUnit::CubicMillimeter, "mm3"},
    {VolumeUnit::CubicMillimeter, "mm\xC2\xB3"},
    {VolumeUnit::CubicMillimeter, "cubicmm"},
    {VolumeUnit::CubicMillimeter, "cubicmillimeter"},
    {VolumeUnit::CubicCentimeter, "cm3"},
    {VolumeUnit::CubicCentimeter, "cm\xC2\xB3"},
    {VolumeUnit::CubicCentimeter, "cubiccm"},
    {VolumeUnit::CubicCentimeter, "cubiccentimeter"},
};

static constexpr UnitDisplay<VolumeUnit> kVolumeDisplay[] = {
    {VolumeUnit::Milliliter, "mL"},
    {VolumeUnit::Liter, "L"},
    {VolumeUnit::Gallon, "gal"},
    {VolumeUnit::FluidOunce, "fl oz"},
    {VolumeUnit::CubicMeter, "m\xC2\xB3"},
    {VolumeUnit::CubicMillimeter, "mm\xC2\xB3"},
    {VolumeUnit::CubicCentimeter, "cm\xC2\xB3"},
};

static constexpr VolumeUnit kVolumeOutputOrder[] = {
    VolumeUnit::Milliliter,
    VolumeUnit::Liter,
    VolumeUnit::Gallon,
    VolumeUnit::FluidOunce,
    VolumeUnit::CubicMeter,
    VolumeUnit::CubicMillimeter,
    VolumeUnit::CubicCentimeter,
};

static bool parse_volume_unit(const std::string& unit_str,
                              VolumeUnit* out_unit){
    return parse_unit_aliases(unit_str, kVolumeAliases, out_unit);
} // ———  END OF function parse_volume_unit—————————————————————————————————————



static void convert_volume(double value, VolumeUnit from_unit){
    Volume vol(value, from_unit);
    printf("Volume conversion of %.4f %s:\n", value,
           display_symbol(from_unit, kVolumeDisplay));
    print_all_conversions(vol, kVolumeOutputOrder);
} // ———  END OF function convert_volume————————————————————————————————————————



bool try_convert_volume(double value, const std::string& unit_str, const std::string& to_unit_str){
    VolumeUnit from_unit;
    if (!parse_volume_unit(unit_str, &from_unit)) {
        return false;
    }

    if (to_unit_str.empty()) {
        convert_volume(value, from_unit);
        return true;
    }

    VolumeUnit to_unit;
    if (!parse_volume_unit(to_unit_str, &to_unit)) {
        printf("Incompatible units: '%s' is a volume unit, '%s' is not\n",
               unit_str.c_str(), to_unit_str.c_str());
        return true;
    }

    Volume vol(value, from_unit);
    printf("%s = %s\n", vol.to_string(from_unit).c_str(), vol.to_string(to_unit).c_str());
    return true;
} // ———  END OF function try_convert_volume————————————————————————————————————

}  // namespace unitfy
