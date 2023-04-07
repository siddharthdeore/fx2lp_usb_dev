#ifndef QCPSUBWINDOW_H
#define QCPSUBWINDOW_H

#pragma once

#include "qcustomplot.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <fftw3.h>
#define N_FFT 64
class QCPWidget : public QWidget
{
    Q_OBJECT
protected:
    std::size_t x_range = 1;
    std::size_t interval = 100;
    QTimer *timer;
    bool isAutoscale = false;
    bool isRolling = false;
    bool isFFT = false;

    fftw_plan p;
    fftw_complex in[N_FFT], out[N_FFT]; /* double [2] */
    QVector<double> fftVec;
    QVector<double> freqVec;
    long counter = 0;

public:
    QCustomPlot *qcp;
    QCustomPlot *fftplot;
    std::map<QString, QCPGraph *> graphs;
    QCPWidget(QWidget *parent = nullptr) : QWidget(parent)
    {
        QVBoxLayout *vbox_layout = new QVBoxLayout;
        vbox_layout->setContentsMargins(0, 0, 0, 0);
        this->setLayout(vbox_layout);
        qcp = new QCustomPlot(this);
        qcp->setAntialiasedElements(QCP::aeAll);
        qcp->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom); // | QCP::iSelectPlottables
        qcp->setOpenGl(true, 16);
        qcp->legend->setVisible(true);
        vbox_layout->addWidget(qcp);

        QWidget *config = new QWidget;
        config->setFixedHeight(40);

        QHBoxLayout *hbox_layout = new QHBoxLayout(config);
        hbox_layout->setContentsMargins(0, 4, 0, 4);

        config->setLayout(hbox_layout);
        hbox_layout->addWidget(new QLabel("Refresh Interval (msec)"));

        QSpinBox *sb_interval = new QSpinBox(config);
        sb_interval->setRange(1, 10000);
        sb_interval->setValue(100);
        hbox_layout->addWidget(sb_interval);
        hbox_layout->addWidget(new QLabel("Range (sec)"));

        QSpinBox *sb_range = new QSpinBox(config);
        sb_range->setRange(1, 100);
        hbox_layout->addWidget(sb_range);

        QCheckBox *cb_autoscale = new QCheckBox("Autoscale");
        hbox_layout->addWidget(cb_autoscale);

        QCheckBox *cb_setrolling = new QCheckBox("Rolling");
        hbox_layout->addWidget(cb_setrolling);

        QCheckBox *cb_fft = new QCheckBox("FFT");
        hbox_layout->addWidget(cb_fft);

        hbox_layout->addStretch();

        vbox_layout->addWidget(config);

        for (int i = 0; i < N_FFT / 2; i++)
        {
            freqVec.push_back(1000 * double(i) / N_FFT);
            fftVec.push_back(0);
        }
        timer = new QTimer();
        timer->start(interval);

        connect(timer, SIGNAL(timeout()), this, SLOT(update_plot()));
        connect(sb_range, SIGNAL(valueChanged(int)), this, SLOT(setRange(int)));
        connect(sb_interval, SIGNAL(valueChanged(int)), this, SLOT(setInterval(int)));
        connect(cb_autoscale, SIGNAL(toggled(bool)), this, SLOT(setAutoScale(bool)));
        connect(cb_setrolling, SIGNAL(toggled(bool)), this, SLOT(setRolling(bool)));
        connect(cb_fft, SIGNAL(toggled(bool)), this, SLOT(showFFT(bool)));
    }
    ~QCPWidget()
    {
        qcp->clearItems();
        timer->stop();
        delete timer;
    }

    void addGraph(const QString name)
    {
        qcp->addGraph();
        const std::size_t index = qcp->graphCount() - 1;
        qcp->graph(index)->setName(name);
        qcp->graph(index)->setPen(QPen(Qt::GlobalColor(std::rand() % 18)));
        graphs[name] = qcp->graph(index);
    }

    void save(const std::string &file_name)
    {
        if (qcp->graphCount() > 0)
        {
            std::cout << "\nWriting Results.." << std::endl;
            std::ofstream result_file;
            result_file.open((file_name + ".csv").c_str());
            result_file << std::fixed;
            auto data = *qcp->graph(0)->data();
            // write csv header
            result_file << "t";
            for (std::size_t c = 0; c < qcp->graphCount(); c++)
            {
                result_file << ", " << qcp->graph(c)->name().toStdString();
            }
            result_file << "\n";
            // write csv data
            for (std::size_t i = 0; i < data.size(); i++)
            {
                result_file << data.at(i)->key;
                for (std::size_t c = 0; c < qcp->graphCount(); c++)
                {
                    result_file << ", " << (qcp->graph(c)->data())->at(i)->value;
                }
                result_file << "\n";
            }
            std::cout << "Saved successfull" << std::endl;
        }
    }
public slots:
    void update_plot()
    {
        if (isAutoscale)
            qcp->yAxis->rescale();
        if (isRolling)
        {
            qcp->xAxis->rescale();
            qcp->xAxis->setRange(qcp->xAxis->range().upper, this->x_range, Qt::AlignRight);
        }
        qcp->replot();
        if (isFFT)
        {
            update_fft();
        }
    }
    void update_fft()
    {
        std::size_t count = qcp->graph(0)->dataCount();
        if (count > N_FFT)
        {
            auto ptr = qcp->graph(0)->data();
            for (int i = 0; i < N_FFT; i++)
            {
                in[i][0] = ptr->at(count - N_FFT + i)->value;
            }
            /* forward Fourier transform, save the result in 'out' */
            p = fftw_plan_dft_1d(N_FFT, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
            fftw_execute(p);

            for (int i = 0; i < N_FFT / 2; i++)
            {
                // fftVec[i] += 0.5 * (std::abs(out[i][0]) / N_FFT - fftVec[i]);
                fftVec[i] = std::abs(out[i][0]) / N_FFT;
            }
            this->fftplot->graph(0)->setData(freqVec, fftVec);
            this->fftplot->rescaleAxes();
            this->fftplot->replot();
        }
    }
    void setRange(int val)
    {
        this->x_range = val;
    }
    void setInterval(int val)
    {
        this->interval = val;
        timer->setInterval(val);
    }
    void setAutoScale(bool val)
    {
        this->isAutoscale = val;
    }
    void setRolling(bool val)
    {
        this->isRolling = val;
    }
    void showFFT(bool val)
    {
        isFFT = val;
        if (val)
        {
            this->fftplot = new QCustomPlot;
            this->fftplot->addGraph();
            this->fftplot->setAntialiasedElements(QCP::aeAll);
            this->fftplot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom); // | QCP::iSelectPlottables
            this->fftplot->setOpenGl(true, 16);
            this->layout()->addWidget(this->fftplot);
            this->fftplot->graph(0)->setData(freqVec, fftVec);
            this->fftplot->rescaleAxes();
            this->fftplot->replot();
        }
        else
        {
            this->layout()->removeWidget(this->fftplot);
            delete this->fftplot;
        }
    }
};

class QCPSubWindow : public QMdiSubWindow
{
    Q_OBJECT

public:
    QCPSubWindow(const QString &name, QWidget *parent = nullptr) : window_name(name), QMdiSubWindow(parent)
    {
        this->setWindowTitle(window_name);
        plot_widget = new QCPWidget;
        this->setWidget(plot_widget);
    }
    ~QCPSubWindow()
    {
        delete plot_widget;
    }
    std::size_t graphCount() const
    {
        return plot_widget->qcp->graphCount();
    }

    QCPGraph *graph(const std::size_t &index)
    {
        return plot_widget->qcp->graph(index);
    }
    QCPGraph *graph(const QString &name)
    {
        return plot_widget->graphs[name];
    }
    void addGraph(const QString &name)
    {
        plot_widget->addGraph(name);
    }
    void save()
    {
        plot_widget->save(window_name.toStdString());
    }
    void closeEvent(QCloseEvent *event)
    {
        QMessageBox::StandardButton resBtn = QMessageBox::question(this, "Plots",
                                                                   tr("Do you want to Save?\n"),
                                                                   QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                   QMessageBox::Yes);
        if (resBtn == QMessageBox::No)
        {
            event->accept();
        }
        else if (resBtn == QMessageBox::Yes)
        {
            save();
            event->accept();
        }
        else
        {
            event->ignore();
        }
    }

private:
    QString window_name;
    QCPWidget *plot_widget;
};

#endif