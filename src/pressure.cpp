/*
 * filename:    pressure.cpp
 * created:     26 April 2026
 * author:      jole
 * description: Pressure quantity type and unit conversion implementation.
 */

#include <cstdio>

#include "delta.h"
#include "unit_helpers.h"



namespace unitfy {

Pressure::Pressure(double value, PressureUnit unit){
    switch (unit){
        case PressureUnit::Pascal:      pascal_ = value;
                                        break;
        case PressureUnit::Kilopascal:  pascal_ = value * 1000.0;
                                        break;
        case PressureUnit::Bar:         pascal_ = value * 100000.0;
                                        break;
        case PressureUnit::Atmosphere:  pascal_ = value * 101325.0;
                                        break;
        case PressureUnit::Psi:         pascal_ = value * 6894.757293168;
                                        break;
        case PressureUnit::Torr:        pascal_ = value * 133.322368421;
                                        break;
        case PressureUnit::MmHg:        pascal_ = value * 133.322387415;
                                        break;
        default:                        throw QuantityError("Invalid pressure unit");
    } // ———  END OF switch(unit)———————————————————————————————————————————————

    if (pascal_ < 0.0) {
        throw QuantityError(quantity_error_messages::kPressureNegative);
    }
} // ———  END OF function Pressure::Pressure————————————————————————————————————



double Pressure::to_unit(PressureUnit unit) const{
    switch (unit){
        case PressureUnit::Pascal:      return pascal_;
        case PressureUnit::Kilopascal:  return pascal_ / 1000.0;
        case PressureUnit::Bar:         return pascal_ / 100000.0;
        case PressureUnit::Atmosphere:  return pascal_ / 101325.0;
        case PressureUnit::Psi:         return pascal_ / 6894.757293168;
        case PressureUnit::Torr:        return pascal_ / 133.322368421;
        case PressureUnit::MmHg:        return pascal_ / 133.322387415;
        default:                        throw QuantityError("Invalid pressure unit");
    } // ———  END OF switch(unit)———————————————————————————————————————————————
} // ———  END OF function Pressure::to_unit—————————————————————————————————————



std::string Pressure::to_string(PressureUnit unit) const{
    static const char* names[] = {"Pa", "kPa", "bar", "atm", "psi", "torr", "mmHg"};

    return format_value(to_unit(unit), format_settings::kPressurePrecision) + " " + names[static_cast<int>(unit)];
} // ———  END OF function Pressure::to_string———————————————————————————————————



Pressure Pressure::operator+(const Pressure& other) const{
    Pressure result(pascal_, PressureUnit::Pascal);
    result.pascal_ = this->pascal_ + other.pascal_;

    return result;
} // ———  END OF function Pressure::operator+———————————————————————————————————



Pressure Pressure::operator-(const Pressure& other) const{
    Pressure result(pascal_, PressureUnit::Pascal);
    result.pascal_ = this->pascal_ - other.pascal_;
    if (result.pascal_ < 0.0) {
        throw QuantityError(quantity_error_messages::kPressureSubNegative);
    }

    return result;
} // ———  END OF function Pressure::operator-———————————————————————————————————



Pressure Pressure::operator*(double scalar) const{
    Pressure result(pascal_, PressureUnit::Pascal);
    result.pascal_ = this->pascal_ * scalar;
    if (result.pascal_ < 0.0) {
        throw QuantityError(quantity_error_messages::kPressureMulNegative);
    }

    return result;
} // ———  END OF function Pressure::operator*———————————————————————————————————



Pressure Pressure::operator/(double scalar) const{
    if (scalar == 0.0) {
        throw QuantityError(quantity_error_messages::kDivisionByZero);
    }

    Pressure result(pascal_, PressureUnit::Pascal);
    result.pascal_ = this->pascal_ / scalar;
    if (result.pascal_ < 0.0) {
        throw QuantityError(quantity_error_messages::kPressureDivNegative);
    }

    return result;
} // ———  END OF function Pressure::operator/———————————————————————————————————



Pressure operator*(double scalar, const Pressure& p) {
    return p * scalar;
} // ———  END OF function operator*(Pressure)———————————————————————————————————



static constexpr UnitAlias<PressureUnit> kPressureAliases[] = {
    {PressureUnit::Pascal, "pa"},
    {PressureUnit::Pascal, "pascal"},
    {PressureUnit::Pascal, "pascals"},
    {PressureUnit::Kilopascal, "kpa"},
    {PressureUnit::Kilopascal, "kilopascal"},
    {PressureUnit::Kilopascal, "kilopascals"},
    {PressureUnit::Bar, "bar"},
    {PressureUnit::Atmosphere, "atm"},
    {PressureUnit::Atmosphere, "atmosphere"},
    {PressureUnit::Atmosphere, "atmospheres"},
    {PressureUnit::Psi, "psi"},
    {PressureUnit::Torr, "torr"},
    {PressureUnit::MmHg, "mmhg"},
};

static constexpr UnitDisplay<PressureUnit> kPressureDisplay[] = {
    {PressureUnit::Pascal, "Pa"},
    {PressureUnit::Kilopascal, "kPa"},
    {PressureUnit::Bar, "bar"},
    {PressureUnit::Atmosphere, "atm"},
    {PressureUnit::Psi, "psi"},
    {PressureUnit::Torr, "torr"},
    {PressureUnit::MmHg, "mmHg"},
};

static constexpr PressureUnit kPressureOutputOrder[] = {
    PressureUnit::Pascal,
    PressureUnit::Kilopascal,
    PressureUnit::Bar,
    PressureUnit::Atmosphere,
    PressureUnit::Psi,
    PressureUnit::Torr,
    PressureUnit::MmHg,
};

static bool parse_pressure_unit(const std::string& unit_str,
                                PressureUnit* out_unit){
    return parse_unit_aliases(unit_str, kPressureAliases, out_unit);
} // ———  END OF function parse_pressure_unit———————————————————————————————————



static void convert_pressure(double value, PressureUnit from_unit){
    Pressure p(value, from_unit);
    printf("Pressure conversion of %.4f %s:\n", value,
           display_symbol(from_unit, kPressureDisplay));
    print_all_conversions(p, kPressureOutputOrder);
} // ———  END OF function convert_pressure——————————————————————————————————————



bool try_convert_pressure(double value, const std::string& unit_str, const std::string& to_unit_str){
    PressureUnit from_unit;
    if (!parse_pressure_unit(unit_str, &from_unit)) {
        return false;
    }

    if (to_unit_str.empty()) {
        convert_pressure(value, from_unit);
        return true;
    }

    PressureUnit to_unit;
    if (!parse_pressure_unit(to_unit_str, &to_unit)) {
        printf("Incompatible units: '%s' is a pressure unit, '%s' is not\n",
               unit_str.c_str(), to_unit_str.c_str());
        return true;
    }

    Pressure p(value, from_unit);
    printf("%s = %s\n", p.to_string(from_unit).c_str(), p.to_string(to_unit).c_str());
    return true;
} // ———  END OF function try_convert_pressure——————————————————————————————————

}  // namespace unitfy