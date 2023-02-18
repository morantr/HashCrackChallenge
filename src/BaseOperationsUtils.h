#pragma once

#include <gtest/gtest.h>
#include <string>
#include <string_view>

class BaseOperationsUtils {
  public:
    /**
     * @brief Convert a @a decimal number to a base with given @a base_characters.
     * This function is needed since the standard library doesn't provide, a way to convert to any
     * base.
     *
     * @param decimal Decimal number to convert.
     * @param base_characters List of new base characters.
     * @return String of the decimal converted to base.
     *
     * @example decimal_to_base_x(420, {'0','1',..,'F'}) -> "1A4".
     */
    static std::string decimal_to_base_x(uint decimal, const std::string_view &base_characters);

    /**
     * @brief Sums two integers in base X, and return the result in base X.
     *
     * @param int1 Integer 1 to sum.
     * @param int2 Integer 2 to sum.
     * @param base_characters Characters of the base.
     * @return std::string The sum of the two integers in base X.
     */
    static std::string sum_base_x_integers(
        const std::string &int1, const std::string &int2, const std::string_view &base_characters);

    /**
     * @brief Increment integer @a int_str of base X by 1.
     *
     * @param int_str Integer to increment.
     * @param base_characters Characters of the base.
     */
    static void increment_base_x_integer(
        std::string &int_str, const std::string_view &base_characters);
};
