#include "HashGenerator.h"

#include "BaseOperationsUtils.h"

#include <base64.h>
#include <sha256.h>

HashGenerator::HashGenerator(
    std::string_view salt, std::string_view pepper, std::string_view valid_characters) :
    m_salt(salt),
    m_pepper(pepper), m_valid_characters(valid_characters)
{
}

HashGenerator::HashGenerator(HashGenerator &&hash_generator) :
    m_current_permutation(std::move(hash_generator.m_current_permutation)),
    m_salt(std::move(hash_generator.m_salt)), m_pepper(std::move(hash_generator.m_pepper)),
    m_valid_characters(std::move(hash_generator.m_valid_characters))
{
}

void HashGenerator::set_initial_permutation(std::string_view initial_permutation)
{
    m_current_permutation.assign(initial_permutation);
}

std::string HashGenerator::get_next_permutation_hash()
{
    // Fill m_current_permutation with next password permutation
    BaseOperationsUtils::increment_base_x_integer(m_current_permutation, m_valid_characters);

    // Adds "salt" and "pepper" to the the password
    auto decrypted_password = _get_spiced_permutation();

    // Encrypt the password with SHA-256
    return _encrypt_password(decrypted_password);
}

std::string HashGenerator::_get_spiced_permutation()
{
    std::string decrypted_pass;
    decrypted_pass.reserve(m_salt.size() + m_current_permutation.size() + m_pepper.size());
    decrypted_pass.append(m_salt).append(m_current_permutation).append(m_pepper);
    return decrypted_pass;
}

std::string HashGenerator::_encrypt_password(std::string_view decrypted_password)
{
    SHA256 sha256;
    return sha256(decrypted_password.data(), decrypted_password.size());
}
