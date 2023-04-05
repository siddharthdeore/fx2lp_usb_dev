#include <iostream>
#include <FX2/FX2.h>

#include <string>
#include <vector>

int main(int argc, char const *argv[])
{

    std::string filepath;
    if (argc < 2)
    {
        std::cout << "please enter valid .ihx file in params\n"
                  << "eg:\tloader led.ihs" << std::endl;
        return 0;
    }
    filepath = std::string(argv[1]);

    auto dev = std::make_unique<FX2>();

    // print available devices
    dev->print_devices();

    // load firmware
    dev->load_firmware(filepath);
}