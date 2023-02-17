#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <complex>
#include <thread>
#include <atomic>
#include <iostream>
#include <mutex>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Mandelbrot");
    setFixedWidth(1024);
    setFixedHeight(900);
    m.setCenter(width() / 2, height() / 2);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent* event) {
    QPainter p(this);
    QImage img(width(), height(), QImage::Format_RGB888);
    m.queueFillImage(img);
    using namespace std::chrono;
    auto start = high_resolution_clock::now();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(stop - start);
    std::cout << "Time of duration is " << duration.count() << std::endl;
    p.drawImage(0, 0, img);
}

void MainWindow::wheelEvent(QWheelEvent *event) {
    auto pos = m.get_pos(event->x(), event->y());
    m.setCenter(pos.first, pos.second);
    if (event->delta() > 0) {
        m.setScale(event->delta() / 60.0);
    }
    else {
        m.setScale(1/(-event->delta() / 60.0));
    }
    m.set_max_step(10);
    update();
    using namespace std::this_thread; // sleep_for, sleep_until
    using namespace std::chrono; // nanoseconds, system_clock, second
    sleep_for(nanoseconds(5));
    m.set_max_step(600);
    update();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if (pressed) {
        moved = true;
        m.set_max_step(10);
        update();
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    pressed = true;
    moved = false;
    last_pos = m.get_pos(event->x(), event->y());
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (moved) {
        auto pos = m.get_pos(event->x(), event->y());
        m.move_picture(pos.first - last_pos.first, pos.second - last_pos.second);
        pressed = false;
        moved = false;
        update();
        using namespace std::this_thread; // sleep_for, sleep_until
        using namespace std::chrono; // nanoseconds, system_clock, second
        sleep_for(nanoseconds(5));
        m.set_max_step(600);
        update();
    }
}
