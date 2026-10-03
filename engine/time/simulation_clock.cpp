#include "simulation_clock.h"
#include <sstream>
#include <iomanip>

namespace origin::time {

std::string SimulationClock::to_string() const {
    double sim_time = simulation_time();
    uint64_t days = static_cast<uint64_t>(sim_time / TimeConstants::SECONDS_PER_DAY);
    double remaining = sim_time - days * TimeConstants::SECONDS_PER_DAY;
    uint64_t hours = static_cast<uint64_t>(remaining / TimeConstants::SECONDS_PER_HOUR);
    remaining -= hours * TimeConstants::SECONDS_PER_HOUR;
    uint64_t minutes = static_cast<uint64_t>(remaining / TimeConstants::SECONDS_PER_MINUTE);
    remaining -= minutes * TimeConstants::SECONDS_PER_MINUTE;
    uint64_t seconds = static_cast<uint64_t>(remaining);
    double fractional = remaining - seconds;
    
    std::ostringstream oss;
    oss << "Tick " << tick_ << " | "
        << days << "d " << std::setfill('0') << std::setw(2) << hours << "h "
        << std::setw(2) << minutes << "m " << std::setw(2) << seconds << "s";
    if (fractional > 0.001) {
        oss << "." << std::setw(3) << static_cast<int>(fractional * 1000);
    }
    return oss.str();
}

SimulationClock::CalendarTime SimulationClock::calendar_time(double seconds_per_day) const {
    CalendarTime cal;
    double sim_time = simulation_time();
    
    cal.year = static_cast<uint64_t>(sim_time / TimeConstants::SECONDS_PER_YEAR);
    double year_remainder = sim_time - cal.year * TimeConstants::SECONDS_PER_YEAR;
    
    cal.day_of_year = static_cast<uint32_t>(year_remainder / seconds_per_day);
    double day_remainder = year_remainder - cal.day_of_year * seconds_per_day;
    
    cal.hour = static_cast<uint32_t>(day_remainder / TimeConstants::SECONDS_PER_HOUR);
    double hour_remainder = day_remainder - cal.hour * TimeConstants::SECONDS_PER_HOUR;
    
    cal.minute = static_cast<uint32_t>(hour_remainder / TimeConstants::SECONDS_PER_MINUTE);
    double minute_remainder = hour_remainder - cal.minute * TimeConstants::SECONDS_PER_MINUTE;
    
    cal.second = static_cast<uint32_t>(minute_remainder);
    cal.fractional_second = minute_remainder - cal.second;
    
    return cal;
}

} // namespace origin::time