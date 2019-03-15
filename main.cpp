#include <thread>
#include "mock.h"

void check(const trompeloeil::sequence& seq);

using namespace std::literals;

int main(int argc, char *argv[])
{
    Mock m;
    trompeloeil::sequence seq;

    REQUIRE_CALL(m, bar()).IN_SEQUENCE(seq).TIMES(1);

    std::thread t([&m] {
        std::this_thread::sleep_for(200ms);
        m.bar();
    });

    check(seq);

    t.join();

    return 0;
}
