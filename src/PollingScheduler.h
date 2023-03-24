#pragma once

#include <chrono>
#include <functional>
#include <string>
#include <string_view>

/**
 * @brief The PollingScheduler allows calling a function (task) periodically at certain time cycles.
 *
 * @details The PollingScheduler checks if the current time is greater than the previous cycle time.
 * If there is a timeout, then the scheduled task is called.
 *
 * To use PollingScheduler, need to schedule one task or more, using the @a schedule_task method,
 * and call @a poll() in a loop.
 * Even though the PollingScheduler is using polling to check if there is a timeout, it should be
 * faster than receiving events from from the Kernel especially in low frequency periods which will
 * use the branching mechanism of the CPU better.
 *
 * @example
 *
 * PollingScheduler ps;
 *
 * ps.schedule_task("periodic task", [&]() {  code ... }, std::chrono::seconds(1));
 *
 * while (true) {
 *  ps.poll();
 * }
 *
 */

using ScheduledTask = std::function<void()>;

class PollingScheduler {
  public:
    /**
     * @brief Poll for timeout in all of the scheduled tasks. If there is a timout that task
     * function will be called.
     *
     */
    void poll();

    /**
     * @brief Schedule a task.
     *
     * @param name Name of the task (mainly for debug purpose)
     * @param task Task function to call when there is a timeout.
     * @param time_cycle Time cycles to run the task.
     * @return uint32_t Id of the task (for future use if modifying/deleting task ability will be
     *  added).
     */
    uint32_t schedule_task(
        std::string_view name, ScheduledTask task, std::chrono::milliseconds time_cycle);

  private:
    struct sScheduledTaskParams {
        sScheduledTaskParams(
            std::string_view name, ScheduledTask task_, std::chrono::milliseconds time_cycle_) :
            task_name(name),
            task(task_), time_cycle(time_cycle_), time_reference(std::chrono::steady_clock::now())
        {
        }

        std::string task_name;
        ScheduledTask task;
        std::chrono::milliseconds time_cycle;
        std::chrono::steady_clock::time_point time_reference;
    };

    std::vector<sScheduledTaskParams> m_scheduled_tasks;
};
