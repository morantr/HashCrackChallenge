
#pragma once

#include <map>
#include <string>
#include <string_view>

class CharPermutationsString {
  public:
    explicit CharPermutationsString(const std::string &start_str, uint32_t max_permutations);

    const std::string &get_last_string() const;
    const std::string &get_next_permutation();
    static void init(const std::string_view &valid_characters);

  private:
    void calc_last_string();

    std::string m_string;
    std::string m_last_string;
    const uint32_t m_max_permutations;
    static std::string_view m_valid_characters;
};
