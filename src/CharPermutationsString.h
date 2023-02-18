
#pragma once

#include <map>
#include <string>
#include <string_view>

class CharPermutationsString {
  public:
    CharPermutationsString() = delete;

    bool set_task(const std::string &start_str, uint32_t max_permutations);

    const std::string &get_next_permutation();
    static void init(const std::string_view &valid_characters);

  private:
    void calc_last_string();
    uint32_t counter;
    uint32_t permutations_counter;
    std::string m_integer_str;

    const uint32_t m_max_permutations;
    static std::string_view m_valid_characters;
};
