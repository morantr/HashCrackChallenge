#include "Thread.h"

#include <iostream>
#include <string_view>

Thread::Thread(
    std::string_view thread_name, LoopFunction &&loop_func, ThreadInitFunction &&thread_init_func) :
    m_thread_name(thread_name),
    m_loop_func(loop_func), m_thread_init_func(thread_init_func),
    m_thread_state(eThreadState::NOT_STARTED)
{
}

Thread::~Thread() { stop_thread(); }

void Thread::start_thread()
{
    std::cout << "Thread " << m_thread_name << " started"
              << "\n";
    m_thread_state = eThreadState::RUNNING;
    m_thread       = std::thread(&Thread::_run, this);
}

void Thread::stop_thread()
{
    std::cout << "Thread " << m_thread_name << " stopped"
              << "\n";
    m_thread_state = eThreadState::STOPPED;
}

void Thread::join_thread()
{
    if (!m_thread.joinable()) {
        return;
    }
    m_thread.join();
    std::cout << m_thread_name << " joined"
              << "\n";
}

bool Thread::is_thread_running() { return m_thread_state == eThreadState::RUNNING; }

std::string_view Thread::get_thread_name() const { return m_thread_name; }

void Thread::_run()
{
    if (m_thread_init_func) {
        if (!m_thread_init_func()) {
            std::cout << "Thread init function of " << m_thread_name << " has failed"
                      << "\n";
            m_thread_state = eThreadState::STOPPED;
            return;
        }
    }

    if (!m_loop_func) {
        std::cerr << "Can't run thread loop function as it was not provided to the thread object "
                  << m_thread_name << "\n";
        m_thread_state = eThreadState::STOPPED;
        return;
    }

    while (is_thread_running()) {
        m_loop_func();
    }
}
