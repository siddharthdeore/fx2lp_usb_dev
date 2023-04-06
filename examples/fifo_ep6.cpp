#include <iostream>
#include <FX2/FX2.h>

#include <atomic>
#include <signal.h>
#include <chrono>
#include <thread>
#include <bitset>

std::atomic<bool> runing(true);
int main(int argc, char const *argv[])
{
    signal(SIGINT, [](int s)
           { runing = false; });

    auto dev = std::make_unique<FX2>();

    // print available devices
    dev->print_devices();

    // load firmware
    dev->load_firmware("fifo_ep6.ihx");

    auto prev = std::chrono::high_resolution_clock::now();
    // auto next = std::chrono::high_resolution_clock::now();
    auto wall = std::chrono::high_resolution_clock::now();
    auto now = wall;
    // auto delta_t = std::chrono::microseconds(1000);
    const int N = 512;
    unsigned char in_buffer[N];
    std::size_t counter = 0;
    unsigned int prev_val = 0;
    while (runing)
    {
        int received_count = dev->ep6_bulkin(in_buffer, N);
        if (received_count > 0)
        {
            if (counter++ % 100== 0)
            {
                unsigned int val;
                for (size_t i = 0; i < N / 2; i += 2)
                {
                    val = (in_buffer[i] << 8 | in_buffer[i + 1]);

                    // if (val != prev_val)
                    {
                        // now = std::chrono::high_resolution_clock::now();
                        // std::cout << std::bitset<16>(val) << " " << (now - wall).count() * 1e-9 << "sec\n";
                        // prev_val = val;
                    }
                }
                now = std::chrono::high_resolution_clock::now();
                std::cout << std::bitset<16>(val) << " " << (now - wall).count() * 1e-9 << "sec\n";
                prev_val = val;
            }
        }
    }

    return 0;
}
