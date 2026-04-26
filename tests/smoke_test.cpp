/*
 * filename:    smoke_test.cpp
 * created:     26 April 2026
 * author:      jole
 * description: Smoke tests for all quantity types (temperature, length,
 *              volume, pressure) covering conversions and error handling.
 */

#include "delta.h"

#include <cmath>
#include <string>

namespace {

bool nearly_equal(double lhs, double rhs, double tol = 1e-9) {
    return std::abs(lhs - rhs) < tol;
}

// Returns true and prints nothing on pass; prints the label and returns false on fail.
bool check(bool condition, const char* label) {
    if (!condition) {
        printf("FAIL: %s\n", label);
    }
    return condition;
}

} // namespace

int main() {
    bool ok = true;

    // ——— Temperature ————————————————————————————————————————————————————————
    {
        const unitfy::Temperature boiling(100.0, unitfy::TemperatureUnit::Celsius);
        ok &= check(nearly_equal(boiling.to_unit(unitfy::TemperatureUnit::Kelvin),    373.15),        "100 C -> K");
        ok &= check(nearly_equal(boiling.to_unit(unitfy::TemperatureUnit::Fahrenheit), 212.0),        "100 C -> F");
        ok &= check(nearly_equal(boiling.to_unit(unitfy::TemperatureUnit::Rankine),   671.67, 1e-5),  "100 C -> R");

        const unitfy::Temperature freezing(32.0, unitfy::TemperatureUnit::Fahrenheit);
        ok &= check(nearly_equal(freezing.to_unit(unitfy::TemperatureUnit::Celsius), 0.0, 1e-9), "32 F -> 0 C");

        const unitfy::Temperature abs_zero(0.0, unitfy::TemperatureUnit::Kelvin);
        ok &= check(nearly_equal(abs_zero.to_unit(unitfy::TemperatureUnit::Celsius), -273.15), "0 K -> -273.15 C");

        bool threw = false;
        try { unitfy::Temperature bad(-1.0, unitfy::TemperatureUnit::Kelvin); }
        catch (const unitfy::QuantityError&) { threw = true; }
        ok &= check(threw, "negative Kelvin throws");
    }

    // ——— Length ——————————————————————————————————————————————————————————————
    {
        const unitfy::Length one_km(1.0, unitfy::LengthUnit::Kilometer);
        ok &= check(nearly_equal(one_km.to_unit(unitfy::LengthUnit::Meter),   1000.0), "1 km -> 1000 m");
        ok &= check(nearly_equal(one_km.to_unit(unitfy::LengthUnit::Mile),    1.0 / 1.609344, 1e-9), "1 km -> mi");

        const unitfy::Length one_inch(1.0, unitfy::LengthUnit::Inch);
        ok &= check(nearly_equal(one_inch.to_unit(unitfy::LengthUnit::Meter), 0.0254), "1 in -> 0.0254 m");

        const unitfy::Length uk_foot(1.0, unitfy::LengthUnit::UKFoot);
        ok &= check(nearly_equal(uk_foot.to_unit(unitfy::LengthUnit::Meter),  0.3048), "1 ukft -> 0.3048 m");

        const unitfy::Length us_survey_foot(1.0, unitfy::LengthUnit::USSurveyFoot);
        ok &= check(nearly_equal(us_survey_foot.to_unit(unitfy::LengthUnit::Meter), 1200.0 / 3937.0), "1 usft -> m");

        const unitfy::Length one_meter(1.0, unitfy::LengthUnit::Meter);
        ok &= check(nearly_equal(one_meter.to_unit(unitfy::LengthUnit::USSurveyFoot), 3937.0 / 1200.0), "1 m -> usft");

        bool threw = false;
        try { unitfy::Length bad(-1.0, unitfy::LengthUnit::Meter); }
        catch (const unitfy::QuantityError&) { threw = true; }
        ok &= check(threw, "negative length throws");
    }

    // ——— Volume ——————————————————————————————————————————————————————————————
    {
        const unitfy::Volume one_liter(1.0, unitfy::VolumeUnit::Liter);
        ok &= check(nearly_equal(one_liter.to_unit(unitfy::VolumeUnit::Milliliter),  1000.0),          "1 L -> 1000 mL");
        ok &= check(nearly_equal(one_liter.to_unit(unitfy::VolumeUnit::CubicMeter),  0.001),            "1 L -> 0.001 m3");
        ok &= check(nearly_equal(one_liter.to_unit(unitfy::VolumeUnit::Gallon),      1.0 / 3.78541, 1e-5), "1 L -> gal");

        const unitfy::Volume one_gallon(1.0, unitfy::VolumeUnit::Gallon);
        ok &= check(nearly_equal(one_gallon.to_unit(unitfy::VolumeUnit::Liter), 3.78541, 1e-5), "1 gal -> L");

        bool threw = false;
        try { unitfy::Volume bad(-1.0, unitfy::VolumeUnit::Liter); }
        catch (const unitfy::QuantityError&) { threw = true; }
        ok &= check(threw, "negative volume throws");
    }

    // ——— Pressure ————————————————————————————————————————————————————————————
    {
        const unitfy::Pressure one_atm(1.0, unitfy::PressureUnit::Atmosphere);
        ok &= check(nearly_equal(one_atm.to_unit(unitfy::PressureUnit::Pascal),     101325.0),               "1 atm -> Pa");
        ok &= check(nearly_equal(one_atm.to_unit(unitfy::PressureUnit::Kilopascal), 101.325),                 "1 atm -> kPa");
        ok &= check(nearly_equal(one_atm.to_unit(unitfy::PressureUnit::Bar),        1.01325, 1e-9),           "1 atm -> bar");
        ok &= check(nearly_equal(one_atm.to_unit(unitfy::PressureUnit::Psi),        14.6959, 1e-4),           "1 atm -> psi");

        const unitfy::Pressure one_bar(1.0, unitfy::PressureUnit::Bar);
        ok &= check(nearly_equal(one_bar.to_unit(unitfy::PressureUnit::Pascal), 100000.0), "1 bar -> Pa");

        bool threw = false;
        try { unitfy::Pressure bad(-1.0, unitfy::PressureUnit::Pascal); }
        catch (const unitfy::QuantityError&) { threw = true; }
        ok &= check(threw, "negative pressure throws");
    }

    if (ok) {
        printf("All tests passed.\n");
    }

    return ok ? 0 : 1;
}
