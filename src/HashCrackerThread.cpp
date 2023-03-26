#include "HashCrackerThread.h"

#include "BaseOperationsUtils.h"

#include <algorithm>
#include <base64.h>
#include <iomanip>
#include <iostream>

HashCrackerThread::HashCrackerThread(uint32_t id, HashGenerator &&hash_generator) :
    m_id(id), m_thread("HashCrackerThread::" + std::to_string(m_id),
                  std::bind(&HashCrackerThread::loop, this),
                  std::bind(&HashCrackerThread::_thread_init, this)),
    m_hash_generator(std::move(hash_generator)), m_statistics(),
    m_message_endpoint(m_io.get_internal_endpoint())

{
    std::cout << m_thread.get_thread_name() << " created!\n";

    /* Define Message Handlers */
    m_message_endpoint.register_message_handler(eMessageType::SET_TASK,
        std::bind(&HashCrackerThread::_msg_handler_set_task, this, std::placeholders::_1));

    m_message_endpoint.register_message_handler(eMessageType::REMOVE_HASH_FROM_LIST,
        std::bind(
            &HashCrackerThread::_msg_handler_remove_hash_from_list, this, std::placeholders::_1));
}

bool HashCrackerThread::_thread_init() { return init(); }

bool HashCrackerThread::init()
{
    /* Schedule periodic hash rate update notifications */
    m_scheduler.schedule_task(std::string(m_thread.get_thread_name()) + " hash rate update",
        std::bind(&HashCrackerThread::_send_hash_rate_update, this), std::chrono::seconds(1));

    /* Schedule periodic message receive and handling, and set the relevant send function */

    // If running the class in thread context use the thread safe functions, otherwise use the
    // regular functions.
    if (m_thread.is_thread_running()) {
        m_scheduler.schedule_task(std::string(m_thread.get_thread_name()) + " handle messages",
            std::bind(&MsgInternalEndPoint::handle_messages_thread_safe, &m_message_endpoint),
            std::chrono::seconds(1));

        _send_message = [&](std::unique_ptr<MsgBase> &&msg) {
            m_message_endpoint.send_message_thread_safe(std::move(msg));
        };

    } else {
        m_scheduler.schedule_task(std::string(m_thread.get_thread_name()) + " handle messages",
            std::bind(&MsgInternalEndPoint::handle_messages, &m_message_endpoint),
            std::chrono::seconds(1));

        _send_message = [&](std::unique_ptr<MsgBase> &&msg) {
            m_message_endpoint.send_message(std::move(msg));
        };
    }

    return true;
}

MsgExternalEndPoint &HashCrackerThread::get_external_endpoint()
{
    return m_io.get_external_endpoint();
}

void HashCrackerThread::set_hash_list(const std::vector<std::string_view> &encrypted_password_list)
{
    /* Base64 decode the given hash list */
    m_hash_map.reserve(encrypted_password_list.size());
    for (auto hash_sv : encrypted_password_list) {
        std::string hash_str(hash_sv);

        // I'm not sure why, but the creator of the base64 library designed the decode function to
        // return an array of uint8_t values encapsulated in a std::string instead of an array or a
        // vector. The result is obviously non-printable since it contains many non-printable
        // characters.
        // Since the SHA256 library returns a printable string of uint8_t values in hexadecimal
        // format, we need to transform the output of the base64_decode() function to a string of
        // hexadecimal values.

        auto b64_decoded_uint8s = base64_decode(hash_str);
        std::string b64_decoded_hash;

        // 64 is the number bytes in SHA-256 multiplied by 2, since each byte represented by two
        // characters.
        b64_decoded_hash.reserve(64);

        // Transform each uint8_t in the b64_decoded_uint8s string into two hexadecimal printable
        // characters.
        for (auto c : b64_decoded_uint8s) {
            constexpr std::string_view base_characters_16 = "0123456789abcdef";
            auto hex =
                BaseOperationsUtils::decimal_to_base_x(static_cast<uint8_t>(c), base_characters_16);

            // Pad with single '0' if there is a single character.
            hex.insert(0, hex.size() & 1, '0');
            b64_decoded_hash.append(hex);
        }

        // Push the base64 decoded hash to vector that maps the decoded hash to the non decoded hash
        m_hash_map.emplace_back(std::move(sHashDict {hash_str, b64_decoded_hash}));
    }

    // Sort the hash map to allow binary search on it.
    std::sort(m_hash_map.begin(), m_hash_map.end(),
        [](const sHashDict &a, const sHashDict &b) { return a.decoded_hash < b.decoded_hash; });
}

void HashCrackerThread::loop()
{
    // Do scheduled tasks
    m_scheduler.poll();

    // Do Computing
    _work();
}

void HashCrackerThread::_work()
{
    if (m_finished_current_task) {
        std::cout << m_thread.get_thread_name() << " work\n";
        sleep(1);
        return;
    }

    auto b64_decoded_hash = m_hash_generator.get_next_permutation_hash();

    auto iter_opt = _find_hash_encrypted_password_list(b64_decoded_hash);

    // Continue if the hash if not in the list.
    if (iter_opt) {
        // Notify to others about the discovered hash, so they will remove it also from the
        // list.
        _send_hash_discovery((*iter_opt)->encoded_hash, m_hash_generator.get_current_permutation());

        // Remove the discovered hash from the list.
        m_hash_map.erase(*iter_opt);
    }
    ++m_permutation_counter;

    if (m_permutation_counter == m_max_permutations) {
        m_finished_current_task = true;
        _send_finished_task();
    }
}

Thread &HashCrackerThread::get_thread() { return m_thread; }

const std::optional<HashCrackerThread::VectorOfHashDictIterator>
HashCrackerThread::_find_hash_encrypted_password_list(std::string_view b64_decoded_hash) const
{
    /**
     * @details There were several implementation options.
     * 1. Using Vector or Set/Unordered Set
     * For the small amount of data in the container this project will use, vector is the fastest.
     * 2. For lookup in the Vector, we can use std::find(), or binary seach using std::lower_bound.
     * Both options gave equivalent performance. Used the binary search eventually though it is
     * requireing a sorted container since it iterate on less elements in total.
     */

    auto found_iter = std::lower_bound(m_hash_map.begin(), m_hash_map.end(), b64_decoded_hash,
        [](const sHashDict &hash_dict, const std::string_view decoded_hash) {
            return hash_dict.decoded_hash < decoded_hash;
        });

    if (found_iter != m_hash_map.end() && b64_decoded_hash == found_iter->decoded_hash) {
        return found_iter;
    }

    return std::nullopt;
}

/**************************************************************************************************/
/* Message Handlers                                                                               */
/**************************************************************************************************/

void HashCrackerThread::_msg_handler_set_task(std::unique_ptr<MsgBase> &&message)
{
    std::cout << m_thread.get_thread_name() << " Received SET_TASK message\n";
    if (!m_finished_current_task) {
        std::cerr << m_thread.get_thread_name()
                  << "\n New task set before previous task finished\n";
        return;
    }

    auto msg = static_cast<sMSG_SET_TASK *>(message.get());

    m_hash_generator.set_initial_permutation(msg->initial_permutation);
    m_max_permutations      = msg->max_permutations;
    m_finished_current_task = false;
    std::cout << m_thread.get_thread_name()
              << " initial_permutation: " << std::quoted(msg->initial_permutation)
              << " max_permutations: " << msg->max_permutations << "\n";
}

void HashCrackerThread::_msg_handler_remove_hash_from_list(std::unique_ptr<MsgBase> &&message)
{
    // std::cout << m_thread.get_thread_name() << " Received REMOVE_HASH_FROM_LIST message\n";

    auto msg = static_cast<sMSG_REMOVE_HASH_FROM_LIST *>(message.get());

    auto find_iter = std::find_if(m_hash_map.begin(), m_hash_map.end(),
        [&](const sHashDict &hash_dict) { return hash_dict.encoded_hash == msg->hash; });

    if (find_iter == m_hash_map.end()) {
        std::cerr << "FATAL: Can't remove hash " << msg->hash
                  << " since it does not exist in the list\n";
    }
    m_hash_map.erase(find_iter);
}

/**************************************************************************************************/
/* Message Senders                                                                                */
/**************************************************************************************************/

void HashCrackerThread::_send_finished_task()
{
    auto msg = std::make_unique<sMSG_FINISHED_TASK>(m_id);
    _send_message(std::move(msg));
}

void HashCrackerThread::_send_hash_discovery(std::string_view hash, std::string_view permutation)
{
    auto msg = std::make_unique<sMSG_HASH_DISCOVERY>(hash, permutation);
    _send_message(std::move(msg));
}

void HashCrackerThread::_send_hash_rate_update()
{
    auto rate = m_statistics.calculate_hash_rate_per_second(m_permutation_counter);
    if (rate == static_cast<uint64_t>(-1)) {
        return;
    }
    auto msg = std::make_unique<sMSG_HASH_RATE_UPDATE>(m_id, rate);
    _send_message(std::move(msg));
}
