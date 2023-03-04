
#pragma once

#include <array>
#include <string>
#include <string_view>
#include <vector>

class HashGenerator {
  public:
    HashGenerator(
        std::string_view salt, std::string_view pepper, std::string_view valid_characters);

    HashGenerator(HashGenerator &&hash_generator);

    /**
     * @brief Overrides the current permutation @a m_current_permutation with a new one -
     * @a initial_permutation.
     *
     * @param initial_permutation
     */
    void set_initial_permutation(std::string_view initial_permutation);

    /**
     * @brief Increment the permutation to the next one, and construct a hash from that.
     *
     * @return std::string_view A Hash of the next permutation.
     */
    std::string_view get_next_permutation_hash();

    /**
     * @brief Get the current permutation string.
     *
     * @return std::string_view The current permutation.
     */
    std::string_view get_current_permutation() { return m_current_permutation; }

  private:
    static constexpr size_t sha256_vector_size = 32;

    /**
     * @brief Construct a spiced permutation by adding @a m_salt as prefix, and @a m_pepper
     * as suffix to @a m_current_permutation which holds the latest password permutation.
     *
     * @return std::string of spiced permutation.
     */
    std::string _get_spiced_permutation();

    /**
     * @brief Encrypt given password @a decrypted_password with SHA-256.
     *
     * @param decrypted_pass Password to encrypt.
     * @return std::array<uint8_t, sha256_vector_size> containing the encrypted password.
     */
    std::array<uint8_t, sha256_vector_size> _encrypt_password(std::string_view decrypted_password);

    std::string m_current_permutation;
    const std::string m_salt;
    const std::string m_pepper;
    const std::string m_valid_characters;
};
