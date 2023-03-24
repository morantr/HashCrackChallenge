#pragma once

#include <chrono>
#include <string_view>
#include <utility>

class UiUtils {
  public:
    /**
     * @brief Build a human readable hash rate.
     *
     * @note This function is limmeted to rate of 4 GHash/Sec due to max value of uint64_t.
     *
     * @param rate Hash rate per second.
     * @return std::pair<uint64_t, std::string_view> The pair first is the rate numering value, and
     * the pair second is the string representing the unit per seconds.
     */
    static std::pair<double, std::string_view> build_hash_rate_string(uint64_t rate);
};
