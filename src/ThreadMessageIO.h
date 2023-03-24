#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <queue>

/**
 * @brief This file provides a simple one-to-many/many-to-one message-passing mechanism between
 * threads.
 *
 * @details To use this mechanism, create an instance of the `ThreadMessageIO` class. This class
 * provides two endpoints, an internal endpoint and an external endpoint.
 * The internal endpoint is used to send and receive messages on the thread that owns the
 * ThreadMessageIO object, while the external endpoint is used to send and receive messages on any
 * other thread.
 *
 * To send a message, call the @a send_message() method of the appropriate endpoint and pass a
 * unique_ptr to the message object. To receive messages, call the @a handle_messages() method of
 * the appropriate endpoint in a loop.
 *
 * The EndPoint classes consist of two versions of each function: one that is thread-safe and blocks
 * until it acquires the lock on the input or output buffers, and releases the lock when the action
 * on the internal message buffer is complete. The other version is not thread-safe and does not
 * include an internal locking mechanism.
 * The thread-safe functions are suffixed with"_thread_safe."
 * This separation is implemented to avoid creating a general function with branching inside that
 * caters to both cases, or a function that is always thread-safe. Both of these scenarios would
 * have worse performance than having two separate functions with dedicated implementations.
 *
 * @example
 *
 * ### Message definition #########################################################################
 * enum eMessageType {
 *  MESSAGE_1,
 *  MESSAGE_2,
 *  ...
 * }
 *
 * struct sMessage1 : MsgBase {
 *  sMessage1(int arg1_, int arg2_) :
 *      MsgBase(eMessageType::MESSAGE_1), arg1(arg1_), arg2(arg2_)
 *  {
 *  }
 *  int arg1;
 *  int arg2;
 * }
 *
 * ### MessageIO Owner ############################################################################
 *
 * class Owner {
 *  // Code ...
 *  MsgExternalEndPoint &get_external_endpoint() { return m_io.get_external_endpoint(); }
 *  private:
 *  void msg_1_handler(std::unique_ptr<MsgBase> &&message) {
 *    auto msg = static_cast<sMessage1 *>(message.get());
 *    std::cout << "arg1: " << msg->arg1 << ", arg2: " << msg->arg2 << "\n";
 *  }
 *  ThreadMessageIO m_io;
 * };
 *
 * ///// Somewhere in the Owner code, do once /////
 * // Register message
 * auto& internal_msg_endpoint = m_io.get_internal_endpoint();
 * internal_msg_endpoint.register_message_handler(eMessageType::MESSAGE_1,
 *  std::bind(&Owner::msg_1_handler, this, std::placeholders::_1));
 *
 * ///// Handle messages /////
 * auto& internal_msg_endpoint = m_io.get_internal_endpoint();
 * internal_msg_endpoint.handle_messages();
 *
 * ///// Send a message /////
 * auto msg = std::make_unique<sSomeMessage>(1,2);
 * auto& internal_msg_endpoint = m_io.get_internal_endpoint();
 * internal_msg_endpoint.send_message(std::move(msg));
 *
 * ### Owner host code  ###########################################################################
 *
 * Owner owner;
 * MsgExternalEndPoint external_endpoint = owner.get_external_endpoint();
 *
 * // Register message
 * auto& external_msg_endpoint = m_io.get_external_endpoint();
 *
 * external_msg_endpoint.register_message_handler(eMessageType::MESSAGE_1,
 *  [&](std::unique_ptr<MsgBase> &&message) {
 *    auto msg = static_cast<sMessage1 *>(message.get());
 *    std::cout << "arg1: " << msg->arg1 << ", arg2: " << msg->arg2 << "\n";
 *  });
 *
 * while (true) {
 *  // Handle incoming messages
 *  external_msg_endpoint.handle_messages();
 *
 *  // Send messages
 * auto msg = std::make_unique<sSomeMessage>(1,2);
 *  external_msg_endpoint.send_message(std::move(msg));
 * }
 */

/**
 * @brief The base message class for thread messages.
 *
 * Contains a unique message type identifier to identify the message.
 */
struct MsgBase {
    MsgBase(uint16_t msg_type) : message_type(msg_type) {}
    virtual ~MsgBase() {}
    const uint16_t message_type;
};

/**
 * @brief Safe thread queues for message input and output.
 *
 * Contains a mutex for each queue to ensure thread safety if needed.
 */
struct SafeThreadQueues {
    std::queue<std::unique_ptr<MsgBase>> m_input_message_queue;
    std::mutex m_in_msg_mutex;

    std::queue<std::unique_ptr<MsgBase>> m_output_message_queue;
    std::mutex m_out_msg_mutex;
};

/**
 * @brief Base message endpoint class for thread messages.
 *
 * Provides a mechanism for registering and handling message handlers.
 * All endpoints share the same queues for message input and output.
 */
class MsgEndPoint {
  public:
    /**
     * @brief Constructor for the message endpoint.
     *
     * @param io Reference to the queues for message input and output.
     */
    MsgEndPoint(SafeThreadQueues &io);

    /**
     * @brief A type alias for the message handler function.
     */
    using HandlerFunction = std::function<void(std::unique_ptr<MsgBase> &&msg)>;

    /**
     * @brief A type alias for the message type identifier.
     */
    using MsgType = uint16_t;

    /**
     * @brief A type alias for the message handler pair.
     */
    using MsgHandler = std::pair<MsgType, HandlerFunction>;

    /**
     * @brief Register a message handler for a specific message type.
     *
     * @param msg_type The unique message type identifier.
     * @param handler_function The message handler function.
     */
    void register_message_handler(MsgType msg_type, HandlerFunction handler_function);

  protected:
    /**
     * @brief Handle a received message by calling its corresponding message handler function.
     *
     * @param msg A unique pointer to the received message.
     */
    void handle_message(std::unique_ptr<MsgBase> &&msg);

    SafeThreadQueues &m_io;

    // Vector of registered message handlers.
    std::vector<MsgHandler> m_message_handlers;
};

/**
 * @brief Internal message endpoint class for sending and receiving messages.
 *
 * Provides a mechanism for sending and receiving messages on the owner thread.
 */
class MsgInternalEndPoint : public MsgEndPoint {
  public:
    /**
     * @brief Constructor for the internal message endpoint.
     *
     * @param io Reference to the queues for message input and output.
     * required.
     */
    MsgInternalEndPoint(SafeThreadQueues &io);

    /**
     * @brief Send a message on the internal endpoint.
     *
     * @param msg A unique pointer to the message to be sent.
     */
    void send_message(std::unique_ptr<MsgBase> &&msg);
    void send_message_thread_safe(std::unique_ptr<MsgBase> &&msg);

    /**
     * @brief Processes all messages in the input message queue of the internal endpoint.
     *
     * This method should be called in a loop to continuously process all incoming messages in the
     * queue.
     */
    void handle_messages();
    void handle_messages_thread_safe();
};

/**
 * @brief External message endpoint class for sending and receiving messages.
 *
 * Provides a mechanism for sending and receiving messages on a non owner thread.
 */
class MsgExternalEndPoint : public MsgEndPoint {
  public:
    /**
     * @brief Constructor for the external message endpoint.
     *
     * @param io Reference to the queues for message input and output.
     */
    MsgExternalEndPoint(SafeThreadQueues &io);

    /**
     * @brief Send a message on the external endpoint.
     *
     * @param msg A unique pointer to the message to be sent.
     */
    void send_message(std::unique_ptr<MsgBase> &&msg);
    void send_message_thread_safe(std::unique_ptr<MsgBase> &&msg);

    /**
     * @brief Processes all messages in the input message queue of the external endpoint.
     *
     * This method should be called in a loop to continuously process all incoming messages in the
     * queue.
     */
    void handle_messages();
    void handle_messages_thread_safe();
};

/**
 * @see Explanation in the page beginning.
 */
class ThreadMessageIO {
  public:
    /**
     * @brief Default constructor for the ThreadMessageIO class.
     */
    ThreadMessageIO();

    /**
     * @brief Returns a reference to the internal endpoint of the message queue.
     * Should be used by the ThreadMessageIO object owner.
     *
     * @return MsgInternalEndPoint reference to the internal endpoint.
     */
    MsgInternalEndPoint &get_internal_endpoint();

    /**
     * @brief Returns a reference to the external endpoint of the message queue.
     * Should be used by the a non ThreadMessageIO object owner.
     *
     * @return MsgExternalEndPoint reference to the external endpoint.
     */
    MsgExternalEndPoint &get_external_endpoint();

  private:
    SafeThreadQueues m_io;
    MsgInternalEndPoint m_internal_endpoint;
    MsgExternalEndPoint m_external_endpoint;
};
