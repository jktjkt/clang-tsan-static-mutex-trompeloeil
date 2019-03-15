#include <thread>
#include <trompeloeil.hpp>

using namespace std::literals;

void check(const trompeloeil::sequence& seq) {
    for (int i = 0; i < 10; ++i) {
        {
            auto lock = trompeloeil::get_lock();
            if (seq.is_completed()) {
                break;
            }
        }
        std::this_thread::sleep_for(100ms);
    }
}
