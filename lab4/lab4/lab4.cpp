#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>

using namespace cv;
using namespace std;

void Line(cv::Mat& img, int x1, int y1, int x2, int y2, cv::Scalar color1, int thickness = 1) {

    cv::Scalar color = cv::Scalar(color1[2], color1[1], color1[0]); //  RGB

    int x = x1, y = y1;  // текущее значение точки, инициализируем координатами начальной точки
    int dx = x2 - x1, dy = y2 - y1;
    int ix, iy; // величина приращений (-1, 0, 1) по координатам определяют направление движения
    int e; // ошибка
    int i; // счетчик цикла

    // определение величины приращения по координатам, а также получение абсолютных значений dx, dy
    if (dx > 0) ix = 1;
    else if (dx < 0) { ix = -1; dx = -dx; }
    else ix = 0;

    if (dy > 0) iy = 1;
    else if (dy < 0) { iy = -1; dy = -dy; }
    else iy = 0;

    if (dx >= dy) {
        e = 2 * dy - dx; // инициализация ошибки с поправкой на половину пиксела

        if (iy >= 0) { // увеличиваем или не изменяем y
            // основной цикл
            for (i = 0; i <= dx; i++) {
                cv::Point point(x, y);
                cv::circle(img, point, thickness, color, -1); // выводим точку
                if (e >= 0) { // ошибка стала неотрицательной
                    y += iy; // изменяем y
                    e -= 2 * dx; // уменьшаем ошибку
                }
                x += ix; // всегда изменяем x
                e += dy * 2; // и увеличиваем ошибку
            }
        }
        else { // y уменьшается
            for (i = 0; i <= dx; i++) {
                cv::Point point(x, y);
                cv::circle(img, point, thickness, color, -1); // выводим точку
                if (e > 0) { // ошибка стала положительной. Условие изменилось с >= на >
                    y += iy;
                    e -= 2 * dx;
                }
                x += ix;
                e += dy * 2;
            }
        }
    }
    else {
        e = 2 * dx - dy; // инициализация ошибки с поправкой на половину пиксела

        if (ix >= 0) { // увеличиваем или не изменяем x
            // основной цикл
            for (i = 0; i <= dy; i++) {
                cv::Point point(x, y);
                cv::circle(img, point, thickness, color, -1); // выводим точку
                if (e >= 0) { // ошибка стала неотрицательной
                    x += ix; // изменяем y
                    e -= 2 * dy; // уменьшаем ошибку
                }
                y += iy; // всегда изменяем x
                e += dx * 2; // и увеличиваем ошибку
            }
        }
        else { // y уменьшается
            for (i = 0; i <= dy; i++) {
                cv::Point point(x, y);
                cv::circle(img, point, thickness, color, -1); // выводим точку
                if (e > 0) { // ошибка стала положительной. Условие изменилось с >= на >
                    x += ix;
                    e -= 2 * dy;
                }
                y += iy;
                e += dx * 2;
            }
        }
    }
}

// Функция для вычисления точки на кривой Безье третьего порядка
Point bezierPoint(const Point& p0, const Point& p1, const Point& p2, const Point& p3, double t) {
    double u = 1 - t;
    double tt = t * t;
    double uu = u * u;
    double uuu = uu * u;
    double ttt = tt * t;

    Point p;
    p.x = uuu * p0.x + 3 * uu * t * p1.x + 3 * u * tt * p2.x + ttt * p3.x;
    p.y = uuu * p0.y + 3 * uu * t * p1.y + 3 * u * tt * p2.y + ttt * p3.y;
    return p;
}

float Dist(cv::Point P0) {
    return abs(P0.x) + abs(P0.y);
}

// Функция для построения кривой Безье третьего порядка
void drawBezierCurve(Mat& img, const Point& p0, const Point& p1, const Point& p2, const Point& p3, const Scalar& color1) {

    float H = std::max(Dist(p0 - 2 * p1 + p2), Dist(p1 - 2 * p2 + p3));
    int N = 1 + static_cast<int>(sqrt(3 * H));
    float step = static_cast < float>(1) / N;

    vector<Point2f> points; 

    for (int i = 0; i <= N; i ++) {
        double t = static_cast<float>(i) / N;
        Point p = bezierPoint(p0, p1, p2, p3, t);
        points.push_back(Point2f(static_cast<int>(p.x), static_cast<int>(p.y))); // Convert to Point2f
    }

    // Рисуем кривую с помощью полилинии OpenCV
    if (points.size() > 1) {
        for (size_t i = 0; i < points.size() - 1; i++) {
            Line(img, points[i].x, points[i].y, points[i+1].x, points[i + 1].y, color1);
        }
    }
    vector<cv::Point> controlPoints = { p0, p1, p2, p3};

    for (auto& point : controlPoints) {
        Point t1, t2, t3, t4;
        t1.x = point.x - 4;
        t1.y = point.y;
        t2.x = point.x + 4;
        t2.y = point.y;
        t3.x = point.x;
        t3.y = point.y - 4;
        t4.x = point.x;
        t4.y = point.y + 4;

        line(img, t1, t2, 0x000000);
        line(img, t3, t4, 0x000000);
    }
}


int main() {
    // Создаем пустое изображение
    int width = 600;
    int height = 400;
    Mat img1(height, width, CV_8UC3, Scalar(255, 255, 255));

    Point p0 = { 100, 300 };
    Point p1 = { 150, 50 };
    Point p2 = { 400, 50 };
    Point p3 = { 450, 300 };

    // Рисуем кривую Безье
    Scalar color(255, 0, 0);
    drawBezierCurve(img1, p0, p1, p2, p3, color);

    // Отображаем изображение
    imshow("Bezier Curve", img1);

    Mat img2(height, width, CV_8UC3, Scalar(255, 255, 255));

    p0 = { 50, 350 };
    p1 = { 150, 50 };
    p2 = { 250, 350 };
    p3 = { 350, 50 };

    // Рисуем кривую Безь
    drawBezierCurve(img2, p0, p1, p2, p3, color);

    // Отображаем изображение
    imshow("Bezier Curve1", img2);

    Mat img3(height, width, CV_8UC3, Scalar(255, 255, 255));

    p0 = { 300, 350 };
    p1 = { 100, 100 };
    p2 = { 250, 100 };
    p3 = { 450, 200 };

    // Рисуем кривую Безье
    drawBezierCurve(img3, p0, p1, p2, p3, color);

    // Отображаем изображение
    imshow("Bezier Curve2", img3);

    waitKey(0);
    destroyAllWindows();

    return 0;
}