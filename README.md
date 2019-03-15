# TSAN reports a data race due to duplicate mutex instances

This is a minimal test case for [trompeloeil](https://github.com/rollbear/trompeloeil) miscompilation under clang 7.0.1 with [libc++](https://releases.llvm.org/7.0.0/projects/libcxx/docs/UsingLibcxx.html), [ThreadSanitizer](https://clang.llvm.org/docs/ThreadSanitizer.html) and optimizations.
Here's how to run:

```sh
make CXX=clang++-7 CXXFLAGS="-std=c++17 -stdlib=libc++ -fsanitize=thread -O2"
./doit
```

No error is reported when not using `-stdlib=libc++`, or in optimization levels lower than `-O2`, or when [line **666** of `trompeloeil.hpp`](https://github.com/rollbear/trompeloeil/blob/470b827518db19c43d7487a27f6e6ee9eae7dd38/include/trompeloeil.hpp#L666) is removed.
That cannot be a coincidence.

Here's the report; notice that mutexes **M5** and **M9** are created/initialized as two separate mutexes:

```
==================
WARNING: ThreadSanitizer: data race (pid=4513)
  Write of size 8 at 0x7b0800000028 by thread T1 (mutexes: write M5):
    #0 trompeloeil::sequence_handler<1ul>::retire() <null> (doit+0x4b8e31)
    #1 trompeloeil::call_matcher<void (), std::__1::tuple<> >::run_actions(std::__1::tuple<>&, trompeloeil::call_matcher_list<void ()>&) <null> (doit+0x4b777c)
    #2 trompeloeil::return_of<void ()>::type trompeloeil::mock_func<false, void ()>(std::__1::integral_constant<bool, true>, trompeloeil::expectations<false, void ()>&, char const*, char const*) <null> (doit+0x4b3799)
    #3 void* std::__1::__thread_proxy<std::__1::tuple<std::__1::unique_ptr<std::__1::__thread_struct, std::__1::default_delete<std::__1::__thread_struct> >, main::$_0> >(void*) <null> (doit+0x4b318d)

  Previous read of size 8 at 0x7b0800000028 by main thread (mutexes: write M9):
    #0 check(trompeloeil::sequence const&) <null> (doit+0x4b93a6)
    #1 main <null> (doit+0x4b2fb3)

  Location is heap block of size 24 at 0x7b0800000020 allocated by main thread:
    #0 operator new(unsigned long) /var/tmp/portage/sys-libs/compiler-rt-sanitizers-7.0.1/work/compiler-rt-7.0.1.src/lib/tsan/rtl/tsan_new_delete.cc:65:3 (doit+0x4b1e6d)
    #1 main <null> (doit+0x4b2c1c)

  Mutex M5 (0x7b1000000018) created at:
    #0 pthread_mutex_init /var/tmp/portage/sys-libs/compiler-rt-sanitizers-7.0.1/work/compiler-rt-7.0.1.src/lib/tsan/rtl/tsan_interceptors.cc:1184:3 (doit+0x447de3)
    #1 std::__1::__libcpp_recursive_mutex_init(pthread_mutex_t*) /var/tmp/portage/sys-libs/libcxx-7.0.1/work/libcxx-7.0.1.src/include/__threading_support:221:28 (libc++.so.1+0x82e6d)
    #2 std::__1::recursive_mutex::recursive_mutex() /var/tmp/portage/sys-libs/libcxx-7.0.1/work/libcxx-7.0.1.src/src/mutex.cpp:54:43 (libc++.so.1+0x82e6d)
    #3 __libc_csu_init /var/tmp/portage/sys-libs/glibc-2.25-r11/work/glibc-2.25/csu/elf-init.c:88:8 (doit+0x4b9514)

  Mutex M9 (0x7b1000000058) created at:
    #0 pthread_mutex_init /var/tmp/portage/sys-libs/compiler-rt-sanitizers-7.0.1/work/compiler-rt-7.0.1.src/lib/tsan/rtl/tsan_interceptors.cc:1184:3 (doit+0x447de3)
    #1 std::__1::__libcpp_recursive_mutex_init(pthread_mutex_t*) /var/tmp/portage/sys-libs/libcxx-7.0.1/work/libcxx-7.0.1.src/include/__threading_support:221:28 (libc++.so.1+0x82e6d)
    #2 std::__1::recursive_mutex::recursive_mutex() /var/tmp/portage/sys-libs/libcxx-7.0.1/work/libcxx-7.0.1.src/src/mutex.cpp:54:43 (libc++.so.1+0x82e6d)
    #3 __libc_csu_init /var/tmp/portage/sys-libs/glibc-2.25-r11/work/glibc-2.25/csu/elf-init.c:88:8 (doit+0x4b9514)

  Thread T1 (tid=4516, running) created by main thread at:
    #0 pthread_create /var/tmp/portage/sys-libs/compiler-rt-sanitizers-7.0.1/work/compiler-rt-7.0.1.src/lib/tsan/rtl/tsan_interceptors.cc:965:3 (doit+0x446cd5)
    #1 main <null> (doit+0x4b2fa7)

SUMMARY: ThreadSanitizer: data race (/home/jkt/temp/clang-tsan-static-mutex/doit+0x4b8e31) in trompeloeil::sequence_handler<1ul>::retire()
==================
ThreadSanitizer: reported 1 warnings
```
