#include <cmath>
#include <iomanip>
#include <sstream>

#include "delta.h"



namespace delta {

std::string format_value(double value, int precision) {
    
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

}  // namespace delta