/*
 * filename:    unit_helpers.h
 * created:     26 April 2026
 * author:      jole
 * description: Internal helpers for unit string parsing and normalisation.
 */

#pragma once

#include <cctype>
#include <cstdio>
#include <string>
#include <algorithm>



template <typename Unit>
struct UnitAlias{
    Unit unit;
    const char* alias;
};



template <typename Unit>
struct UnitDisplay{
    Unit unit;
    const char* symbol;
};



inline std::string to_lower_copy(const std::string& value){
    std::string lower = value;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    return lower;
}



template <typename Unit, std::size_t N>
bool parse_unit_aliases(const std::string& unit_str,
                        const UnitAlias<Unit> (&aliases)[N],
                        Unit* out_unit){
    const std::string lower = to_lower_copy(unit_str);
    for (const UnitAlias<Unit>& entry : aliases) {
        if (lower == entry.alias) {
            *out_unit = entry.unit;
            return true;
        }
    }

    return false;
}



template <typename Unit, std::size_t N>
const char* display_symbol(Unit unit,
                           const UnitDisplay<Unit> (&display_map)[N]) {
    for (const UnitDisplay<Unit>& entry : display_map) {
        if (entry.unit == unit){
            return entry.symbol;
        }
    }

    return "?";
}



template <typename Quantity, typename Unit, std::size_t N>
void print_all_conversions(const Quantity& quantity,
                           const Unit (&output_units)[N]){
    for (const Unit unit : output_units) {
        printf("  %s\n", quantity.to_string(unit).c_str());
    }
}
