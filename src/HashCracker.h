// #include <array>
// #include <string>

// constexpr uint32_t user_passphrase_max_length = 20;
// constexpr std::string_view m_default_salt     = "IEEE";
// constexpr std::string_view m_default_pepper   = "Xtreme";
// constexpr uint32_t spiced_passphrase_max_length =
//     user_passphrase_max_length + m_default_salt.size() + m_default_pepper.size();
// constexpr std::basic_string_view<char> valid_chars = "abcdefghijklmnopqrstuvwxyz0123456789";

// class HashCracker {
//   public:
//     inline char increment_char() {}
//     void iterate() {}

//   private:
//     std::array<char, spiced_passphrase_max_length> m_spiced_pass = {};
//     uint8_t char_max_idx;
//     uint8_t char_mov_idx;
//     uint8_t valid_chars_idx;
// };
