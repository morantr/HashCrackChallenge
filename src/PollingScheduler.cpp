#include "PollingScheduler.h"

uint32_t PollingScheduler::schedule_task(
    std::string_view name, ScheduledTask function, std::chrono::milliseconds time_cycle)
{
    auto id = m_scheduled_tasks.size();
    m_scheduled_tasks.emplace_back(name, function, time_cycle);
    return id;
}

void PollingScheduler::poll()
{
    std::for_each(m_scheduled_tasks.begin(), m_scheduled_tasks.end(),
        [&](sScheduledTaskParams &scheduled_params) {
            if (std::chrono::steady_clock::now() < scheduled_params.time_reference) {
                return;
            }

            // std::cout << "Run task " << scheduled_params.task_name << std::endl;
            scheduled_params.time_reference =
                std::chrono::steady_clock::now() + scheduled_params.time_cycle;
            scheduled_params.task();
        });
}
