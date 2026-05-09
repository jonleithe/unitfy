/*
 * filename:    time.cpp
 * created:     09 May 2026
 * author:      jole
 * description: Time point conversion implementation for calendar dates,
 *              Julian Day, Modified Julian Day, and Unix epoch values.
 */

#include <cmath>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <string>

#include "delta.h"
#include "unit_helpers.h"



namespace unitfy {

namespace {
    constexpr double kSecondsPerDay = 86400.0;
    constexpr double kUnixEpochJulianDay = 2440587.5;
    constexpr double kJulianDayToModifiedJulianDay = 2400000.5;

    struct CivilDate {
        int year;
        unsigned month;
        unsigned day;
    };

    // Algorithms adapted from Howard Hinnant's public domain civil calendar
    // conversions. They map proleptic Gregorian dates to Unix-epoch days.
    int days_from_civil(int year, unsigned month, unsigned day)
    {
        year -= month <= 2;
        const int era = (year >= 0 ? year : year - 399) / 400;
        const unsigned yoe = static_cast<unsigned>(year - era * 400);
        const unsigned doy = (153 * (month + (month > 2 ? -3 : 9)) + 2) / 5 + day - 1;
        const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
        return era * 146097 + static_cast<int>(doe) - 719468;
    }

    CivilDate civil_from_days(int days)
    {
        days += 719468;
        const int era = (days >= 0 ? days : days - 146096) / 146097;
        const unsigned doe = static_cast<unsigned>(days - era * 146097);
        const unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
        int year = static_cast<int>(yoe) + era * 400;
        const unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
        const unsigned mp = (5 * doy + 2) / 153;
        const unsigned day = doy - (153 * mp + 2) / 5 + 1;
        const unsigned month = mp + (mp < 10 ? 3 : -9);
        year += month <= 2;

        return {year, month, day};
    }

    bool is_leap_year(int year)
    {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }

    unsigned days_in_month(int year, unsigned month)
    {
        static constexpr unsigned kDaysByMonth[] = {
            31, 28, 31, 30, 31, 30,
            31, 31, 30, 31, 30, 31,
        };

        if (month == 2 && is_leap_year(year)) {
            return 29;
        }

        return kDaysByMonth[month - 1];
    }

    CivilDate parse_date(const std::string& date)
    {
        if (date.size() != 10 || date[4] != '-' || date[7] != '-') {
            throw QuantityError(quantity_error_messages::kInvalidDate);
        }

        for (std::size_t i = 0; i < date.size(); ++i) {
            if (i == 4 || i == 7) {
                continue;
            }
            if (date[i] < '0' || date[i] > '9') {
                throw QuantityError(quantity_error_messages::kInvalidDate);
            }
        }

        const int year = std::stoi(date.substr(0, 4));
        const unsigned month = static_cast<unsigned>(std::stoi(date.substr(5, 2)));
        const unsigned day = static_cast<unsigned>(std::stoi(date.substr(8, 2)));

        if (month < 1 || month > 12 || day < 1 || day > days_in_month(year, month)) {
            throw QuantityError(quantity_error_messages::kInvalidDate);
        }

        return {year, month, day};
    }

    std::string format_date(double unix_seconds)
    {
        const int days = static_cast<int>(std::floor(unix_seconds / kSecondsPerDay));
        const CivilDate date = civil_from_days(days);

        char buffer[11];
        std::snprintf(buffer, sizeof(buffer), "%04d-%02u-%02u",
                      date.year, date.month, date.day);

        return std::string(buffer);
    }

    std::string format_time_value(double value)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(format_settings::kTimePrecision) << value;

        return oss.str();
    }
} // namespace



TimePoint::TimePoint(double value, TimeUnit unit)
{
    switch(unit){
        case TimeUnit::Date:
            unix_seconds_ = value * kSecondsPerDay;
            break;
        case TimeUnit::JulianDay:
            unix_seconds_ = (value - kUnixEpochJulianDay) * kSecondsPerDay;
            break;
        case TimeUnit::ModifiedJulianDay:
            unix_seconds_ = (value + kJulianDayToModifiedJulianDay - kUnixEpochJulianDay) * kSecondsPerDay;
            break;
        case TimeUnit::UnixSeconds:
            unix_seconds_ = value;
            break;
        case TimeUnit::UnixMilliseconds:
            unix_seconds_ = value / 1000.0;
            break;
        default:
            throw QuantityError("Invalid time unit");
    }
} // ———  END OF function TimePoint::TimePoint—————————————————————————————————



TimePoint TimePoint::from_date_string(const std::string& date)
{
    const CivilDate civil_date = parse_date(date);
    return TimePoint(static_cast<double>(days_from_civil(civil_date.year,
                                                         civil_date.month,
                                                         civil_date.day)),
                     TimeUnit::Date);
} // ———  END OF function TimePoint::from_date_string——————————————————————————



double TimePoint::to_unit(TimeUnit unit) const
{
    switch(unit){
        case TimeUnit::Date:
            return std::floor(unix_seconds_ / kSecondsPerDay);
        case TimeUnit::JulianDay:
            return unix_seconds_ / kSecondsPerDay + kUnixEpochJulianDay;
        case TimeUnit::ModifiedJulianDay:
            return to_unit(TimeUnit::JulianDay) - kJulianDayToModifiedJulianDay;
        case TimeUnit::UnixSeconds:
            return unix_seconds_;
        case TimeUnit::UnixMilliseconds:
            return unix_seconds_ * 1000.0;
        default:
            throw QuantityError("Invalid time unit");
    }
} // ———  END OF function TimePoint::to_unit———————————————————————————————————



std::string TimePoint::to_string(TimeUnit unit) const
{
    if (unit == TimeUnit::Date) {
        return format_date(unix_seconds_) + " date";
    }

    static const char* names[] = {"date", "JD", "MJD", "unix", "unixms"};

    return format_time_value(to_unit(unit))
           + " "
           + names[static_cast<int>(unit)];
} // ———  END OF function TimePoint::to_string—————————————————————————————————



static constexpr UnitAlias<TimeUnit> kTimeAliases[] = {
    {TimeUnit::Date, "date"},
    {TimeUnit::Date, "calendar"},
    {TimeUnit::Date, "gregorian"},
    {TimeUnit::JulianDay, "jd"},
    {TimeUnit::JulianDay, "julianday"},
    {TimeUnit::JulianDay, "julian day"},
    {TimeUnit::ModifiedJulianDay, "mjd"},
    {TimeUnit::ModifiedJulianDay, "modifiedjulian"},
    {TimeUnit::ModifiedJulianDay, "modifiedjulianday"},
    {TimeUnit::ModifiedJulianDay, "modified julian day"},
    {TimeUnit::UnixSeconds, "unix"},
    {TimeUnit::UnixSeconds, "epoch"},
    {TimeUnit::UnixSeconds, "unixs"},
    {TimeUnit::UnixSeconds, "epochseconds"},
    {TimeUnit::UnixMilliseconds, "unixms"},
    {TimeUnit::UnixMilliseconds, "epochms"},
    {TimeUnit::UnixMilliseconds, "epochmilliseconds"},
}; // ———  END OF kTimeAliases—————————————————————————————————————————————————



static constexpr UnitDisplay<TimeUnit> kTimeDisplay[] = {
    {TimeUnit::Date, "date"},
    {TimeUnit::JulianDay, "JD"},
    {TimeUnit::ModifiedJulianDay, "MJD"},
    {TimeUnit::UnixSeconds, "unix"},
    {TimeUnit::UnixMilliseconds, "unixms"},
}; // ———  END OF kTimeDisplay—————————————————————————————————————————————————



static constexpr TimeUnit kTimeOutputOrder[] = {
    TimeUnit::Date,
    TimeUnit::JulianDay,
    TimeUnit::ModifiedJulianDay,
    TimeUnit::UnixSeconds,
    TimeUnit::UnixMilliseconds,
}; // ———  END OF kTimeOutputOrder—————————————————————————————————————————————



static bool parse_time_unit(const std::string& unit_str,
                            TimeUnit* out_unit)
{
    return parse_unit_aliases(unit_str, kTimeAliases, out_unit);
} // ———  END OF function parse_time_unit——————————————————————————————————————



static void convert_time(const TimePoint& time_point, const std::string& value_str, TimeUnit from_unit)
{
    printf("Time conversion of %s %s:\n", value_str.c_str(),
           display_symbol(from_unit, kTimeDisplay));

    print_all_conversions(time_point, kTimeOutputOrder);
} // ———  END OF function convert_time—————————————————————————————————————————



bool try_convert_time(double value,
                      const std::string& unit_str,
                      const std::string& to_unit_str)
{
    TimeUnit from_unit;

    if(!parse_time_unit(unit_str, &from_unit)){
        return false;
    }

    if (from_unit == TimeUnit::Date) {
        return false;
    }

    TimePoint time_point(value, from_unit);
    const std::string value_str = format_time_value(value);

    if(to_unit_str.empty()){
        convert_time(time_point, value_str, from_unit);
        return true;
    }

    TimeUnit to_unit;

    if(!parse_time_unit(to_unit_str, &to_unit)){
        printf("Incompatible units: '%s' is a time unit, '%s' is not\n",
               unit_str.c_str(), to_unit_str.c_str());

        return true;
    }

    printf("%s = %s\n", time_point.to_string(from_unit).c_str(),
                        time_point.to_string(to_unit).c_str());

    return true;
} // ———  END OF function try_convert_time—————————————————————————————————————



bool try_convert_time(const std::string& value_str,
                      const std::string& unit_str,
                      const std::string& to_unit_str)
{
    TimeUnit from_unit;

    if(!parse_time_unit(unit_str, &from_unit)){
        return false;
    }

    if (from_unit != TimeUnit::Date) {
        std::istringstream iss(value_str);
        double numeric_value;
        char trailing = '\0';
        if (!(iss >> numeric_value) || (iss >> trailing)) {
            throw QuantityError("Time value must be numeric unless unit is date");
        }

        return try_convert_time(numeric_value, unit_str, to_unit_str);
    }

    const TimePoint time_point = TimePoint::from_date_string(value_str);

    if(to_unit_str.empty()){
        convert_time(time_point, value_str, from_unit);
        return true;
    }

    TimeUnit to_unit;

    if(!parse_time_unit(to_unit_str, &to_unit)){
        printf("Incompatible units: '%s' is a time unit, '%s' is not\n",
               unit_str.c_str(), to_unit_str.c_str());

        return true;
    }

    printf("%s = %s\n", time_point.to_string(from_unit).c_str(),
                        time_point.to_string(to_unit).c_str());

    return true;
} // ———  END OF function try_convert_time—————————————————————————————————————

}  // namespace unitfy
