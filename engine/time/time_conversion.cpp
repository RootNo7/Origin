#include "time_conversion.h"
#include <sstream>
#include <iomanip>
#include <regex>
#include <cctype>

namespace origin::time {

std::string TimeConversion::format_duration(double seconds) {
    if (seconds < 0) seconds = 0;
    
    uint64_t total_seconds = static_cast<uint64_t>(seconds);
    double fractional = seconds - total_seconds;
    
    uint64_t years = total_seconds / static_cast<uint64_t>(TimeConstants::SECONDS_PER_YEAR);
    total_seconds %= static_cast<uint64_t>(TimeConstants::SECONDS_PER_YEAR);
    
    uint64_t days = total_seconds / static_cast<uint64_t>(TimeConstants::SECONDS_PER_DAY);
    total_seconds %= static_cast<uint64_t>(TimeConstants::SECONDS_PER_DAY);
    
    uint64_t hours = total_seconds / static_cast<uint64_t>(TimeConstants::SECONDS_PER_HOUR);
    total_seconds %= static_cast<uint64_t>(TimeConstants::SECONDS_PER_HOUR);
    
    uint64_t minutes = total_seconds / static_cast<uint64_t>(TimeConstants::SECONDS_PER_MINUTE);
    uint64_t secs = total_seconds % static_cast<uint64_t>(TimeConstants::SECONDS_PER_MINUTE);
    
    std::ostringstream oss;
    bool has_output = false;
    
    if (years > 0) {
        oss << years << "y ";
        has_output = true;
    }
    if (days > 0 || has_output) {
        oss << days << "d ";
        has_output = true;
    }
    if (hours > 0 || has_output) {
        oss << std::setfill('0') << std::setw(2) << hours << "h ";
        has_output = true;
    }
    if (minutes > 0 || has_output) {
        oss << std::setw(2) << minutes << "m ";
        has_output = true;
    }
    oss << std::setw(2) << secs << "s";
    
    if (fractional > 0.001) {
        oss << "." << std::setw(3) << std::setfill('0') << static_cast<int>(fractional * 1000);
    }
    
    return oss.str();
}

std::string TimeConversion::format_ticks(uint64_t ticks) {
    return format_duration(ticks_to_seconds(ticks));
}

uint64_t TimeConversion::parse_duration(const std::string& str) {
    // Simple parser for formats like "1y 2d 3h 4m 5s" or "3h 30m"
    std::regex pattern(R"((\d+(?:\.\d+)?)\s*([ydhms]))");
    std::smatch match;
    std::string::const_iterator search_start(str.cbegin());
    double total_seconds = 0.0;
    
    while (std::regex_search(search_start, str.cend(), match, pattern)) {
        double value = std::stod(match[1].str());
        char unit = match[2].str()[0];
        
        switch (unit) {
            case 'y': total_seconds += value * TimeConstants::SECONDS_PER_YEAR; break;
            case 'd': total_seconds += value * TimeConstants::SECONDS_PER_DAY; break;
            case 'h': total_seconds += value * TimeConstants::SECONDS_PER_HOUR; break;
            case 'm': total_seconds += value * TimeConstants::SECONDS_PER_MINUTE; break;
            case 's': total_seconds += value; break;
        }
        
        search_start = match.suffix().first;
    }
    
    return seconds_to_ticks(total_seconds);
}

} // namespace origin::time