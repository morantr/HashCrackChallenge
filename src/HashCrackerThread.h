#pragma once

#include "HashGenerator.h"
#include "PollingScheduler.h"
#include "Statistics.h"
#include "Thread.h"
#include "ThreadMessageIO.h"

#include <optional>
#include <string>
#include <vector>

/**************************************************************************************************/
/* Messages                                                                                       */
/**************************************************************************************************/
enum eMessageType : uint16_t {
    SET_TASK,
    HASH_DISCOVERY,
    REMOVE_HASH_FROM_LIST,
    FINISHED_TASK,
    HASH_RATE_UPDATE,
};

struct sMSG_SET_TASK : MsgBase {
    sMSG_SET_TASK(std::string_view initial_permutation_, uint32_t max_permutations_) :
        MsgBase(eMessageType::SET_TASK), initial_permutation(initial_permutation_),
        max_permutations(max_permutations_)

    {
    }
    std::string initial_permutation;
    uint32_t max_permutations;
};

struct sMSG_HASH_DISCOVERY : MsgBase {
    sMSG_HASH_DISCOVERY(std::string_view hash_, std::string_view permutation_) :
        MsgBase(eMessageType::HASH_DISCOVERY), hash(hash_), permutation(permutation_)
    {
    }
    std::string hash;
    std::string permutation;
};

struct sMSG_REMOVE_HASH_FROM_LIST : MsgBase {
    sMSG_REMOVE_HASH_FROM_LIST(std::string_view hash_) :
        MsgBase(eMessageType::REMOVE_HASH_FROM_LIST), hash(hash_)

    {
    }
    std::string hash;
};

struct sMSG_FINISHED_TASK : MsgBase {
    sMSG_FINISHED_TASK(uint32_t worker_id_) :
        MsgBase(eMessageType::FINISHED_TASK), worker_id(worker_id_)

    {
    }
    uint32_t worker_id;
};

struct sMSG_HASH_RATE_UPDATE : MsgBase {
    sMSG_HASH_RATE_UPDATE(uint32_t worker_id_, uint64_t rate_) :
        MsgBase(eMessageType::HASH_RATE_UPDATE), worker_id(worker_id_), rate(rate_)

    {
    }
    uint32_t worker_id;
    uint64_t rate;
};

/**************************************************************************************************/
/* HashCrackerThread Class                                                                        */
/**************************************************************************************************/

class HashCrackerThread {
  public:
    /**
     * @brief Construct a new Hash Cracker Thread object
     *
     * @param id Thread ID.
     * @param hash_generator HashGenerator object.
     */
    HashCrackerThread(uint32_t id, HashGenerator &&hash_generator);

    /**
     * @brief Initialize the HashCrackerThread instance. Should be called only if the instance is
     * running in the same thread context as the caller.
     *
     * @return true on success, otherwise false.
     */
    bool init();

    /**
     * @brief Obtain the thread object. Needed for start/stop/join the thread from outside.
     *
     * @return Thread&
     */
    Thread &get_thread();

    /**
     * @brief The loop() function is the function that the thread object will call repeatedly in a
     * loop. If the thread context is not running, it should be run manually.
     */
    void loop();

    /**
     * @brief Obtain the external endpoint object to communicate with the HashCrackerThread from
     * outside.
     *
     * @return MsgExternalEndPoint&
     */
    MsgExternalEndPoint &get_external_endpoint();

    /**
     * @brief Set the hash list.
     *
     * @param encrypted_password_list List of encrypted passwords.
     */
    void set_hash_list(const std::vector<std::string_view> &encrypted_password_list);

  private:
    /**
     * @brief An initialization function given to and called by the Thread class in the thread
     * context. This function calls the @a init() function.
     *
     * @return true on success; otherwise, false.
     */
    bool _thread_init();

    /**
     * @brief Performs the thread work, which includes a new permutation hash calculation and
     * comparison to the list of known hashes.
     */
    void _work();

    /* Message Handlers */
    void _msg_handler_set_task(std::unique_ptr<MsgBase> &&message);
    void _msg_handler_remove_hash_from_list(std::unique_ptr<MsgBase> &&message);

    /* Messeger Senders */
    void _send_finished_task();
    void _send_hash_discovery(std::string_view hash, std::string_view permutation);
    void _send_hash_rate_update();

    /**
     * @brief A general send message function. This function pointer is initialized to the
     * appropriate send function of the internal message endpoint to send the message with, a thread
     * safe or not.
     */
    std::function<void(std::unique_ptr<MsgBase> &&msg)> _send_message;

    struct sHashDict {
        std::string encoded_hash;
        std::string decoded_hash;
    };

    /**
     * @brief Find given base64 decoded hash in the list of known hashes.
     *
     * @return std::optional, containing an iterator to the found element if found, otherwise
     * std::nullopt.
     */
    using VectorOfHashDictIterator = std::vector<sHashDict>::const_iterator;
    const std::optional<VectorOfHashDictIterator> _find_hash_encrypted_password_list(
        std::string_view b64_decoded_hash) const;

    // Object ID
    const uint32_t m_id;

    Thread m_thread;
    PollingScheduler m_scheduler;
    HashGenerator m_hash_generator;
    Statistics m_statistics;
    ThreadMessageIO m_io;

    /**
     * @brief Internal message endpoint to send and receive messages.
     */
    MsgInternalEndPoint &m_message_endpoint;

    /**
     * @brief Vector that maps base64 decode hash to the encoded.
     */
    std::vector<sHashDict> m_hash_map;

    /**
     * @brief Current task variables
     */
    uint64_t m_max_permutations    = 0;
    uint64_t m_permutation_counter = 0;
    bool m_finished_current_task   = true;
};
