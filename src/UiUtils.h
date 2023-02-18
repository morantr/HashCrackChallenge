#pragma once

#include <chrono>
#include <string_view>
#include <utility>

class UiUtils {
  public:
    /**
     * @brief Calculate the hash rate between to counters, and build a human readable hash units.
     *
     * @note This function is limmeted to rate of 4 GHash/Sec due to max value of uint64_t.
     *
     * @param hash_counter Hash counter of the latest hash calculation.
     * @param ref_idx Reference hash counter.
     * @param update_rate How many seconds passed between @a hash_counter and @a ref_idx.
     * @return std::pair<uint64_t, std::string_view> The pair first is the rate numering value, and
     * the pair second is the string representing the unit per seconds.
     */
    static std::pair<uint64_t, std::string_view> build_hash_rate_string(
        uint64_t hash_counter, uint64_t ref_idx, std::chrono::seconds update_rate);
};
