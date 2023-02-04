#include <algorithm>
#include <base64.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sha256.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

using namespace std;

#include "CharPermutationsString.h"

#include <array>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>

constexpr char chars[] = "0123456789abcdefghijklmnopqrstuvwxyz";

// TODO: Add it as test to unit test file
// {
//     auto update_rate = std::chrono::seconds(1);
//     std::string hash_rate_str;
//     uint hash_rate;

//     uint rate = 10;
//     std::tie(hash_rate, hash_rate_str) = construct_rate_opt(rate, 0, update_rate);
//     cout<< rate << ": " << hash_rate << hash_rate_str << "\n";

//     rate = 99;
//     std::tie(hash_rate, hash_rate_str) = construct_rate_opt(rate, 0, update_rate);
//     cout<< rate << ": " << hash_rate << hash_rate_str << "\n";

//     rate = 768;
//     std::tie(hash_rate, hash_rate_str) = construct_rate_opt(rate, 0, update_rate);
//     cout<< rate << ": " << hash_rate << hash_rate_str << "\n";

//     rate = 5432;
//     std::tie(hash_rate, hash_rate_str) = construct_rate_opt(rate, 0, update_rate);
//     cout<< rate << ": " << hash_rate << hash_rate_str << "\n";

//     rate = 92111;
//     std::tie(hash_rate, hash_rate_str) = construct_rate_opt(rate, 0, update_rate);
//     cout<< rate << ": " << hash_rate << hash_rate_str << "\n";

//     rate = 7864320;
//     std::tie(hash_rate, hash_rate_str) = construct_rate_opt(rate, 0, update_rate);
//     cout<< rate << ": " << hash_rate << hash_rate_str << "\n";

//     rate = 88887771;
//     std::tie(hash_rate, hash_rate_str) = construct_rate_opt(rate, 0, update_rate);
//     cout<< rate << ": " << hash_rate << hash_rate_str << "\n";

//     rate = 4294967294;
//     std::tie(hash_rate, hash_rate_str) = construct_rate_opt(rate, 0, update_rate);
//     cout<< rate << ": " << hash_rate << hash_rate_str << "\n";
// }

// This function is limmeted to rate of 4 GHash/Sec due to max value of uint64_t.
std::pair<uint, std::string_view> construct_rate(
    uint64_t hash_counter, uint64_t ref_idx, std::chrono::seconds update_rate)
{
    // Calculate the difference between hash_counter and ref_idx
    uint64_t hash_difference = hash_counter - ref_idx;

    // The constant for converting between units
    constexpr uint unit_conversion_constant = 1000;

    // The units for the hash rate, in increasing order of magnitude
    constexpr std::array<std::string_view, 4> unit_strings = {
        " Hash/Sec", " KHash/Sec", " MHash/Sec", " GHash/Sec"};

    // The index of the selected unit
    int unit_index = 0;

    // Calculate the intermediate value by dividing the hash difference by the update rate
    uint64_t intermediate_value = hash_difference / update_rate.count();

    // Determine the unit based on the intermediate value
    // The branching here could actually contribute to performance since it is expected to use
    // the same branch all the time.
    if (intermediate_value >= std::pow(unit_conversion_constant, 3)) {
        // The hash rate is in the giga hashes per second unit
        unit_index = 3;
    } else if (intermediate_value >= std::pow(unit_conversion_constant, 2)) {
        // The hash rate is in the mega hashes per second unit
        unit_index = 2;
    } else if (intermediate_value >= std::pow(unit_conversion_constant, 1)) {
        // The hash rate is in the kilo hashes per second unit
        unit_index = 1;
    }

    // Calculate the final hash rate
    uint hash_rate = intermediate_value / std::pow(unit_conversion_constant, unit_index);

    // Return the hash rate, and string representing the its unit
    return {hash_rate, unit_strings[unit_index]};
}

int main(int argc, char* argv[])
{
    // print_combs();
    CharPermutationsString::init(chars);
    auto max = std::numeric_limits<ulong>::max() - 1;
    CharPermutationsString char_premutations("0", max);
    // std::cout << "sizeof(unsigned long long int)=" << sizeof(ulong) << '\n';

    constexpr std::string_view salt   = "IEEE";
    constexpr std::string_view pepper = "Xtreme";

    auto ref_time = std::chrono::steady_clock::now();

    // Save the index to deal with counter reset when it is incremented beyond its maximum value.
    ulong hash_counter = 1, ref_idx = 1;

    while (1) {
        auto generated_phrase = char_premutations.get_next_permutation();

        std::string decrypted_pass;
        decrypted_pass.reserve(salt.size() + generated_phrase.size() + pepper.size());
        decrypted_pass.append(salt).append(generated_phrase).append(pepper);

        SHA256 sha256;
        auto encrypted_hex_str = sha256(decrypted_pass);

        std::array<uint8_t, SHA256::HashBytes> encrypted;

        auto cstr = encrypted_hex_str.c_str();
        for (int i = 0; i < encrypted.size(); ++i) {
            sscanf(cstr, "%2hhx", &encrypted[i]);
            cstr += 2;
        }

        auto hash = base64_encode(encrypted.data(), encrypted.size());

        auto now                   = std::chrono::steady_clock::now();
        constexpr auto update_rate = std::chrono::seconds(1);

        if (now - ref_time > update_rate) {
            if (hash_counter < ref_idx) {
                hash_counter += std::numeric_limits<typeof(ref_idx)>::max() - ref_idx;
                ref_idx = 0;
            }

            uint hash_rate;
            std::string hash_rate_str;
            std::tie(hash_rate, hash_rate_str) = construct_rate(hash_counter, ref_idx, update_rate);

            std::cout << std::dec << std::setprecision(2);

            // clang-format off
            std::cout << "\e[A\r\e[0K"
                      << "HashRate=" << hash_rate << hash_rate_str
                      << ", hash_counter: " << hash_counter 
                      << ", passphrase: " << generated_phrase
                      // << ", hash: " << hash 
                      << "\n"
                      ;
            // clang-format on 
            ref_time = now;
            ref_idx  = hash_counter;
        }
        ++hash_counter;
    }

    return EXIT_SUCCESS;
}
