#include <iostream>
#include <FX2/FX2.h>

#include <string>
#include <vector>
#include <stdint.h>

#include <atomic>
#include <signal.h>
#include <chrono>
#include <thread>
#include <bitset>

std::atomic<bool> runing(true);

#define FIRMWARE_FREQ_48MHz 48000000
#define FIRMWARE_FREQ_30MHz 30000000

// keep upper 8 bits
#define highByte(x) ((x) >> (8))
// keep lower 8 bits
#define lowByte(x) ((x) & (0xff))

// bEndpointAddress   0x82  EP 2 IN use : lsusb -vd 04b4:8613
#define EP2_BULK_IN 0x82

int main(int argc, char const *argv[])
{

    signal(SIGINT, [](int s)
           { runing = false; });
    auto dev = std::make_unique<FX2>();

    // print available devices
    dev->print_devices();

    // prepare delay command
    std::size_t SAMPLING_RATE = 250000;
    uint16_t delay = 0;
    uint8_t flags = 0;
    uint8_t delay_h = 0;
    uint8_t delay_l = 0;

    if (FIRMWARE_FREQ_48MHz % SAMPLING_RATE == 0)
    {
        flags = 0x40;
        delay = int(FIRMWARE_FREQ_48MHz / SAMPLING_RATE - 1);
        if (delay > (6 * 256))
            delay = 0;
    }

    if ((delay == 0) && ((FIRMWARE_FREQ_30MHz % SAMPLING_RATE) == 0))
    {
        flags = 0x00;
        delay = int(FIRMWARE_FREQ_30MHz / SAMPLING_RATE - 1);
    }
    delay_h = highByte(delay);
    delay_l = lowByte(delay);

    uint8_t command_data[] = {flags, delay_h, delay_l};

    const size_t n_samples = 1024;
    uint8_t buf[n_samples];
    uint8_t last_data = 0;

    while (runing)
    {
        int ret = dev->ctrl_transfer(0x40, 0xB1, 0, 0, command_data, sizeof(command_data), 0x0300);
        if (ret > 0)
        {
            int actual_length = 0;
            ret = dev->bulk_transfer(EP2_BULK_IN, buf, sizeof(buf), &actual_length, 0x0300);
            if (ret > -1)
            {
                for (int i = 0; i < actual_length; ++i)
                {
                    if (last_data != buf[i])
                    {
                        std::cout << actual_length << " ";
                        std::cout << std::bitset<8>(buf[i]) << std::endl;
                        last_data = buf[i];
                    }
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::cout << " Captured" << std::endl;
}