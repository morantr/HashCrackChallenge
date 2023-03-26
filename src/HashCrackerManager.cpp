#include "HashCrackerManager.h"

#include "GlobalDefintions.h"

#include <cassert>
#include <iomanip>
#include <iostream>

/* Common error */
static constexpr std::string_view unsafe_operation_err =
    "Unsafe operation, the hash cracker runs in another thread context\n";

static constexpr std::string_view unnecessary_thread_safe_operation_warn =
    "Unnecessary thread safe operation\n";

HashCrackerManager::HashCrackerManager(uint32_t id, uint32_t& discovered_passwords_count) :
    m_id(id), m_hash_cracker(id, std::move(HashGenerator(salt, pepper, valid_chars))),
    m_msg_endpoint(m_hash_cracker.get_external_endpoint()),
    m_discovered_passwords_count(discovered_passwords_count)
{
    _register_message_handlers();
}

void HashCrackerManager::init(
    const std::vector<std::shared_ptr<HashCrackerManager>>& other_thread_managers,
    std::vector<std::string_view>& hash_list, HashCrackerManager* main_thread_manager)
{
    if (m_is_initialized) {
        std::cerr << "HashCrackerManager " << m_id << " is already initialized\n";
        assert(true);
        return;
    }

    if (main_thread_manager) {
        m_main_thread_manager = main_thread_manager;
    }

    // Insert to the list only other HashCrackerManagers that runs their thread
    std::copy_if(other_thread_managers.begin(), other_thread_managers.end(),
        std::back_inserter(m_other_thread_managers),
        [](const auto& hash_cracker) { return hash_cracker->m_id != 0; });

    // Initialize the main thread, the other thread will automatically be initialized when
    // the their thread will start.
    if (m_id == 0) {
        m_hash_cracker.init();
    }

    // Set the hash list
    m_hash_cracker.set_hash_list(hash_list);

    m_is_initialized = true;
}

Thread& HashCrackerManager::get_thread()
{
    if (m_id == 0) {
        std::cerr << "HashCrackerManager with id 0, should not try to control the thread"
                  << " since it should not run in a thread context\n ";
        assert(true);
    }
    return m_hash_cracker.get_thread();
}

void HashCrackerManager::loop()
{
    if (m_id > 0) {
        std::cerr << unsafe_operation_err;
        assert(true);
    }
    m_hash_cracker.loop();
}

void HashCrackerManager::handle_messages()
{
    if (m_id) {
        std::cerr << unsafe_operation_err;
        assert(true);
    }
    m_msg_endpoint.handle_messages();
}

void HashCrackerManager::handle_messages_thread_safe()
{
    if (m_id == 0) {
        std::cout << unnecessary_thread_safe_operation_warn;
    }
    m_msg_endpoint.handle_messages_thread_safe();
}

void HashCrackerManager::send_message(std::unique_ptr<MsgBase>&& message)
{
    if (m_id > 0) {
        std::cerr << unsafe_operation_err;
        assert(true);
    }
    m_msg_endpoint.send_message(std::move(message));
}

void HashCrackerManager::send_message_thread_safe(std::unique_ptr<MsgBase>&& message)
{
    if (m_id == 0) {
        std::cout << unnecessary_thread_safe_operation_warn;
    }
    m_msg_endpoint.send_message_thread_safe(std::move(message));
}

void HashCrackerManager::_register_message_handlers()
{
    /* Define Message Handlers */

    // HASH_DISCOVERY Handler
    m_msg_endpoint.register_message_handler(
        eMessageType::HASH_DISCOVERY, [&](std::unique_ptr<MsgBase>&& message) {
            auto msg = static_cast<sMSG_HASH_DISCOVERY*>(message.get());
            std::cout << "Cracked hash str: " << std::quoted(msg->permutation)
                      << " hash: " << std::quoted(msg->hash) << "\n\n";

            ++m_discovered_passwords_count;

            for (auto& thread_manager : m_other_thread_managers) {
                auto msg_out = std::make_unique<sMSG_REMOVE_HASH_FROM_LIST>(msg->hash);
                thread_manager->send_message_thread_safe(std::move(msg_out));
            }

            // If this is not the main thread manager, send it to it as well
            if (m_id > 0) {
                if (!m_main_thread_manager) {
                    std::cerr << "main thread manager is null\n";
                    assert(true);
                    return;
                }
                auto msg_out = std::make_unique<sMSG_REMOVE_HASH_FROM_LIST>(msg->hash);
                m_main_thread_manager->m_msg_endpoint.send_message(std::move(msg_out));
            }
        });

    // FINISHED_TASK Handler
    m_msg_endpoint.register_message_handler(
        eMessageType::FINISHED_TASK, [&](std::unique_ptr<MsgBase>&& message) {
            auto msg = static_cast<sMSG_FINISHED_TASK*>(message.get());
            std::cout << "Hash Cracker with ID " << msg->worker_id << " finished the task\n";

            // TODO: Change this behaviour in the future
            if (m_id > 0) {
                m_hash_cracker.get_thread().stop_thread();
            }

            m_finished_task = true;
        });

    // HASH_RATE_UPDATE Handler
    m_msg_endpoint.register_message_handler(
        eMessageType::HASH_RATE_UPDATE, [&](std::unique_ptr<MsgBase>&& message) {
            auto msg = static_cast<sMSG_HASH_RATE_UPDATE*>(message.get());
            // std::cout << "Hash Cracker with ID " << msg->worker_id
            //           << " updated rate: " << msg->rate << "\n";
            m_hash_rate = msg->rate;
        });
}
