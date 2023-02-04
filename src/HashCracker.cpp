// #include "HashCracker.h"

// #include <algorithm>

// HashCracker::HashCracker() {
//     m_default_salt.copy(m_spiced_pass.data(), m_default_salt.size());
//     m_default_pepper.copy(m_spiced_pass.data() + 4, m_default_pepper.size());
//     std::fill_n(m_spiced_pass.data() + 4, 4, 'a');
//     char_max_idx    = 4;
//     char_mov_idx    = 4;
//     valid_chars_idx = 0;
// }

// void HashCracker::iterate() {
//     m_spiced_pass[m_default_salt.size() + char_mov_idx] = valid_chars.at(valid_chars_idx);
//     ++valid_chars_idx %= valid_chars.size();
//     if (valid_chars_idx == 0) {
//         }
//     //
// }
