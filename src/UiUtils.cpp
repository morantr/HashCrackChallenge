#include "UiUtils.h"

#include <array>
#include <cmath>

std::pair<uint64_t, std::string_view> UiUtils::build_hash_rate_string(
    uint64_t hash_counter, uint64_t ref_idx, std::chrono::seconds update_rate)
{
    // Calculate the difference between hash_counter and ref_idx
    uint64_t hash_difference = hash_counter - ref_idx;

    // The constant for converting between units
    constexpr uint unit_conversion_constant = 1000;

    // The units for the hash rate, in increasing order of magnitude
    constexpr std::array<std::string_view, 4> unit_strings = {
        " Hash/Sec", " KHash/Sec", " MHash/Sec", " GHash/Sec"};

    // The index of the selected unit
    int unit_index = 0;

    // Calculate the intermediate value by dividing the hash difference by the update rate
    uint64_t intermediate_value = hash_difference / update_rate.count();

    // Determine the unit based on the intermediate value
    // The branching here could actually contribute to performance since it is expected to use
    // the same branch all the time.
    if (intermediate_value >= std::pow(unit_conversion_constant, 3)) {
        // The hash rate is in the giga hashes per second unit
        unit_index = 3;
    } else if (intermediate_value >= std::pow(unit_conversion_constant, 2)) {
        // The hash rate is in the mega hashes per second unit
        unit_index = 2;
    } else if (intermediate_value >= std::pow(unit_conversion_constant, 1)) {
        // The hash rate is in the kilo hashes per second unit
        unit_index = 1;
    }

    // Calculate the final hash rate
    uint hash_rate = intermediate_value / std::pow(unit_conversion_constant, unit_index);

    // Return the hash rate, and string representing the its unit
    return {hash_rate, unit_strings[unit_index]};
}
