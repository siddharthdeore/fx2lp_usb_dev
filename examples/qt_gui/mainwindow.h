#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include <FX2/FX2.h>

#include <QCPWrapper/QCPSubWindow.h>
#include <map>
#include "qtimer.h"
#include <thread>
QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btn_start_clicked();

private slots:
    void on_btn_flash_fw_clicked();
    void timer_callback();
    void update_plots();
    void thread_function();

private:
    Ui::MainWindow *ui;

    std::map<QString, QCPSubWindow *> windows;
    double wall_time = 0.0;
    double key = 0.0;
    std::unique_ptr<FX2> dev;
    uint8_t command[3];
    QTimer *timer;
    uint8_t last_data = 0;
    std::thread recv_thread;
    bool running = false;

};
#endif // MAINWINDOW_H
