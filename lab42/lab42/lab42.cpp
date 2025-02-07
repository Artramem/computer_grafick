#include <opencv2/opencv.hpp> 
#include <iostream> 
#include <vector>

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


void DrawPolygon(Mat& img, vector<Point> points, Scalar color, int thickness = 1) {
    // Проходим по всем точкам полигона, рисуя линии между соседними точками
    for (size_t i = 0; i < points.size() - 1; ++i) {
        Line(img, points[i].x, points[i].y, points[i + 1].x, points[i + 1].y, color, thickness);
    }
    // Закрываем полигон, соединив последнюю точку с первой
    Line(img, points[points.size() - 1].x, points[points.size() - 1].y, points[0].x, points[0].y, color, thickness);
}

int orientation(Point p, Point q, Point r) {
    double val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    if (val == 0) return 0;  // Коллинеарные
    return (val > 0) ? 1 : 2; // 1 -> часовая, 2 -> против часовой
}

bool isPolygonConvex(const std::vector<Point>& points) {
    int n = points.size();
    if (n < 3) return false; // Полигоны меньше 3 точек не существуют

    int prevOrientation = 0; // Переменная для отслеживания предыдущей ориентации

    for (int i = 0; i < n; ++i) {
        // Получаем текущую тройку точек
        Point p = points[i];
        Point q = points[(i + 1) % n];
        Point r = points[(i + 2) % n];

        // Вычисляем ориентацию тройки точек
        int currOrientation = orientation(p, q, r);

        // Если первый переход, сохраняем ориентацию
        if (currOrientation != 0) {
            if (prevOrientation == 0) {
                prevOrientation = currOrientation; // Сохраняем первую ненулевую ориентацию
            }
            else {
                // Проверяем, изменилось ли направление
                if (currOrientation != prevOrientation) {
                    return false; // Полигон не выпуклый
                }
            }
        }
    }
    return true; // Все углы выпуклые
}

double calculateOrientedArea(const std::vector<cv::Point>& points) {
    int n = points.size();
    double area = 0;

    for (int i = 0; i < n; ++i) {
        double x1 = points[i].x;
        double y1 = points[i].y;
        double x2 = points[(i + 1) % n].x;
        double y2 = points[(i + 1) % n].y;
        area += (x2 - x1) * (y2 + y1);
    }

    return area;
}

int isClockwise(const std::vector<cv::Point>& polygon) {
    if (polygon.size() < 3) {
        std::cout << "Polygon must have at least 3 vertices." << std::endl;
    }

    double orientedArea = calculateOrientedArea(polygon);
    return orientedArea > 0 ? 1 : 0;
}


void CyrusBeckClipLine(cv::Mat& img, cv::Point& p1, cv::Point& p2, \
    const cv::Vec3b& color_line, const std::vector<cv::Point>& points, const cv::Vec3b& color_polygon) {

    if (!isPolygonConvex(points)) {
        std::cout << "Polygon is not convex!" << std::endl;
        return;
    }
    // checking for the correct orientation of the polygon
    if (!isClockwise(points)) {
        std::cout << "Polygon is not correctly oriented!" << std::endl;
        return;
    }

    DrawPolygon(img, points, color_polygon);
    Line(img, p1.x, p1.y, p2.x, p2.y, color_line);

    int n = points.size();
    double t1 = 0, t2 = 1, t;
    double sx = p2.x - p1.x, sy = p2.y - p1.y;
    double nx, ny, denom, num;

    for (int i = 0; i < n; i++) {
        nx = points[(i + 1) % n].y - points[i].y;
        ny = points[i].x - points[(i + 1) % n].x;
        denom = nx * sx + ny * sy;
        num = nx * (p1.x - points[i].x) + ny * (p1.y - points[i].y);

        if (denom != 0) {
            t = -num / denom;
            if (denom > 0) {
                if (t > t1) t1 = t;
            }
            else {
                if (t < t2) t2 = t;
            }
        }
        if (t1 > t2) return;
    }

    if (t1 <= t2) {
        cv::Point2f p1_new = p1 + t1 * (p2 - p1);
        cv::Point2f p2_new = p1 + t2 * (p2 - p1);

        Line(img, p1_new.x, p1_new.y, p2_new.x, p2_new.y, cv::Vec3b(0, 0, 255));

        p1 = p1_new;
        p2 = p2_new;
    }
}

int main() {
    // Создаем холст (изображение) размером 500x500, фон - белый
    cv::Mat img(800, 800, CV_8UC3, cv::Scalar(255, 255, 255));
    std::vector<cv::Point> polygonPoints = { {100, 300},{200, 600},{560, 500},{650, 200}, { 150, 150 } };
    cv::Point p1(50, 50);
    cv::Point p2(750, 750);
    CyrusBeckClipLine(img, p1, p2, cv::Vec3b(0, 0, 0), polygonPoints, cv::Vec3b(200, 0, 200));


    cv::Point p3(350, 220);
    cv::Point p4(600, 750);
    CyrusBeckClipLine(img, p3, p4, cv::Vec3b(0, 255, 0), polygonPoints, cv::Vec3b(200, 0, 200));
    

    cv::Point p5(150, 300);
    cv::Point p6(380, 450);
    CyrusBeckClipLine(img, p5, p6, cv::Vec3b(254, 100, 0), polygonPoints, cv::Vec3b(200, 0, 200));
    

    cv::Point p7(600, 700);
    cv::Point p8(780, 50);
    CyrusBeckClipLine(img, p7, p8, cv::Vec3b(150, 100, 0), polygonPoints, cv::Vec3b(200, 66, 116));

    cv::imwrite("cyrusbeck.png", img);

    cv::waitKey(0);
    return 0;
}