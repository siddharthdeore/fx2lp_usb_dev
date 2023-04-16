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

#define USB_TIMEOUT 3

#define FIRMWARE_FREQ_48MHz 48000000
#define FIRMWARE_FREQ_30MHz 30000000

/* Protocol commands */
#define CMD_GET_FW_VERSION 0xb0
#define CMD_START 0xb1
#define CMD_GET_REVID_VERSION 0xb2

#define CMD_START_FLAGS_CLK_CTL2_POS 4
#define CMD_START_FLAGS_WIDE_POS 5
#define CMD_START_FLAGS_CLK_SRC_POS 6
/* 6 delay states of up to 256 clock ticks */
#define MAX_SAMPLE_DELAY (6 * 256)
#define CMD_START_FLAGS_CLK_CTL2 (1 << CMD_START_FLAGS_CLK_CTL2_POS)
#define CMD_START_FLAGS_SAMPLE_8BIT (0 << CMD_START_FLAGS_WIDE_POS)
#define CMD_START_FLAGS_SAMPLE_16BIT (1 << CMD_START_FLAGS_WIDE_POS)

#define CMD_START_FLAGS_CLK_30MHZ (0 << CMD_START_FLAGS_CLK_SRC_POS)
#define CMD_START_FLAGS_CLK_48MHZ (1 << CMD_START_FLAGS_CLK_SRC_POS)
// keep upper 8 bits
#define highByte(x) ((x) >> (8))
// keep lower 8 bits
#define lowByte(x) ((x) & (0xff))
// convert two bytes (8 bit each) to 16 bit WORD
#define toWord(h, l) (h << 8) | (l)

// bEndpointAddress  EP 2 IN use : lsusb -vd 04b4:8613
#define EP2_BULK_IN 2 | LIBUSB_ENDPOINT_IN
#define EP2_CTRL_OUT LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT

static unsigned int to_bytes_per_ms(const uint64_t &samplerate)
{
    return samplerate / 1000;
}
static size_t get_buffer_size(const uint64_t &samplerate)
{
    size_t s;

    /*
     * The buffer should be large enough to hold 10ms of data and
     * a multiple of 512.
     */
    s = 1000 * to_bytes_per_ms(samplerate);
    return (s + 511) & ~511;
}
static unsigned int get_number_of_transfers(const uint64_t &samplerate)
{
    unsigned int n;

    /* Total buffer size should be able to hold about 500ms of data. */
    n = (1000 * to_bytes_per_ms(samplerate) / get_buffer_size(samplerate));

    // if (n > NUM_SIMUL_TRANSFERS)
    //     return NUM_SIMUL_TRANSFERS;

    return n;
}
static unsigned int get_timeout(const uint64_t &samplerate)
{
    size_t total_size;
    unsigned int timeout;
    total_size = get_buffer_size(samplerate) * get_number_of_transfers(samplerate);
    timeout = total_size / to_bytes_per_ms(samplerate);
    return timeout + timeout / 4; /* Leave a headroom of 25% percent. */
}

void prepare_command(const std::size_t &samplerate, uint8_t *command_data, const bool &sample_16 = true)
{
    uint16_t delay = 0;
    uint8_t flag = 0;
    uint8_t delay_h = 0;
    uint8_t delay_l = 0;

    if (FIRMWARE_FREQ_48MHz % samplerate == 0)
    {
        flag = CMD_START_FLAGS_CLK_48MHZ;
        delay = int(FIRMWARE_FREQ_48MHz / samplerate - 1);
        if (delay > MAX_SAMPLE_DELAY)
            delay = 0;
    }

    if (delay == 0 && (FIRMWARE_FREQ_30MHz % samplerate == 0))
    {
        flag = CMD_START_FLAGS_CLK_30MHZ;
        delay = (FIRMWARE_FREQ_30MHz / samplerate - 1);
    }

    if (delay < 0 || delay > MAX_SAMPLE_DELAY)
    {
        std::cout << "Unable to sample at " << samplerate << "Hz." << std::endl;
    }
    flag |= sample_16 ? CMD_START_FLAGS_SAMPLE_16BIT : CMD_START_FLAGS_SAMPLE_8BIT;

    command_data[0] = flag;
    command_data[1] = highByte(delay);
    command_data[2] = lowByte(delay);
}
int main(int argc, char const *argv[])
{

    signal(SIGINT, [](int s)
           { runing = false; });
    auto dev = std::make_unique<FX2>();

    // print available devices
    dev->print_devices();

    // prepare delay command
    uint64_t SAMPLING_RATE = 8e6;

    uint8_t command_data[3];
    prepare_command(SAMPLING_RATE, command_data);

    // buffer size must be multiple of 512
    const size_t n_samples = get_buffer_size(SAMPLING_RATE);
    uint8_t buf[n_samples];
    unsigned int timeout = get_timeout(SAMPLING_RATE);
    std::cout << " samples " << n_samples << " timeout" << timeout << std::endl;
    uint16_t last_data = 0;
    auto prev = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();

    int ret = 0;
    long ctr = 0;
    int actual_length = 0;
    while (runing)
    {
        ret = dev->ctrl_transfer(EP2_CTRL_OUT, CMD_START, 0, 0, command_data, sizeof(command_data), USB_TIMEOUT);

        if (ret > 0)
        {
            actual_length = 0;

            ret = dev->bulk_transfer(EP2_BULK_IN, buf, sizeof(buf), &actual_length, timeout);
            if (ret > -1)
            {
                ctr++;
                for (int i = 0; i < actual_length; i += 2)
                {
                    uint16_t data = buf[i] << 8 | buf[i + 1];
                    if (last_data != data)
                    {
                        // std::cout << actual_length << " " << ctr++<<" ";
                        // std::cout << std::bitset<8>(buf[i]) << " " << std::bitset<8>(buf[i + 1]) << "\n";
                        last_data = data;
                    }
                    // if(i%100==0)
                    //     std::cout << std::bitset<8>(buf[i]) << " " <<std::bitset<8>(buf[i+1]) << std::endl;
                }
                std::cout << "Received length " << actual_length << std::endl;
            }
            now = std::chrono::high_resolution_clock::now();
            std::cout << ctr << " ";
            // std::this_thread::sleep_for(std::chrono::milliseconds(1));
            // ret = dev->ctrl_transfer(EP2_CTRL_OUT, CMD_START, 0, 0, command_data, sizeof(command_data), USB_TIMEOUT);
            double dt = ((now - prev).count() * 1e-9) * 2;
            std::cout << "Data rate (sps): " << std::fixed << actual_length / dt << " time:" << dt<< std::endl;
            prev = now;
        }
        else
        {
            now = std::chrono::high_resolution_clock::now();
            // std::cout << ctr << " ";
            ctr = 0;
            // std::this_thread::sleep_for(std::chrono::milliseconds(1));
            // ret = dev->ctrl_transfer(EP2_CTRL_OUT, CMD_START, 0, 0, command_data, sizeof(command_data), USB_TIMEOUT);
            double dt = ((now - prev).count() * 1e-9) * 2;
            std::cout << "Data rate (sps): " << std::fixed << actual_length / dt << " time:" << dt<< std::endl;
            prev = now;
        }
    }
    std::cout << " Captured" << std::endl;
}