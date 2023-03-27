#include "HashCrackerThread.h"

/**
 * @brief HashCrackerManager is a wrapper to HashCrackerThread providing a convenient API to manage
 * it from the main thread context.
 */

class HashCrackerManager {
  public:
    /**
     * @brief Construct a new Hash Cracker Manager object.
     *
     * @param id ID of the HashCrackerThread, value of '0' shall be used only if not running the
     * The HashCrackerThread in a new thread context.
     * @param discovered_passwords_count Counter of discovered password.
     */
    HashCrackerManager(uint32_t id, uint32_t& discovered_passwords_count);

    /**
     * @brief Initialize the HashCrackerManager.
     *
     * @param other_thread_managers A vector filled with hash cracker managers.
     * @param hash_list Hash list.
     * @param main_thread_manager The main thread HashCrackerManager that runs in the main thread
     * context. [Optional]
     */
    void init(const std::vector<std::shared_ptr<HashCrackerManager>>& other_thread_managers,
        std::vector<std::string_view>& hash_list,
        HashCrackerManager* main_thread_manager = nullptr);

    /**
     * @brief Get the thread object, of the internal HashCrackerThread to allow controlling the
     * thread from outside.
     *
     * @warning Should be called only on HashCrackerManager that would run on the non main thread
     * context, i.e. ID > 0.
     *
     * @return Thread&
     */
    Thread& get_thread();

    /**
     * @brief Execute the internal HashCrackerThread loop.
     *
     * @warning Should be called only on HashCrackerManager that would run on the main thread
     * context, i.e. ID = 0.
     *
     */
    void loop();

    /**
     * @brief Handle an incoming messages.
     *
     * @warning The upper function is not thread safe and shall be called only on the object running
     * on the main thread context, i.e. ID = 0.
     */
    void handle_messages();
    void handle_messages_thread_safe();

    /**
     * @brief Sends a message.
     *
     * @warning The upper function is not thread safe and shall be called only on the object running
     * on the main thread context, i.e. ID = 0.
     */
    void send_message(std::unique_ptr<MsgBase>&& message);
    void send_message_thread_safe(std::unique_ptr<MsgBase>&& message);

    /**
     * @brief Get the latest hash rate received from the internal HashCrackerThread.
     *
     * @return uint64_t Hash rate.
     */
    inline uint64_t get_hash_rate() const { return m_hash_rate; }

  private:
    /**
     * @brief Register message handlers for the internal HashCrackerThread.
     *
     */
    void _register_message_handlers();

    /* This manager core */
    uint32_t m_id;
    HashCrackerThread m_hash_cracker;
    MsgExternalEndPoint& m_msg_endpoint;

    /* Reference to other managers */
    HashCrackerManager* m_main_thread_manager;
    std::vector<std::shared_ptr<HashCrackerManager>> m_other_thread_managers;

    /* Status Variables */
    uint32_t& m_discovered_passwords_count;
    uint64_t m_hash_rate  = -1;
    bool m_is_initialized = false;
};
