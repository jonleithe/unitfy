#include "delta.h"

#include <cmath>

namespace {

bool nearly_equal(double lhs, double rhs) {
    return std::abs(lhs - rhs) < 1e-9;
}

} // namespace

int main() {
    delta::MDelta app;
    (void)app;

    const delta::Length uk_foot(1.0, delta::LengthUnit::UKFoot);
    if (!nearly_equal(uk_foot.to_unit(delta::LengthUnit::Meter), 0.3048)) {
        return 1;
    }

    const delta::Length us_survey_foot(1.0, delta::LengthUnit::USSurveyFoot);
    if (!nearly_equal(us_survey_foot.to_unit(delta::LengthUnit::Meter), 1200.0 / 3937.0)) {
        return 1;
    }

    const delta::Length one_meter(1.0, delta::LengthUnit::Meter);
    if (!nearly_equal(one_meter.to_unit(delta::LengthUnit::USSurveyFoot), 3937.0 / 1200.0)) {
        return 1;
    }

    return 0;
}
