#include "CharPermutationsString.h"

#include "Utils.h"

#include <exception>

// const std::string_view CharPermutationsString::m_valid_characters =
//     "0123456789abcdefghijklmnopqrstuvwxyz";

std::string_view CharPermutationsString::m_valid_characters;

CharPermutationsString::CharPermutationsString(
    const std::string &start_str, uint32_t max_permutations) :
    m_string(start_str),
    m_max_permutations(max_permutations)
{
    if (m_valid_characters.empty()) {
        std::cout << "ERROR: CharPermutationsString is not initialized!\n";
        throw std::logic_error("CharPermutationsString is not initialized!");
    }
    calc_last_string();
}

void CharPermutationsString::init(const std::string_view &valid_characters)
{
    if (!m_valid_characters.empty()) {
        return;
    }
    m_valid_characters = valid_characters;
}

const std::string &CharPermutationsString::get_last_string() const { return m_last_string; }

const std::string &CharPermutationsString::get_next_permutation()
{
    Utils::increment_base_x_integer(m_string, m_valid_characters);
    return m_string;
}

void CharPermutationsString::calc_last_string()
{
    auto max_permutations_base_x = Utils::decimal_to_base_x(m_max_permutations, m_valid_characters);
    m_last_string =
        Utils::sum_base_x_integers(m_string, max_permutations_base_x, m_valid_characters);
}
