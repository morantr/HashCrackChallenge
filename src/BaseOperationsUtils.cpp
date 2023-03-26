#include "BaseOperationsUtils.h"

#include <cmath>
#include <iostream>

std::string BaseOperationsUtils::decimal_to_base_x(uint32_t decimal, std::string_view base_characters)
{
    std::string result;

    do {
        int digit = decimal % base_characters.size();
        result += base_characters[digit];
        decimal /= base_characters.size();
    } while (decimal > 0);

    std::reverse(result.begin(), result.end());
    return result;
}

/**
 * @note 'Find' operation on a @a std::string_view is faster than doing it on a @a std::map or on a
 * @a std::unordered_map.
 * In addition, this function 15 timesfaster than using std::stoi to convert the strings to
 * integers, sum them, and converting it back to string with std::ostringstream, and 5 times faster
 * if replacing the std::ostringstream with sprintf.
 */
std::string BaseOperationsUtils::sum_base_x_integers(
    std::string_view int1, std::string_view int2, std::string_view base_characters)
{
    const size_t size_max    = std::max(int1.length(), int2.length());
    const size_t buffer_size = size_max + 2; // placeholder for last carry and null terminator

    constexpr uint8_t max_int_str_size = 20;
    std::array<char, max_int_str_size> result_buffer;

    result_buffer[buffer_size - 1] = 0; // Assign null terminator

    // Find relative position of each integer.
    size_t int1_pos = buffer_size - int1.length() - 1;
    size_t int2_pos = buffer_size - int2.length() - 1;

    uint8_t carry = 0;
    // Start from 'buffer_size - 2' to eliminate null terminator and size to index shift.
    for (uint32_t i = buffer_size - 2; i < buffer_size; --i) {
        uint8_t sum = carry;

        if (i >= int1_pos) {
            sum += base_characters.find(int1[i - int1_pos]);
        }
        if (i >= int2_pos) {
            sum += base_characters.find(int2[i - int2_pos]);
        }
        carry            = sum / base_characters.length();
        result_buffer[i] = base_characters[sum % base_characters.length()];
    }

    std::string result;
    const char *first_char = result_buffer[0] != '0' ? &result_buffer[0] : &result_buffer[1];
    result.assign(first_char);
    return result;
}

void BaseOperationsUtils::increment_base_x_integer(
    std::string &int_str, std::string_view base_characters)
{
    if (int_str.empty()) {
        int_str.insert(int_str.begin(), base_characters[0]);
        return;
    }

    uint8_t carry = 1;
    for (int i = int_str.length() - 1; i >= 0; --i) {
        uint8_t sum = base_characters.find(int_str[i]) + carry;
        carry       = sum / base_characters.length();
        int_str[i]  = base_characters[sum % base_characters.length()];

        if (!carry) {
            break;
        }
    }

    if (carry) {
        int_str.insert(int_str.begin(), '1');
    }
}
