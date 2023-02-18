#include "../BaseOperationsUtils.h"
#include <gtest/gtest.h>
#include <sha256.h>
#include <tuple>
TEST(BaseOperationsUtils, decimal_to_base_x)
{
    std::string_view base_characters_10 = "0123456789";
    std::string_view base_characters_16 = "0123456789abcdef";
    EXPECT_STREQ(BaseOperationsUtils::decimal_to_base_x(1234, base_characters_10).c_str(), "1234");
    EXPECT_STREQ(BaseOperationsUtils::decimal_to_base_x(10, base_characters_10).c_str(), "10");
    EXPECT_STREQ(BaseOperationsUtils::decimal_to_base_x(1000, base_characters_16).c_str(), "3e8");
    EXPECT_STREQ(BaseOperationsUtils::decimal_to_base_x(4095, base_characters_16).c_str(), "fff");
    EXPECT_STREQ(BaseOperationsUtils::decimal_to_base_x(4096, base_characters_16).c_str(), "1000");
    EXPECT_STREQ(BaseOperationsUtils::decimal_to_base_x(43981, base_characters_16).c_str(), "abcd");
}

TEST(BaseOperationsUtils, sum_base_x_integers)
{
    std::string_view bc10 = "0123456789";
    std::string_view bc16 = "0123456789abcdef";
    EXPECT_STREQ(BaseOperationsUtils::sum_base_x_integers("91", "9999", bc10).c_str(), "10090");
    EXPECT_STREQ(BaseOperationsUtils::sum_base_x_integers("9999", "9999", bc10).c_str(), "19998");
    EXPECT_STREQ(BaseOperationsUtils::sum_base_x_integers("10", "88", bc10).c_str(), "98");
    EXPECT_STREQ(BaseOperationsUtils::sum_base_x_integers("0ff", "ffff", bc16).c_str(), "100fe");
    EXPECT_STREQ(BaseOperationsUtils::sum_base_x_integers("ffff", "ffff", bc16).c_str(), "1fffe");
    EXPECT_STREQ(BaseOperationsUtils::sum_base_x_integers("0000", "0000", bc16).c_str(), "0000");
    EXPECT_STREQ(BaseOperationsUtils::sum_base_x_integers("1", "99f", bc16).c_str(), "9a0");
}

TEST(BaseOperationsUtils, increment_base_x_integer)
{
    std::string_view bc10 = "0123456789";
    std::string_view bc16 = "0123456789abcdef";
    std::string str;
    str.assign("98");
    BaseOperationsUtils::increment_base_x_integer(str, bc10);
    EXPECT_STREQ(str.c_str(), "99");

    str.assign("99");
    BaseOperationsUtils::increment_base_x_integer(str, bc10);
    EXPECT_STREQ(str.c_str(), "100");

    str.assign("99");
    BaseOperationsUtils::increment_base_x_integer(str, bc16);
    EXPECT_STREQ(str.c_str(), "9a");

    str.assign("fe");
    BaseOperationsUtils::increment_base_x_integer(str, bc16);
    EXPECT_STREQ(str.c_str(), "ff");

    str.assign("ff");
    BaseOperationsUtils::increment_base_x_integer(str, bc16);
    EXPECT_STREQ(str.c_str(), "100");
}
