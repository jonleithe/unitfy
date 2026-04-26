/*
 * filename:    format.cpp
 * created:     26 April 2026
 * author:      jole
 * description: Numeric formatting helpers for conversion output.
 */

#include <cmath>
#include <iomanip>
#include <sstream>

#include "delta.h"



namespace unitfy {

// Format a double value with the specified precision, using fixed-point notation
// for typical values and scientific notation for very small or very large values.
std::string format_value(double value, int precision)
{   
    // Use scientific notation for very small or very large values.
    if(value != 0.0 && (std::abs(value) < 1e-3 || std::abs(value) > 1e6)){
        std::ostringstream oss;

        oss << std::scientific << std::setprecision(precision) << value;
        
        return oss.str();
    } // ——— END OF scientific notation check———————————————————————————————————

    std::ostringstream oss;

    oss << std::fixed << std::setprecision(precision) << value;
    
    return oss.str();
} // ———  END OF function format_value——————————————————————————————————————————

}  // namespace unitfy