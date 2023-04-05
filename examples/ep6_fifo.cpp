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
    const std::string filename("fifo_ep6_ports_bd.ihx");
    // const std::string filename("port_b_fifo_ep6.ihx");
    dev->load_firmware(filename);

    auto prev = std::chrono::high_resolution_clock::now();
    auto next = std::chrono::high_resolution_clock::now();
    auto delta_t = std::chrono::microseconds(1000);

    unsigned char in_buffer[512];
    std::size_t counter = 0;
    while (runing)
    {
        int received_count = dev->ep6_bulkin(in_buffer, 512);
        if (received_count > 0)
        {
            // std::cout << received_count << std::endl;
            auto now = std::chrono::high_resolution_clock::now();
            if (counter++ % 100 == 0)
            {
                std::cout << ((now - prev).count() * 1e-6) << std::endl;
                for (size_t i = 0; i < 512; i += 2)
                {
                    std::cout << "PB" <<std::bitset<8>(in_buffer[i]) << " PD"
                              << std::bitset<8>(in_buffer[i + 1]) << "\n";
                    //     std::cout << std::hex << (in_buffer[i]<<8 | in_buffer[i + 1]) << "\n";
                }
                std::cout << std::endl;
            }

            prev = now;
        }
        // next += delta_t;
        // std::this_thread::sleep_until(next);
    }

    return 0;
}
