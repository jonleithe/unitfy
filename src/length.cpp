#include <cstdio>

#include "delta.h"
#include "unit_helpers.h"



namespace delta{

namespace {
constexpr double kInternationalFootInMeters = 0.3048;
constexpr double kUSSurveyFootInMeters = 1200.0 / 3937.0;
} // namespace

Length::Length(double value, LengthUnit unit){
    
    switch (unit){
        case LengthUnit::Millimeter:    meters_ = value / 1000.0;
                                        break;
        case LengthUnit::Centimeter:    meters_ = value / 100.0;
                                        break;
        case LengthUnit::Meter:         meters_ = value;
                                        break;
        case LengthUnit::Kilometer:     meters_ = value * 1000.0;
                                        break;
        case LengthUnit::Inch:          meters_ = value * 0.0254;
                                        break;
        case LengthUnit::Foot:          meters_ = value * kInternationalFootInMeters;
                                        break;
        case LengthUnit::USSurveyFoot:  meters_ = value * kUSSurveyFootInMeters;
                                        break;
        case LengthUnit::UKFoot:        meters_ = value * kInternationalFootInMeters;
                                        break;
        case LengthUnit::Mile:          meters_ = value * 1609.344;
                                        break;
        default:                        throw QuantityError("Invalid length unit");
    } // ———  END OF switch(unit)———————————————————————————————————————————————

    if (meters_ < 0.0) {
        throw QuantityError(quantity_error_messages::kLengthNegative);
    } // ———  END OF if(meters_ < 0)————————————————————————————————————————————

} // ———  END OF function Length::Length————————————————————————————————————————



double Length::to_unit(LengthUnit unit) const{
    switch (unit){
        case LengthUnit::Millimeter:    return meters_ * 1000.0;
        case LengthUnit::Centimeter:    return meters_ * 100.0;
        case LengthUnit::Meter:         return meters_;
        case LengthUnit::Kilometer:     return meters_ / 1000.0;
        case LengthUnit::Inch:          return meters_ / 0.0254;
        case LengthUnit::Foot:          return meters_ / kInternationalFootInMeters;
        case LengthUnit::USSurveyFoot:  return meters_ / kUSSurveyFootInMeters;
        case LengthUnit::UKFoot:        return meters_ / kInternationalFootInMeters;
        case LengthUnit::Mile:          return meters_ / 1609.344;
        default:                        throw QuantityError("Invalid length unit");
    } // ———  END OF switch(unit)———————————————————————————————————————————————
} // ———  END OF function Length::to_unit———————————————————————————————————————



std::string Length::to_string(LengthUnit unit) const{
    static const char* names[] = {"mm", "cm", "m", "km", "in", "ft", "usft", "ukft", "mi"};
    
    return format_value(to_unit(unit),
                        format_settings::kLengthPrecision) 
                        + " " 
                        + names[static_cast<int>(unit)];
} // ———  END OF function Length::to_string—————————————————————————————————————



Length Length::operator+(const Length& other) const{
    Length result(meters_, LengthUnit::Meter);
    result.meters_ = this->meters_ + other.meters_;

    return result;    
} // ———  END OF function Length::operator+—————————————————————————————————————



Length Length::operator-(const Length& other) const{
    Length result(meters_, LengthUnit::Meter);
    result.meters_ = this->meters_ - other.meters_;
    if (result.meters_ < 0.0) {
        throw QuantityError(quantity_error_messages::kLengthSubNegative);
    }

    return result;
} // ———  END OF function Length::operator-—————————————————————————————————————



Length Length::operator*(double scalar) const{
    Length result(meters_, LengthUnit::Meter);
    result.meters_ = this->meters_ * scalar;
    if (result.meters_ < 0.0) {
        throw QuantityError(quantity_error_messages::kLengthMulNegative);
    }

    return result;
} // ———  END OF function Length::operator*—————————————————————————————————————



Length Length::operator/(double scalar) const{
    if (scalar == 0.0) {
        throw QuantityError(quantity_error_messages::kDivisionByZero);
    }

    Length result(meters_, LengthUnit::Meter);
    result.meters_ = this->meters_ / scalar;

    if (result.meters_ < 0.0) {
        throw QuantityError(quantity_error_messages::kLengthDivNegative);
    }

    return result;
} // ———  END OF function Length::operator/—————————————————————————————————————



Length operator*(double scalar, const Length& len){
    return len * scalar;
} // ———  END OF function operator*(Length)—————————————————————————————————————



static constexpr UnitAlias<LengthUnit> kLengthAliases[] = {
    {LengthUnit::Millimeter, "mm"},
    {LengthUnit::Millimeter, "millimeter"},
    {LengthUnit::Millimeter, "millimeters"},
    {LengthUnit::Centimeter, "cm"},
    {LengthUnit::Centimeter, "centimeter"},
    {LengthUnit::Centimeter, "centimeters"},
    {LengthUnit::Meter, "m"},
    {LengthUnit::Meter, "meter"},
    {LengthUnit::Meter, "meters"},
    {LengthUnit::Kilometer, "km"},
    {LengthUnit::Kilometer, "kilometer"},
    {LengthUnit::Kilometer, "kilometers"},
    {LengthUnit::Inch, "in"},
    {LengthUnit::Inch, "inch"},
    {LengthUnit::Inch, "inches"},
    {LengthUnit::Foot, "ft"},
    {LengthUnit::Foot, "foot"},
    {LengthUnit::Foot, "feet"},
    {LengthUnit::USSurveyFoot, "usft"},
    {LengthUnit::USSurveyFoot, "us ft"},
    {LengthUnit::USSurveyFoot, "us foot"},
    {LengthUnit::USSurveyFoot, "us feet"},
    {LengthUnit::USSurveyFoot, "surveyfoot"},
    {LengthUnit::USSurveyFoot, "surveyfeet"},
    {LengthUnit::USSurveyFoot, "survey foot"},
    {LengthUnit::USSurveyFoot, "survey feet"},
    {LengthUnit::UKFoot, "ukft"},
    {LengthUnit::UKFoot, "uk ft"},
    {LengthUnit::UKFoot, "uk foot"},
    {LengthUnit::UKFoot, "uk feet"},
    {LengthUnit::UKFoot, "imperialfoot"},
    {LengthUnit::UKFoot, "imperialfeet"},
    {LengthUnit::UKFoot, "imperial foot"},
    {LengthUnit::UKFoot, "imperial feet"},
    {LengthUnit::Mile, "mi"},
    {LengthUnit::Mile, "mile"},
    {LengthUnit::Mile, "miles"},
};

static constexpr UnitDisplay<LengthUnit> kLengthDisplay[] = {
    {LengthUnit::Millimeter, "mm"},
    {LengthUnit::Centimeter, "cm"},
    {LengthUnit::Meter, "m"},
    {LengthUnit::Kilometer, "km"},
    {LengthUnit::Inch, "in"},
    {LengthUnit::Foot, "ft"},
    {LengthUnit::USSurveyFoot, "usft"},
    {LengthUnit::UKFoot, "ukft"},
    {LengthUnit::Mile, "mi"},
};

static constexpr LengthUnit kLengthOutputOrder[] = {
    LengthUnit::Millimeter,
    LengthUnit::Centimeter,
    LengthUnit::Meter,
    LengthUnit::Kilometer,
    LengthUnit::Inch,
    LengthUnit::Foot,
    LengthUnit::USSurveyFoot,
    LengthUnit::UKFoot,
    LengthUnit::Mile,
};

static bool parse_length_unit(const std::string& unit_str,
                              LengthUnit* out_unit){
    return parse_unit_aliases(unit_str, kLengthAliases, out_unit);
} // ———  END OF function parse_length_unit—————————————————————————————————————



static void convert_length(double value, LengthUnit from_unit){
    Length len(value, from_unit);
    printf("Length conversion of %.4f %s:\n", value,
           display_symbol(from_unit, kLengthDisplay));
    print_all_conversions(len, kLengthOutputOrder);
} // ———  END OF function convert_length————————————————————————————————————————



bool try_convert_length(double value, const std::string& unit_str) {
    LengthUnit parsed_unit;
    if (!parse_length_unit(unit_str, &parsed_unit)) {
        return false;
    }

    convert_length(value, parsed_unit);

    return true;
} // ———  END OF function try_convert_length————————————————————————————————————

}  // namespace delta
