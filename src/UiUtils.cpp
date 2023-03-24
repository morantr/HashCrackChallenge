#include "UiUtils.h"

#include <array>

std::pair<double, std::string_view> UiUtils ::build_hash_rate_string(uint64_t rate)
{
    // The units for the hash rate, in increasing order of magnitude
    struct sRate {
        std::string_view rate_str;
        double rate_factor;
    };

    constexpr std::array<sRate, 4> rates = {
        sRate {" Hash/Sec", 1},          //
        sRate {" KHash/Sec", 1000},      //
        sRate {" MHash/Sec", 1000000},   //
        sRate {" GHash/Sec", 1000000000} //
    };

    // Determine the unit based on the intermediate value
    // The branching here could actually contribute to performance since it is expected to use
    // the same branch all the time.
    if (rate >= rates[3].rate_factor) {
        // The hash rate is in the giga hashes per second unit
        return {rate / rates[3].rate_factor, rates[3].rate_str};

    } else if (rate >= rates[2].rate_factor) {
        // The hash rate is in the mega hashes per second unit
        return {rate / rates[2].rate_factor, rates[2].rate_str};

    } else if (rate >= rates[1].rate_factor) {
        // The hash rate is in the kilo hashes per second unit
        return {rate / rates[1].rate_factor, rates[1].rate_str};
    }

    return {rate / rates[0].rate_factor, rates[0].rate_str};
}
