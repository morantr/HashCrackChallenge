#pragma once

#include <chrono>

/**
 * @brief The Statistics class purpose is the calculate the hash rate in hashes/seconds.
 */

class Statistics {
  public:
    Statistics();

    /**
     * @brief Calculate the hash rate per second.
     *
     * @return Rate of number of hashes checked per a second.
     */
    uint64_t calculate_hash_rate_per_second(uint64_t hash_counter);

  private:
    // Reference to the state of the previous cycle
    uint64_t m_counter_reference;
    std::chrono::steady_clock::time_point m_time_reference;
};
