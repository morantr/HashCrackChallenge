#include "Utils.h"

#include <cmath>
#include <iostream>

std::string Utils::decimal_to_base_x(uint decimal, const std::string_view &base_characters)
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
std::string Utils::sum_base_x_integers(
    const std::string &int1, const std::string &int2, const std::string_view &base_characters)
{
    size_t size_max    = std::max(int1.length(), int2.length());
    size_t buffer_size = size_max + 2; // placeholder for last carry and null terminator

    char result_buffer[buffer_size];
    result_buffer[buffer_size - 1] = 0; // Assign null terminator

    // Find relative position of each integer.
    size_t int1_pos = buffer_size - int1.length() - 1;
    size_t int2_pos = buffer_size - int2.length() - 1;

    uint8_t carry = 0;
    // Start from 'buffer_size - 2' to eliminate null terminator and size to index shift.
    for (int i = buffer_size - 2; i >= 0; --i) {
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
    char *first_char = *result_buffer != '0' ? result_buffer : result_buffer + 1;
    result.assign(first_char);
    return result;
}

void Utils::increment_base_x_integer(std::string &int_str, const std::string_view &base_characters)
{
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
