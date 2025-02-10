#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

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

//Проверка на выпуклость
bool isConvex(const std::vector<Point>& points) {
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

//Вычисление площади
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

//Определение ориентации полигона
int isClockwise(const std::vector<cv::Point>& polygon) {
    if (polygon.size() < 3) {
        std::cout << "Polygon must have at least 3 vertices." << std::endl;
    }

    double orientedArea = calculateOrientedArea(polygon);
    return orientedArea > 0 ? 1 : 0;
}

// Функция для переориентации полигона
void reorientPolygon(std::vector<cv::Point>& polygon) {
    std::reverse(polygon.begin(), polygon.end());
}

// Функция для проверки, находится ли точка внутри полуплоскости, определяемой ребром
bool isInside(cv::Point p, cv::Point v1, cv::Point v2) {
    return (v2.x - v1.x) * (p.y - v1.y) - (v2.y - v1.y) * (p.x - v1.x) <= 0;
}

// Алгоритм Сазерленда-Ходжмана
std::vector<cv::Point> sutherlandHodgmanClip(const std::vector<cv::Point>& subjectPolygon, const std::vector<cv::Point>& clipPolygon) {
    std::vector<cv::Point> outputList = subjectPolygon;
    for (size_t j = 0; j < clipPolygon.size(); ++j) {
        std::vector<cv::Point> inputList = outputList;
        outputList.clear();

        cv::Point clipEdgeStart = clipPolygon[j];
        cv::Point clipEdgeEnd = clipPolygon[(j + 1) % clipPolygon.size()];

        for (size_t i = 0; i < inputList.size(); ++i) {
            cv::Point subjectVertex = inputList[i];
            cv::Point prevSubjectVertex = inputList[(i + inputList.size() - 1) % inputList.size()];

            // Если предыдущая вершина находится снаружи, а текущая - внутри
            if (!isInside(prevSubjectVertex, clipEdgeStart, clipEdgeEnd) && isInside(subjectVertex, clipEdgeStart, clipEdgeEnd)) {
                // Вычисляем точку пересечения и добавляем ее в выходной список
                double dx = subjectVertex.x - prevSubjectVertex.x;
                double dy = subjectVertex.y - prevSubjectVertex.y;
                double f = ((clipEdgeStart.x - prevSubjectVertex.x) * (clipEdgeEnd.y - clipEdgeStart.y) - (clipEdgeStart.y - prevSubjectVertex.y) * (clipEdgeEnd.x - clipEdgeStart.x)) / (dx * (clipEdgeEnd.y - clipEdgeStart.y) - dy * (clipEdgeEnd.x - clipEdgeStart.x));
                cv::Point intersection;
                intersection.x = prevSubjectVertex.x + dx * f;
                intersection.y = prevSubjectVertex.y + dy * f;
                cout << intersection << endl;
                outputList.push_back(intersection);
                outputList.push_back(subjectVertex);

            }
            // Если предыдущая вершина находится внутри, а текущая - тоже внутри
            else if (isInside(prevSubjectVertex, clipEdgeStart, clipEdgeEnd) && isInside(subjectVertex, clipEdgeStart, clipEdgeEnd)) {
                // Просто добавляем текущую вершину в выходной список
                outputList.push_back(subjectVertex);
            }
            // Если предыдущая вершина находится внутри, а текущая - снаружи
            else if (isInside(prevSubjectVertex, clipEdgeStart, clipEdgeEnd) && !isInside(subjectVertex, clipEdgeStart, clipEdgeEnd)) {
                // Вычисляем точку пересечения и добавляем ее в выходной список
                double dx = subjectVertex.x - prevSubjectVertex.x;
                double dy = subjectVertex.y - prevSubjectVertex.y;
                double f = ((clipEdgeStart.x - prevSubjectVertex.x) * (clipEdgeEnd.y - clipEdgeStart.y) - (clipEdgeStart.y - prevSubjectVertex.y) * (clipEdgeEnd.x - clipEdgeStart.x)) / (dx * (clipEdgeEnd.y - clipEdgeStart.y) - dy * (clipEdgeEnd.x - clipEdgeStart.x));
                cv::Point intersection;
                intersection.x = prevSubjectVertex.x + dx * f;
                intersection.y = prevSubjectVertex.y + dy * f;
                outputList.push_back(intersection);
            }
            //Если обе вершины снаружи - ничего не добавляем
        }
    }

    return outputList;
}

//Функция отсечения отрезка прямой выпуклым полигоном с помощью алгоритма Кируса-Бека
void CyrusBeckClipLine(cv::Mat& img, cv::Point& p1, cv::Point& p2, \
    const cv::Vec3b& color_line, const std::vector<cv::Point>& points, const cv::Vec3b& color_polygon) {

    std::vector<cv::Point> workingPoints = points; //Создаем локальную копию, с которой работаем
    // Проверяем выпуклость полигона
    if (!isPolygonConvex(workingPoints)) {
        std::cout << "Polygon is not convex! Attempting to reorient..." << std::endl;
        //В случае если полигон не выпуклый, отсечение не имеет смысла. Алгоритм не работает на невыпуклых полигонах
        return;
    }
    //Проверяем ориентацию полигона и переориентируем, если необходимо
    if (!isClockwise(workingPoints)) {
        std::cout << "Polygon is not correctly oriented! Reorienting..." << std::endl;
        reorientPolygon(workingPoints);
    }

    DrawPolygon(img, workingPoints, color_polygon);
    Line(img, p1.x, p1.y, p2.x, p2.y, color_line);

    int n = workingPoints.size();
    double t1 = 0, t2 = 1, t;
    double sx = p2.x - p1.x, sy = p2.y - p1.y;
    double nx, ny, denom, num;

    for (int i = 0; i < n; i++) {
        nx = workingPoints[(i + 1) % n].y - workingPoints[i].y;
        ny = workingPoints[i].x - workingPoints[(i + 1) % n].x;
        denom = nx * sx + ny * sy;
        num = nx * (p1.x - workingPoints[i].x) + ny * (p1.y - workingPoints[i].y);

        if (denom != 0) {
            t = -num / denom;
            if (denom > 0) {
                if (t > t1) t1 = t;
            }
            else {
                if (t < t2) t2 = t;
            }
        }
        else {
            if (abs(denom) < 1e-6) { // Проверяем, что denom "почти" равен 0
                if (num > 1e-6 || num < -1e-6) { // Отрезок снаружи ребра
                    // Отрезок параллелен ребру и находится вне полигона.
                    return;
                }
            }
        }
        if (t1 > t2) return;
    }

    if (t1 <= t2) {
        cv::Point2f p1_new = cv::Point2f(p1) + t1 * (cv::Point2f(p2.x, p2.y) - cv::Point2f(p1.x, p1.y));
        cv::Point2f p2_new = cv::Point2f(p1) + t2 * (cv::Point2f(p2.x, p2.y) - cv::Point2f(p1.x, p1.y));


        Line(img, (int)p1_new.x, (int)p1_new.y, (int)p2_new.x, (int)p2_new.y, cv::Vec3b(255, 0, 255));

        p1.x = (int)p1_new.x;
        p1.y = (int)p1_new.y;
        p2.x = (int)p2_new.x;
        p2.y = (int)p2_new.y;
    }
}

int main() {
    // Создаем изображение
    Mat img(500, 500, CV_8UC3, Scalar(255, 255, 255));

    // Определение отсекающего и отсекаемого полигонов
    vector<Point> clipPolygon = {
        Point(20, 20),
        Point(20, 60),
        Point(60, 60),
        Point(60, 20),
    };

    vector<Point> subjectPolygon = {
        Point(70, 20),
        Point(70, 50),
        Point(100, 50),
        Point(100, 20),
    };

    // Проверяем и переориентируем отсекающий полигон
    if (!isConvex(clipPolygon)) {
        cout << "Clipping polygon is not convex!" << endl;
        return -1;
    }
    if (!isClockwise(clipPolygon)) {
        cout << "Reorienting the clipping polygon..." << endl;
        reorientPolygon(clipPolygon);
    }
    
    // Вызываем алгоритм Сазерленда-Ходжмана
    vector<Point> clippedPolygon = sutherlandHodgmanClip(subjectPolygon, clipPolygon);
    // Вывод результата
    DrawPolygon(img, clipPolygon, Scalar(0, 0, 0)); 
    DrawPolygon(img, subjectPolygon, Scalar(255, 0, 0)); 
    
    if (!clippedPolygon.empty()) {
        DrawPolygon(img, clippedPolygon, Scalar(0, 255, 0)); // Зеленый - результат отсечения
    }

    // Определение отсекающего и отсекаемого полигонов
    clipPolygon = {
        Point(20, 20),
        Point(20, 60),
        Point(60, 60),
        Point(60, 20),
    };
    
    subjectPolygon = {
        Point(25, 25),
        Point(25, 50),
        Point(50, 50),
        Point(50, 25),
    };

    // Проверяем и переориентируем отсекающий полигон
    if (!isConvex(clipPolygon)) {
        cout << "Clipping polygon is not convex!" << endl;
        return -1;
    }
    if (!isClockwise(clipPolygon)) {
        cout << "Reorienting the clipping polygon..." << endl;
        reorientPolygon(clipPolygon);
    }

    // Вызываем алгоритм Сазерленда-Ходжмана
    clippedPolygon = sutherlandHodgmanClip(subjectPolygon, clipPolygon);
    // Вывод результата
    DrawPolygon(img, clipPolygon, Scalar(0, 0, 0));
    DrawPolygon(img, subjectPolygon, Scalar(255, 0, 0));

    if (!clippedPolygon.empty()) {
        DrawPolygon(img, clippedPolygon, Scalar(0, 255, 0)); // Зеленый - результат отсечения
    }


    // Определение отсекающего и отсекаемого полигонов
    clipPolygon = {
        Point(200, 200),
        Point(400, 200),
        Point(400, 400),
        Point(200, 400),
    };

    subjectPolygon = {
        Point(250, 250),
        Point(150, 250),
        Point(150, 350)
    };

    // Проверяем и переориентируем отсекающий полигон
    if (!isConvex(clipPolygon)) {
        cout << "Clipping polygon is not convex!" << endl;
        return -1;
    }
    if (!isClockwise(clipPolygon)) {
        cout << "Reorienting the clipping polygon..." << endl;
        reorientPolygon(clipPolygon);
    }

    // Вызываем алгоритм Сазерленда-Ходжмана
    clippedPolygon = sutherlandHodgmanClip(subjectPolygon, clipPolygon);
    // Вывод результата
    DrawPolygon(img, clipPolygon, Scalar(0, 0, 0));
    DrawPolygon(img, subjectPolygon, Scalar(255, 0, 0));

    if (!clippedPolygon.empty()) {
        DrawPolygon(img, clippedPolygon, Scalar(0, 255, 0)); // Зеленый - результат отсечения
    }


    // Определение отсекающего и отсекаемого полигонов
    clipPolygon = {
        Point(200, 200),
        Point(400, 200),
        Point(400, 400),
        Point(200, 400),
    };

    subjectPolygon = {
        Point(400, 250),
        Point(400, 350),
        Point(450, 350)
    };

    // Проверяем и переориентируем отсекающий полигон
    if (!isConvex(clipPolygon)) {
        cout << "Clipping polygon is not convex!" << endl;
        return -1;
    }
    if (!isClockwise(clipPolygon)) {
        cout << "Reorienting the clipping polygon..." << endl;
        reorientPolygon(clipPolygon);
    }

    // Вызываем алгоритм Сазерленда-Ходжмана
    clippedPolygon = sutherlandHodgmanClip(subjectPolygon, clipPolygon);
    // Вывод результата
    DrawPolygon(img, subjectPolygon, Scalar(255, 0, 0));
    cout << clippedPolygon;
    if (!clippedPolygon.empty()) {
        DrawPolygon(img, clippedPolygon, Scalar(0, 255, 0)); // Зеленый - результат отсечения
    }


    imshow("Sutherland-Hodgman Clipping", img);


    Mat img2(500, 500, CV_8UC3, Scalar(255, 255, 255));

    clipPolygon = {
        Point(200, 200),
        Point(400, 200),
        Point(400, 400),
        Point(200, 400),
    };
    Point p1(170, 150);
    Point p2(450, 450);
    CyrusBeckClipLine(img2, p1, p2, Vec3b(0, 255, 255), clipPolygon, Vec3b(0, 0, 0));

    p1 = Point(230, 450);
    p2 = Point(260, 340);
    CyrusBeckClipLine(img2, p1, p2, Vec3b(0, 255, 255), clipPolygon, Vec3b(0, 0, 0));

    p1 = Point(230, 350);
    p2 = Point(260, 290);
    CyrusBeckClipLine(img2, p1, p2, Vec3b(0, 255, 255), clipPolygon, Vec3b(0, 0, 0));

    p1 = Point(20, 450);
    p2 = Point(20, 20);
    CyrusBeckClipLine(img2, p1, p2, Vec3b(0, 255, 255), clipPolygon, Vec3b(0, 0, 0));


    imshow("Cyrus-Beck Clipping", img2);
    
    waitKey(0);
    destroyAllWindows();

    return 0;
}