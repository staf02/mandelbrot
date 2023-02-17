#ifndef MANDELBROT_H
#define MANDELBROT_H

#endif // MANDELBROT_H
#include <QImage>
#include <concurrent_queue.h>
#include <thread>
#include <vector>

class mandelbrot {
public:
    mandelbrot() = default;

    mandelbrot(double center_x, double center_y) : center_x(center_x), center_y(center_y) {}

    double pixelColor(double x, double y, int width, int height) {
        std::atomic<double> re(0), im(0);

        for (size_t step = 0; step < max_step; step++) {
            if (re * re + im * im >= 4.0) {
                return (step % 51) / 50.0;
            }
            std::atomic<double> nre, nim;
            nre = re * re - im * im + (x - width / 2) / 300.0;
            nim = 2 * re * im + (y - height / 2) / 300.0;
            re.store(nre);
            im.store(nim);
        }
        return 0;
    }

     void queueFillImage(QImage& img) {
         std::vector<std::thread> workers;
         const int cnt = 1024;
         concurrent_queue<std::pair<int, int>> q;
         for (int i = 0; i < img.height(); i++) {
             for (int j = 0; j < img.width(); j++) {
                 q.push(std::make_pair(i, j));
             }
         }
         for (size_t N = 0; N < cnt; ++N) {
                 workers.emplace_back([this, &q, &img]() {
                     std::pair<int, int> num;
                     while (q.try_pop(num)) {
                         auto w = img.width();
                         auto h = img.height();
                         auto real_x = num.second, real_y = num.first;
                         auto pos = get_pos(real_x, real_y);
                         auto x = pos.first, y = pos.second;
                         double val = pixelColor(x, y, w, h);
                         auto start = img.bits();
                         auto p = start + real_y * img.bytesPerLine() + img.bytesPerLine() / w * real_x;
                         *p++ = 0;
                         *p++ = static_cast<uchar>(val * 0xff);
                         *p++ = static_cast<uchar>(val * 0xff);
                     }});
         }
         for (auto& W : workers)
             W.join();
     }

    void setCenter(double center_x, double center_y) {
        this->center_x = center_x;
        this->center_y = center_y;
    }

    void setScale(double scale) {
        this->scale *= scale;
    }

    void move_picture(double dx, double dy) {
        center_x -= dx;
        center_y -= dy;
    }

    std::pair<double, double> get_pos(int real_x, int real_y) {
        double x = real_x, y = real_y;
        x /= scale;
        y /= scale;
        x += center_x * (1 - 1 / scale);
        y += center_y * (1 - 1 / scale);
        return std::make_pair(x, y);
    }

    void set_max_step(size_t max_step) {
        this->max_step = max_step;
    }

private:
    double scale{1};
    double center_x, center_y;
    size_t max_step = 600;
};
