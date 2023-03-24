#include "Statistics.h"

#include <iostream>

Statistics::Statistics() :
    m_counter_reference(0), m_time_reference(std::chrono::steady_clock::now())
{
}

uint64_t Statistics::calculate_hash_rate_per_second(uint64_t hash_counter)
{
    // calculate the time difference in milliseconds between the last function call to now.
    auto time_delta_ms = (std::chrono::duration_cast<std::chrono::milliseconds>(
                              std::chrono::steady_clock::now() - m_time_reference))
                             .count();

    if (time_delta_ms == 0) {
        return -1;
    }

    // Update the time reference (it will not be used in the rest of the function)
    m_time_reference = std::chrono::steady_clock::now();

    uint64_t hash_delta = hash_counter - m_counter_reference;

    // Update the counter reference (it will not be used in the rest of the function)
    m_counter_reference = hash_counter;

    constexpr auto milliseconds_in_second = 1000.0;

    // Calculate the final hash rate
    return hash_delta / (time_delta_ms / milliseconds_in_second);
}
