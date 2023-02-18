#include "CharPermutationsString.h"
#include "UiUtils.h"

#include <algorithm>
#include <array>
#include <base64.h>
#include <chrono>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sha256.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

constexpr char chars[] = "0123456789abcdefghijklmnopqrstuvwxyz";

void full_flow_demo()
{
    CharPermutationsString::init(chars);
    auto max = std::numeric_limits<ulong>::max() - 1;
    CharPermutationsString char_premutations("0", max);

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
        for (uint i = 0; i < encrypted.size(); ++i) {
            // Using sscanf since it is the fastest way found to convert hex string to uint.
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
            std::tie(hash_rate, hash_rate_str) =
                UiUtils::build_hash_rate_string(hash_counter, ref_idx, update_rate);

            std::cout << std::dec << std::setprecision(2);

            // clang-format off
            std::cout << "\33[A\r\33[0K" // Remove the previous print to prevent screen flooding
                      << "HashRate=" << hash_rate << hash_rate_str
                      << ", hash_counter: " << hash_counter 
                      << ", passphrase: " << generated_phrase
                      << "\n"
                      ;
            // clang-format on 
            ref_time = now;
            ref_idx  = hash_counter;
        }
        ++hash_counter;
    }
}

int main(int argc, char* argv[])
{
    try
    {
        full_flow_demo();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    

    return EXIT_SUCCESS;
} 
