#include "../BaseOperationsUtils.h"
#include "../UiUtils.h"
#include "../external/include/base64.h"

#include <gtest/gtest.h>
#include <sha256.h>
#include <tuple>

// Demonstrate some base64_encode
TEST(Base64, base64_encode)
{
    // Expect two strings not to be equal.
    std::string str("Man");
    auto b64 = base64_encode((const uint8_t *)str.c_str(), str.size());
    EXPECT_STREQ("TWFu", b64.c_str());
}

TEST(Flow, DemoPassword)
{
    constexpr std::string_view salt   = "IEEE";
    constexpr std::string_view pepper = "Xtreme";
    std::string passphrase("password1");
    std::string decrypted_pass;
    decrypted_pass.reserve(salt.size() + passphrase.size() + pepper.size());
    decrypted_pass.append(salt).append(passphrase).append(pepper);

    std::cout << "decrypted_pass: " << decrypted_pass << '\n';

    SHA256 sha256;
    auto encrypted_hex_str = sha256(decrypted_pass);

    std::array<uint8_t, SHA256::HashBytes> encrypted;

    auto cstr = encrypted_hex_str.c_str();
    for (uint i = 0; i < encrypted.size(); ++i) {
        sscanf(cstr, "%2hhx", &encrypted[i]);
        cstr += 2;
    }

    std::cout << "encrypted uints: ";
    for (auto e : encrypted) {
        std::cout << int(e) << " ";
    }
    std::cout << "\n";
    auto hash = base64_encode(encrypted.data(), encrypted.size());
    std::cout << "sha-256 encrypted b64 encoded: " << hash << '\n';

    EXPECT_STREQ(hash.c_str(), "tDdmKQpMiVDFA1YdblkHSFzL4Z9UIQ9FSouf3TybOu0=");
}

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

TEST(UiUtils, build_hash_rate_string)
{
    auto update_rate = std::chrono::seconds(1);
    std::string_view hash_rate_str;
    uint64_t hash_rate;

    constexpr std::array<std::string_view, 4> unit_strings = {
        " Hash/Sec", " KHash/Sec", " MHash/Sec", " GHash/Sec"};

    uint64_t rate;

    rate                               = 10;
    std::tie(hash_rate, hash_rate_str) = UiUtils::build_hash_rate_string(rate, 0, update_rate);
    EXPECT_EQ(hash_rate, 10);
    EXPECT_STREQ(hash_rate_str.data(), unit_strings[0].data());

    rate                               = 99;
    std::tie(hash_rate, hash_rate_str) = UiUtils::build_hash_rate_string(rate, 0, update_rate);
    EXPECT_EQ(hash_rate, 99);
    EXPECT_STREQ(hash_rate_str.data(), unit_strings[0].data());

    rate                               = 768;
    std::tie(hash_rate, hash_rate_str) = UiUtils::build_hash_rate_string(rate, 0, update_rate);
    EXPECT_EQ(hash_rate, 768);
    EXPECT_STREQ(hash_rate_str.data(), unit_strings[0].data());

    rate                               = 5432;
    std::tie(hash_rate, hash_rate_str) = UiUtils::build_hash_rate_string(rate, 0, update_rate);
    EXPECT_EQ(hash_rate, 5);
    EXPECT_STREQ(hash_rate_str.data(), unit_strings[1].data());

    rate                               = 92111;
    std::tie(hash_rate, hash_rate_str) = UiUtils::build_hash_rate_string(rate, 0, update_rate);
    EXPECT_EQ(hash_rate, 92);
    EXPECT_STREQ(hash_rate_str.data(), unit_strings[1].data());

    rate                               = 7864320;
    std::tie(hash_rate, hash_rate_str) = UiUtils::build_hash_rate_string(rate, 0, update_rate);
    EXPECT_EQ(hash_rate, 7);
    EXPECT_STREQ(hash_rate_str.data(), unit_strings[2].data());

    rate                               = 88887771;
    std::tie(hash_rate, hash_rate_str) = UiUtils::build_hash_rate_string(rate, 0, update_rate);
    EXPECT_EQ(hash_rate, 88);
    EXPECT_STREQ(hash_rate_str.data(), unit_strings[2].data());

    rate                               = 4294967294;
    std::tie(hash_rate, hash_rate_str) = UiUtils::build_hash_rate_string(rate, 0, update_rate);
    EXPECT_EQ(hash_rate, 4);
    EXPECT_STREQ(hash_rate_str.data(), unit_strings[3].data());

    // Integer overflow scenario
    rate = UINT64_MAX;
    std::tie(hash_rate, hash_rate_str) =
        UiUtils::build_hash_rate_string(rate + 5000, rate, update_rate);
    EXPECT_EQ(hash_rate, 5);
    EXPECT_STREQ(hash_rate_str.data(), unit_strings[1].data());
}
