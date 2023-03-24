
#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <thread>

class Thread {
  public:
    using LoopFunction       = std::function<void()>;
    using ThreadInitFunction = std::function<bool()>;

    /**
     * @brief Construct a new Thread object.
     *
     * @note The function arguments could be initialized in the follwing ways:
     * 1. std::bind(&SomeClass::loop, this) - Class function.
     * 2. std::bind(&loop) - Non class function.
     * 3. [&]() { [SomeClass::]loop(); }
     * 4. nullptr - if there is not intention to use that function.
     *
     * @param thread_name Thread name.
     * @param loop_func A function the thread will run in a loop.
     * @param thread_init_func An initialization function of the thread which will be run in the
     * thread context.
     */
    Thread(std::string_view thread_name, LoopFunction &&loop_func,
        ThreadInitFunction &&thread_init_func);

    ~Thread();

    /**
     * @brief Start the thread. Should be called from caller thread.
     * This function moves the Thread object to state @a eThreadState::RUNNING;
     */
    void start_thread();

    /**
     * @brief Block caller thread until thread is stopped.
     */
    void join_thread();

    /**
     * @brief Stop the thread. Could be called from the thread itself or from the caller thread.
     */
    void stop_thread();

    /**
     * @brief Returns true if the thread is in state eThreadState::RUNNING.
     *
     * @return true if the thread is in state eThreadState::RUNNING, otherwise false.
     */
    bool is_thread_running();

    /**
     * @brief Get the thread name.
     *
     * @return std::string_view thread name.
     */
    std::string_view get_thread_name() const;

    enum class eThreadState {
        NOT_STARTED,
        RUNNING,
        STOPPED,
    };

  private:
    /**
     * @brief The actual function the thread runs. In this function there is infinite callings to
     * the @a m_loop_func function (as long as the thread in in eThreadState::RUNNING state).
     */
    void _run();

    /**
     * @brief The thread name
     */
    const std::string m_thread_name;

    /**
     * @brief When the thread starts, this function will be called in a loop.
     */
    const LoopFunction m_loop_func;

    /**
     * @brief Perform initialization of the thread which will be done @b inside the thread context.
     * If this function fails, the thread object will move to state eThreadState::STOPPED.
     */
    const ThreadInitFunction m_thread_init_func;

    /**
     * @brief Internal state object.
     */
    std::thread m_thread;

    /**
     * @brief Thread state.
     */
    eThreadState m_thread_state;
};
