#include "BaseOperationsUtils.h"
#include "GlobalDefintions.h"
#include "HashCrackerManager.h"
#include "UiUtils.h"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>

bool single_thread = false;

std::vector<std::string_view> hash_list = {
    "/PtjJboZGlsmTovvyOhBOoTVnQKUP/gJXxjLAW9Lppw=", "05HwH93tksb69U1ifesCQuYFP+gKPVH2L6W8JeBdXy0=",
    "0BkyqI3NHyjh0m20wNt6txW08dglSMP4/qzUEezq4Aw=", "1mT5cdKRz4BbfMdc8LAdnxfjsGO4lV0k0/V1IHtidmY=",
    "28AdfW0JHmCP4TbieGON8dafRaFpUgpzuX2bHZN6WsM=", "3hoie8omUyvM/9Qfx9dKfoptlwemYe2os8aohTGzoyw=",
    "3uDaglIdYUn11AadEhELBjE15A0L6hAaWnZmjCGtt+M=", "4D4NstIYSjVN826Q+SXUDDmXglJplpYWiJYf9rt7H8U=",
    "7FCsEXCDTAxyLh3EPnNx7YrJ44SzehQYv3GmPSA6pWk=", "81X3NN5JgTuGgqq3ErF0eL/l/wZFYkCwur6fZ06L2Us=",
    "8FzGA/nS7XizLrAVOr/FoeKSq4gaoQRq+kpBKNXHIzc=", "8OtpJ+E1XHv2RDsKIEwJc9KUFwPRzaqeHJ735Er6AVE=",
    "8cdgZu9dBOrcTBMqElM+y9Vh5FTBRQ7n9EGa/4qVHUk=", "8esDbw8ZVmUuUMEy2Scf5qGiZYiykevrvKpq2bVYHj4=",
    "9DS4orbhPFbjJcosEqQg+eg0Si5qSOnftfHiqK8sYug=", "9XDFIu4RPH0EL5XR+5VYILJ3UFAyltpfjONJKp/vcLk=",
    "B2E/K/DywbLEKutOKpS8HxHFrZwucwac4KjzYgsXg3g=", "BJQeqlV+4ejv0je5GpekzGdHHWHL5nnrbtD/170LZCA=",
    "BjQiH/A2FUNHlUwhBi8NWmj3HmhmAh6Ag0kRyVSaVo8=", "BwbAsOqPsxteVCpAwIjrhYogsUS1bF/bLns2QdcLYUI=",
    "CYDZabjeyTAwcEDEcvrX83514UmpjzvQUQ68DIZ/PXg=", "CornANxoZ5FJnlhwHmK42CDXf3h6jFr3g73YIRuoymQ=",
    "DDa2TJX20pPsNftfyJ3s6LBwSMSR3EADZGDxW2wThbs=", "FFXy3vru2D8rTWZRlh9lSMvtEusfWgO17OmJCTQTECs=",
    "FGkqFC/jLDqDZ10fql1nGw7AQNWioOrZ3ydEaJyXBwo=", "Fz+Y0H/R2rMZlc1C88Yx0A0xluYnVTinlw5qaSx8vWQ=",
    "GcJMWMDF6+f+onf6oi1FbpnN7dVrFEZnlXtHqmaygs4=", "GsXTQM0w+Clb1c9B7n28mADU2quLeI1n91KTyBboeHI=",
    "HLnuqQmCYetzrau3frCDEpZ52QCIby108gugsmwAwQ0=", "HWv9gx+GL/6g+0b0eOc+1Z/8BHse91/5T/DdiDwEknU=",
    "IDB/pOthrWobzapJ/N8HsraNhwfbExAa2uusdiKHFFI=", "IXYqlHbVeONERbxFe8SaEPEEKex45EihiC/l8CR52kk=",
    "Idvs4Al9YZsqPG8xkSxVqb6MOVhbw5k+qtF8UZKYVE8=", "IxQxcFXR51q8h8FLblPhYfUR30lIAt6hX8TjZWVa/GE=",
    "JCmqBN0MsW13tEmsQPYWg9Fj25MUrqFYvSK2arxTt0A=", "JOXxLH/i8i+fxDIWP2cts5Si/5En1A8M3s/vy6Aadic=",
    "KudA8vCEQdGaaCSxotpAcluXnVPS3MAZPkwI/lVupak=", "LGZEfbUr/tMREpJtsao/uuewcJXApmgfHDbh1zzfdhU=",
    "LZIzmWEqXDPJsnKttFGRaG/jUhHrbTEKt1XCO6XbdME=", "Lq0kV5M0HDSgB4m5KZbbn6BYRNlkKfaaAr3/11ueopY=",
    "Lqxt1UjT1ecV6ucgYn+yrGSUTxPWkZgdDtbygGwC/BA=", "MHQTB1MSsvhBxMpdRUiaM9Uj1QxU7zYq3FqDlW2HT2s=",
    "MKewBZryb2l36Y0tDyx+WuVeXUGfiSzcJplm9y1w9m0=", "N5aunKGHeN2WETLXLzfhfCxAfkwtGU/imziiTF3t7oY=",
    "NmrOUzHxKSfNT8UJ1YXbRL8I+HCAb+glJ0bBXcHfagE=", "NnSS+AuW1Z6zytSfqaiIVp4xxHHe70Av+IdhDlkoItQ=",
    "O9L1ZWYwuzgaImTjOwuogXfpC+C44zzcDhpt08LjR5c=", "R/ye+L9W+l6hyZ/v1POsWYboEGemIisARL8ohUvfBLI=",
    "R1v9fEb9VrZuU5xiYTKTqhHF03VtXg7+KtfFHPkQuCQ=", "RVfhsLovxa+/6tWgeSBASIIkzXkVtDPT4yYvjboHhIk=",
    "Rz38Ng2qI3mPkaRB6uDoCYmmfzbVTCzpt2sG1o+TZqo=", "S98FBzlv2vMVP/q+23m1wrHMJIrcy1rhoQGy338c4Bs=",
    "SzraQWWasG5ZO1tJq16DqU/7M/o/WRiAWRl1aFFvwr8=", "TInfNYwXvofBA+9QIe3+XEfDpO5ER+R+Jn3BOshhZWU=",
    "VDkcRd54BhYlK5Wg2PPDa/jzGrSkMepGIv6Tw3I2ksc=", "VjFTqTEY27V8lK2yCvhLhYm2Brh9bN1vWckVaevsiiY=",
    "Wau4ReopjFKk8SYYNq5lIBL+Rgg8aBR6h9UgTIv2u7I=", "XtEWsXf16y6Bc7vQxDy7hwRdBVWo3dV9C6CDVSf4PLs=",
    "Y5b6UztMueFYIFMl4a61jlD/ZhFG74/rVn8XaqqU+8c=", "YollqBlewcxE/kF6PKvv0r1CLZkKx82657bB0eQbiK0=",
    "YzSqlQTtq5j+Kd+hW1ISgBW0mn61vsQsxNeipq0sYCo=", "aJIH+q0YjYZCpierKtbue5JDtZSF8tKxVKuHYUPQ65k=",
    "bi6rh2HgTbJxR2GOTNWZLlxiiWZVnObptGj0KqOCSYo=", "cX7VyMvSYhuRvEfAUb3uNh8kmjpNFg0tatUPN562iOg=",
    "eSCTiCrzHPbngPu3F4ivPkLUv7MqLUlmWAhA4UO975Y=", "eUqkcVCbgIx1bGhhmnN5MxJFJhVruINmG65TjT/EQ1k=",
    "gE7PseB3mspPtYG3JROzT9FeqTfPFYQvBF2SJD9V19Y=", "gMi4hC4o7Fmv6yIrU48BVy8I1khXwkaD36G7bWiZHeo=",
    "h84yifAWGLj9sakEqxZ3QEjkXL42AoScP3L5Tdevm98=", "hEGKCiTZSA5x560hodRoIBBTE8pv4sP1VXG4D0fXWcI=",
    "ix+0IJIpLpHeSHEII+Q/IVY+FlRXn3xMA0ey6UITi34=", "j2GTUqtqZpotY8wF16zkvnbdCLTnX3oOZ30SjQUnIUk=",
    "jtUg00EsmzzFkk8JgKg3OpkmPRpN9xbwsdNXQSPczwo=", "k1J9Dv3EI442CO6A2FGN1H8JgFO2kjNBvkjDR0WIvkA=",
    "kuRpkIh9kaNz6XvG8U6GO/IARH/SqhRnTiJbZHXC0yQ=", "lFgqRrqTz1WXmO22u+Is1ZmWWUtuYrTJigsSB7I9NHI=",
    "lUfxHX9xH2aOHheMMqQF+f5BNh97avew2uOwEN3B7HE=", "m0IeuugWDOl9cFUFRYJhouCBT39T0dpp1xBOKPqHP94=",
    "mgA5kgALstQpGUBp4vZ8oiz0P4jjAGl0wjgls6kQyMA=", "nMAwaDYvEIAwoqtqWMpBAWdhuRgRq/fmwWbRM7cOMIU=",
    "opBtoC66YDRbLNqZAAu2FIeKfF0HMOGHCOCPYeNHdx4=", "ot/igM+me4e3UTT731qcBkSAcToyADMCddr7i7LCWGo=",
    "qrkd/8imuRtiDb9N1w2hQRxJAkdx3Wqh1HVXPS7dym8=", "r6BN0tdyAYZD0Nmc7bfV0WRcFBb1A2WIPPKHVRG59k8=",
    "rdALvOYVhA3hnUTBlaQXigWBSgMYzGTreSKyMoAoKfw=", "rjwtKqkPc7cfQ+zZ+E9c+fzQYhRvhVtaKEFb+srIHcQ=",
    "rwvmTDiJxIEETbsngvpxYGwZZK+FGo7527odGuQUjtQ=", "socJeO02bT2w+XZUrLoopbZvQ1lRhDfE88GVrJQ8p+g=",
    "tDdmKQpMiVDFA1YdblkHSFzL4Z9UIQ9FSouf3TybOu0=", "tojYiNtlWmq+7r1dSvxDk3W5at/NMAi1uxCHY61WAKk=",
    "tqpGCBzhR+0ONFk1sBiHPhz+kRiXmY3CGdUXqnMJwLg=", "uAZthS7b4ySZtWpM9pJ7ulYnhFdpFABpR2iPRQEmff0=",
    "uCG9dSBejCOrZWsX7+u8G340p74s8lDS/El8MIeOyMo=", "ugcIIpDID0R1uFqBAcN3PNXhwlhen77GdAccFgpbs+A=",
    "usg8BTtSfewL5M3OVg91TJCTc5vONLqgUCC/Si1Grsg=", "vs2sCU8qG0pDYQfcjlPzDzvcbJnhP1OgFRcXP4i3ffw=",
    "wEtqAs8JHjicWnXshAWF5Sg6NoswuG9qeJ7USw7YD7c=", "y+zbMpySKY+WF97KkgRQ+tBpM7iTqTj9guWmGJcqfyA=",
    "y1R6JQiUzUovgtdrvCkbeQAyMhFoupzhI5ZuQVPfCgw=", "zqKPAOt5ziHSeRxc0TgUZF3rJxzBHAKdJeccvt3F7Jg="};

void full_flow_demo()
{
    uint32_t num_thread_supported = std::thread::hardware_concurrency();

    if (single_thread) {
        num_thread_supported = 1;
    }

    std::cout << num_thread_supported << " concurrent threads are supported\n";

    uint32_t discovered_passwords_count = 0;

    /* Creation of HashCrackerManager  */

    uint32_t thread_id = 0;
    // Main thread
    HashCrackerManager main_thread_hash_cracker_manager(thread_id, discovered_passwords_count);

    // Other threads
    std::vector<std::shared_ptr<HashCrackerManager>> hash_cracker_thread_managers;

    // Create more working thread and push the into the pool
    for (thread_id = 1; thread_id < num_thread_supported; ++thread_id) {
        hash_cracker_thread_managers.emplace_back(
            std::make_unique<HashCrackerManager>(thread_id, discovered_passwords_count));
    }

    /* HashCrackerManager initialization */
    // Main thread
    main_thread_hash_cracker_manager.init(hash_cracker_thread_managers, hash_list);

    // Other threads
    for (auto& hash_cracker_manager : hash_cracker_thread_managers) {
        hash_cracker_manager->init(
            hash_cracker_thread_managers, hash_list, &main_thread_hash_cracker_manager);
    }

    /* Set HashCrackerManagers task */
    std::string initial_permutation("");
    constexpr uint32_t permutations_job = 100000000;
    const std::string permutations_job_base_str =
        BaseOperationsUtils::decimal_to_base_x(permutations_job, valid_chars);

    for (thread_id = 0; thread_id < num_thread_supported; ++thread_id) {
        auto& hash_cracker_manager =
            (thread_id == 0 ? main_thread_hash_cracker_manager
                            : *hash_cracker_thread_managers[thread_id - 1]);

        auto msg = std::make_unique<sMSG_SET_TASK>(initial_permutation, permutations_job);
        hash_cracker_manager.send_message(std::move(msg));

        initial_permutation = BaseOperationsUtils::sum_base_x_integers(
            initial_permutation, permutations_job_base_str, valid_chars);
    }

    /* Start HashCrackerManagers threads */
    for (auto& hash_cracker_manager : hash_cracker_thread_managers) {
        hash_cracker_manager->get_thread().start_thread();
    }

    constexpr auto update_rate = std::chrono::seconds(1);
    auto ref_time              = std::chrono::steady_clock::now();

    std::cout.setf(std::ios::fixed);

    while (true) {
        main_thread_hash_cracker_manager.loop();

        if (std::chrono::steady_clock::now() - ref_time < update_rate) {
            continue;
        }
        ref_time = std::chrono::steady_clock::now();

        main_thread_hash_cracker_manager.handle_messages();

        uint64_t rate = -1;
        if (main_thread_hash_cracker_manager.get_hash_rate() != static_cast<uint64_t>(-1)) {
            rate += main_thread_hash_cracker_manager.get_hash_rate();
        }

        for (auto& hash_cracker_manager : hash_cracker_thread_managers) {
            hash_cracker_manager->handle_messages_thread_safe();

            if (hash_cracker_manager->get_hash_rate() == static_cast<uint64_t>(-1)) {
                continue;
            }
            rate += hash_cracker_manager->get_hash_rate();
        }

        if (main_thread_hash_cracker_manager.is_task_finished()) {
            break;
        }

        if (rate == static_cast<uint64_t>(-1)) {
            continue;
        }

        double hash_rate;
        std::string hash_rate_str;
        std::tie(hash_rate, hash_rate_str) = UiUtils::build_hash_rate_string(rate);

        std::cout << ""
                  //<< "\033[0F" // Remove the previous print to prevent screen flooding.
                  << "HashRate=" << std::setprecision(2) << hash_rate << hash_rate_str
                  << ", total passwords discoveries: " << discovered_passwords_count << "/"
                  << hash_list.size()
                  << "                                                                          \n";
    }

    for (auto& hash_cracker_manager : hash_cracker_thread_managers) {
        hash_cracker_manager->get_thread().join_thread();
    }
}

int main(int argc, char* argv[])
{
    for (auto arg_index = 0; arg_index < argc; ++arg_index) {
        if (std::string_view(argv[arg_index]) == "-s") {
            std::cout << "single thread mode\n";
            single_thread = true;
        }
    }

    try {
        full_flow_demo();
        std::cout << "Demo finished\n";
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
