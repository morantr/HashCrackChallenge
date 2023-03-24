#include "ThreadMessageIO.h"

#include <iostream>

/**************************************************************************************************/
/* MsgEndPoint                                                                                    */
/**************************************************************************************************/

MsgEndPoint::MsgEndPoint(SafeThreadQueues &io) : m_io(io) {}

void MsgEndPoint::register_message_handler(MsgType msg_type, HandlerFunction handler_function)
{
    auto found_iter = std::find_if(m_message_handlers.begin(), m_message_handlers.end(),
        [&](const MsgHandler handler) { return msg_type == handler.first; });

    if (found_iter != m_message_handlers.end()) {
        std::cerr << "Handler for message type " << msg_type << " is already exist, ignore\n";
        return;
    }

    m_message_handlers.emplace_back(MsgHandler {msg_type, handler_function});
}

void MsgEndPoint::handle_message(std::unique_ptr<MsgBase> &&msg)
{
    auto found_iter = std::find_if(m_message_handlers.begin(), m_message_handlers.end(),
        [&](const MsgHandler handler) { return msg->message_type == handler.first; });

    if (found_iter == m_message_handlers.end()) {
        std::cerr << "Handler for message type " << msg->message_type << " not found\n";
        return;
    }

    auto &handler_function = found_iter->second;
    handler_function(std::move(msg));
}

/**************************************************************************************************/
/* MsgInternalEndPoint                                                                            */
/**************************************************************************************************/

MsgInternalEndPoint::MsgInternalEndPoint(SafeThreadQueues &io) : MsgEndPoint(io) {}

void MsgInternalEndPoint::send_message(std::unique_ptr<MsgBase> &&msg)
{
    m_io.m_output_message_queue.emplace(std::move(msg));
}

void MsgInternalEndPoint::send_message_thread_safe(std::unique_ptr<MsgBase> &&msg)
{
    m_io.m_out_msg_mutex.lock();
    m_io.m_output_message_queue.emplace(std::move(msg));
    m_io.m_out_msg_mutex.unlock();
}

void MsgInternalEndPoint::handle_messages()
{
    while (true) {
        if (m_io.m_input_message_queue.empty()) {
            return;
        }
        auto msg = std::move(m_io.m_input_message_queue.front());
        m_io.m_input_message_queue.pop();
        handle_message(std::move(msg));
        continue;
    }
}

void MsgInternalEndPoint::handle_messages_thread_safe()
{
    while (true) {
        m_io.m_in_msg_mutex.lock();
        if (m_io.m_input_message_queue.empty()) {
            m_io.m_in_msg_mutex.unlock();
            return;
        }
        auto msg = std::move(m_io.m_input_message_queue.front());
        m_io.m_input_message_queue.pop();
        m_io.m_in_msg_mutex.unlock();
        handle_message(std::move(msg));
    }
}

/**************************************************************************************************/
/* MsgExternalEndPoint                                                                            */
/**************************************************************************************************/

MsgExternalEndPoint::MsgExternalEndPoint(SafeThreadQueues &io) : MsgEndPoint(io) {}

void MsgExternalEndPoint::send_message(std::unique_ptr<MsgBase> &&msg)
{
    m_io.m_input_message_queue.emplace(std::move(msg));
}

void MsgExternalEndPoint::send_message_thread_safe(std::unique_ptr<MsgBase> &&msg)
{
    m_io.m_in_msg_mutex.lock();
    m_io.m_input_message_queue.emplace(std::move(msg));
    m_io.m_in_msg_mutex.unlock();
}

void MsgExternalEndPoint::handle_messages()
{
    while (true) {
        if (m_io.m_output_message_queue.empty()) {
            return;
        }
        auto msg = std::move(m_io.m_output_message_queue.front());
        m_io.m_output_message_queue.pop();
        handle_message(std::move(msg));
        continue;
    }
}

void MsgExternalEndPoint::handle_messages_thread_safe()
{
    while (true) {
        m_io.m_out_msg_mutex.lock();
        if (m_io.m_output_message_queue.empty()) {
            m_io.m_out_msg_mutex.unlock();
            return;
        }
        auto msg = std::move(m_io.m_output_message_queue.front());
        m_io.m_output_message_queue.pop();
        m_io.m_out_msg_mutex.unlock();
        handle_message(std::move(msg));
    }
}

/**************************************************************************************************/
/* ThreadMessageIO                                                                                */
/**************************************************************************************************/

ThreadMessageIO::ThreadMessageIO() : m_internal_endpoint(m_io), m_external_endpoint(m_io) {}

MsgInternalEndPoint &ThreadMessageIO::get_internal_endpoint() { return m_internal_endpoint; }
MsgExternalEndPoint &ThreadMessageIO::get_external_endpoint() { return m_external_endpoint; }
