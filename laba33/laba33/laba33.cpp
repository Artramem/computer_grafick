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



enum PolygonType { SIMPLE, COMPLEX, CONVEX, CONCAVE };

struct PolygonTypes {
    PolygonType type1;
    PolygonType type2;
};

struct Segment {
    Point start, end;
};

// Функция для вычисления ориентации трех точек
int orientation(Point p, Point q, Point r) {
    double val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    if (val == 0) return 0;  // Коллинеарные
    return (val > 0) ? 1 : 2; // 1 -> часовая, 2 -> против часовой
}
// Вспомогательная функция для определения пересечения отрезков
bool linesIntersect(Point p1, Point p2, Point p3, Point p4) {
    int x1 = p1.x;
    int x2 = p2.x;
    int x3 = p3.x;
    int x4 = p4.x;
    int y1 = p1.y;
    int y2 = p2.y;
    int y3 = p3.y;
    int y4 = p4.y;
    double denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

    if (denom == 0) {
        return false;
    }
    else {
        double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denom;
        double u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / denom;
        return (t >= 0 && t <= 1 && u >= 0 && u <= 1);
    }
}

// Функция для проверки выпуклости полигона
bool isConvexPolygon(const std::vector<Point>& points) {
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




PolygonTypes getPolygonType(const vector<Point>& points) {
    PolygonTypes result;
    // Проверка на простоту
    int intersections = 0;
    for (int i = 0; i < points.size() - 1; ++i) {
        int j = i + 1;
        for (int k = 0; k < points.size() - 1; ++k) {
            int l = k + 1;
            if (i != k && i != l && j != k && j != l) {
                if (linesIntersect(points[i], points[j], points[k], points[l])) {
                    intersections++;
                    break;
                }
            }
        }
    }
    PolygonType type1, type2;

    // Проверка на самопересечения
    if (intersections > 0) {
        result.type1 = COMPLEX;
    }
    else {
        result.type1 = SIMPLE;
    }
    if (isConvexPolygon(points)) {
        result.type2 = CONVEX;
    }
    else {
        result.type2 = CONCAVE;
    }
    return result;
}


bool isInsideEvenOdd(const vector<Point>& points, const Point& p) {
    int count = 0;
    int n = points.size();

    for (int i = 0; i < n; i++) {
        Point p1 = points[i];
        Point p2 = points[(i + 1) % n];

        if ((p1.y > p.y) != (p2.y > p.y)) {
            double xIntersection = (double)(p2.x - p1.x) * (p.y - p1.y) / (p2.y - p1.y) + p1.x;
            if (p.x < xIntersection) {
                count++;
            }
        }
    }
    return (count % 2) == 1;
}
bool isInsideNonZeroWinding(const vector<Point>& points, const Point& p) {
    int windingNumber = 0;
    int n = points.size();

    for (int i = 0; i < n; i++) {
        Point p1 = points[i];
        Point p2 = points[(i + 1) % n];

        if (p1.y <= p.y) {
            if (p2.y > p.y && (p2.x - p1.x) * (p.y - p1.y) - (p2.y - p1.y) * (p.x - p1.x) > 0) {
                windingNumber++;
            }
        }
        else {
            if (p2.y <= p.y && (p2.x - p1.x) * (p.y - p1.y) - (p2.y - p1.y) * (p.x - p1.x) < 0) {
                windingNumber--;
            }
        }
    }
    return windingNumber != 0;
}


void FillPolygon(Mat& img, const vector<Point>& points, Scalar color1, bool useEvenOddRule) {
    cv::Scalar color = cv::Scalar(color1[2], color1[1], color1[0]); //  RGB
    int minX = img.cols, maxX = 0, minY = img.rows, maxY = 0;
    for (const Point& p : points) {
        minX = min(minX, p.x);
        maxX = max(maxX, p.x);
        minY = min(minY, p.y);
        maxY = max(maxY, p.y);
    }

    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            Point p(x, y);
            bool inside = useEvenOddRule ? isInsideEvenOdd(points, p) : isInsideNonZeroWinding(points, p);
            if (inside) {
                cv::Point point(x, y);
                cv::circle(img, point, 1, color, -1); // выводим точку
            }
        }
    }
}


int main() {
    // Создаем изображение 
    cv::Mat img(400, 400, CV_8UC3, cv::Scalar(255, 255, 255));
    cv::Mat image1(600, 600, CV_8UC3, cv::Scalar(255, 255, 255));
    cv::Mat image2(600, 600, CV_8UC3, cv::Scalar(255, 255, 255));

    // Рисуем линию с помощью нашей функции Line()
    Line(img, 10, 50, 90, 100, cv::Scalar(255, 0, 0));    //dx > 0, dy > 0, dx > dy
    Line(img, 90, 50, 10, 100, cv::Scalar(0, 255, 0));    //dx < 0, dy > 0, dx > dy
    Line(img, 90, 50, 90, 100, cv::Scalar(0, 0, 255));    //dx = 0, dy > 0, dx < dy
    Line(img, 200, 100, 250, 50, cv::Scalar(0, 0, 0));  //dx > 0, dy < 0, dx > dy
    Line(img, 200, 100, 250, 100, cv::Scalar(200, 200, 0)); //dx > 0, dy = 0, dx > dy
    Line(img, 200, 100, 250, 20, cv::Scalar(0, 200, 200));  //dx > 0, dy < 0, dx < dy
    Line(img, 300, 300, 300, 300, cv::Scalar(0, 0, 0));     //линия нулевой длины


    // Рисуем треугольник

    vector<Point> points = {
        Point(100, 100),
        Point(150, 190),
        Point(190, 100)
    };

    DrawPolygon(img, points, Scalar(0, 0, 0));

    PolygonTypes type = getPolygonType(points);
    switch (type.type1) {
    case SIMPLE:
        std::cout << "Type 1: SIMPLE" << std::endl;
        break;
    case COMPLEX:
        std::cout << "Type 1: COMPLEX" << std::endl;
        break;
    }
    switch (type.type2) {
    case CONVEX:
        std::cout << "Type 2: CONVEX" << std::endl;
        break;
    case CONCAVE:
        std::cout << "Type 2: CONCAVE" << std::endl;
        break;
    }
    //Сложный полигон с толщиной 5
    points = {
       Point(220, 220),
       Point(280, 220),
       Point(280, 280),
       Point(320, 280),
       Point(320, 320),
       Point(280, 320),
       Point(280, 380),
       Point(220, 380),
       Point(220, 320),
       Point(260, 320),
       Point(260, 260),
       Point(220, 260)
    };
    DrawPolygon(img, points, Scalar(0, 0, 255), 5);

    type = getPolygonType(points);
    switch (type.type1) {
    case SIMPLE:
        std::cout << "Type 1: SIMPLE" << std::endl;
        break;
    case COMPLEX:
        std::cout << "Type 1: COMPLEX" << std::endl;
        break;
    }

    switch (type.type2) {
    case CONVEX:
        std::cout << "Type 2: CONVEX" << std::endl;
        break;
    case CONCAVE:
        std::cout << "Type 2: CONCAVE" << std::endl;
        break;
    }

    // Полигон с самопересенчением
    points = {
        Point(100, 300),
        Point(180, 350),
        Point(170, 390),
        Point(150, 250)
    };
    DrawPolygon(img, points, Scalar(200, 0, 255));


    type = getPolygonType(points);
    switch (type.type1) {
    case SIMPLE:
        std::cout << "Type 1: SIMPLE" << std::endl;
        break;
    case COMPLEX:
        std::cout << "Type 1: COMPLEX" << std::endl;
        break;
    }

    switch (type.type2) {
    case CONVEX:
        std::cout << "Type 2: CONVEX" << std::endl;
        break;
    case CONCAVE:
        std::cout << "Type 2: CONCAVE" << std::endl;
        break;
    }

    points = {
        Point(10, 10),
        Point(200, 100),
        Point(20, 300),
        Point(150, 30),
        Point(100, 330)};

    DrawPolygon(image1, points, Scalar(100, 100, 255));
    FillPolygon(image1, points, Scalar(100, 100, 255), true);


    DrawPolygon(image2, points, Scalar(100, 100, 255));
    FillPolygon(image2, points, Scalar(100, 100, 255), false);

    points = {
    Point(300, 10),
    Point(490, 100),
    Point(350, 300),
    Point(390, 10),
    Point(500, 330),
    Point(420, 360)};

    DrawPolygon(image1, points, Scalar(100, 100, 255));
    FillPolygon(image1, points, Scalar(100, 100, 255), true);


    DrawPolygon(image2, points, Scalar(100, 100, 255));
    FillPolygon(image2, points, Scalar(100, 100, 255), false);
    // Выводим изображение
    cv::imshow("Line", img);
    cv::imshow("Polygon1", image1);
    cv::imshow("Polygon2", image2);
    cv::waitKey(0);

    return 0;
}