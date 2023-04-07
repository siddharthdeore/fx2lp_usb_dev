#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "qfiledialog.h"
#include <bitset>

#define FIRMWARE_FREQ_48MHz 48000000
#define FIRMWARE_FREQ_30MHz 30000000

// keep upper 8 bits
#define highByte(x) ((x) >> (8))
// keep lower 8 bits
#define lowByte(x) ((x) & (0xff))

// bEndpointAddress   0x82  EP 2 IN use : lsusb -vd 04b4:8613
#define EP2_BULK_IN 0x82

void prepare_command(const std::size_t &sampling_rate, uint8_t *command_data)
{
    uint16_t delay = 0;
    uint8_t flag = 0;
    uint8_t delay_h = 0;
    uint8_t delay_l = 0;

    if (FIRMWARE_FREQ_48MHz % sampling_rate == 0)
    {
        flag = 0x40;
        delay = int(FIRMWARE_FREQ_48MHz / sampling_rate - 1);
        if (delay > (6 * 256))
            delay = 0;
    }

    command_data[0] = flag;
    command_data[1] = highByte(delay);
    command_data[2] = lowByte(delay);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dev = std::make_unique<FX2>();
    // print available devices
    dev->print_devices();
    // todo replace timer with thread
    timer = new QTimer();
    timer->setInterval(50);

    connect(timer, SIGNAL(timeout()), this, SLOT(timer_callback()));

    windows["hello"] = new QCPSubWindow("hello");
    windows["hello"]->addGraph("D0");
    ui->mdiArea->addSubWindow(windows["hello"]);
}

MainWindow::~MainWindow()
{
    timer->stop();
    delete ui;
}
void MainWindow::update_plots()
{

}
void MainWindow::thread_function()
{
    running = true;
    auto w = windows["hello"];
    while(running)
    {
        int ret = dev->ctrl_transfer(0x40, 0xB1, 0, 0, command, sizeof(command), 0x0300);

        const std::size_t n_samples = 1024*2;
        uint8_t buf[n_samples];
        uint8_t data;
        if (ret > 0)
        {
            int actual_length = 0;
            ret = dev->bulk_transfer(EP2_BULK_IN, buf, sizeof(buf), &actual_length, 0x07ff);

            if (ret > -1)
            {
                for (int i = 0; i < actual_length; ++i)
                {
                    data = buf[i];
                    w->graph(0)->addData(key,0+((data >> 0) & 1));
                    key +=(1.0/25000.0);
                    //windows["hello"]->graph(1)->addData(key,1 + (data >> 1) & 1);
                    if(last_data != data)
                    {
                        //std::cout << actual_length << " ";
                        //std::cout << std::bitset<8>(data) << std::endl;
                        //auto now = std::chrono::high_resolution_clock::now();
                        //key = now.time_since_epoch().count()*1e-6 - wall_time;
                        last_data = data;
                    }
                }

               // ui->lcdNumber->display(data);
            }
        }
    }

}
void MainWindow::timer_callback()
{
    ui->lcdNumber->display(last_data);
}


void MainWindow::on_btn_flash_fw_clicked()
{

    auto file1Name = QFileDialog::getOpenFileName(this,
                                                  tr("Open Firmware File"),
                                                  "../firmware",
                                                  tr("Intel Hex Files (*.ihx)"));
    // ui->File1Path->setText(file1Name);
    dev->load_firmware(file1Name.toStdString());
}


void MainWindow::on_btn_start_clicked()
{
    if(ui->btn_start->text() == "Start")
    {
        prepare_command(25000,command);
        ui->btn_start->setText("Stop");
        timer->start();
        recv_thread = std::thread(&MainWindow::thread_function,this);
    }
    else
    {
        timer->stop();
        ui->btn_start->setText("Start");
        wall_time = std::chrono::high_resolution_clock::now().time_since_epoch().count()*1e-6;
        running = false;
        if(recv_thread.joinable())
            recv_thread.join();
    }
}

