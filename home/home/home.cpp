#include <opencv2/opencv.hpp>
#include <iostream>
#include <queue>

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

// Функция для 8-связного построчного заполнения
void lineSeedFill8(Mat& img, int x, int y, const Scalar& targetColor, const Scalar& fillColor) {
    // Проверка границ
    if (x < 0 || x >= img.cols || y < 0 || y >= img.rows) {
        return;
    }

    // Получаем цвет текущего пикселя
    Vec3b currentColor = img.at<Vec3b>(y, x);

    // Проверяем, соответствует ли цвет текущего пикселя целевому цвету
    if (cv::norm(Scalar(currentColor) - targetColor) > 1e-6) {  //Используем norm для сравнения Scalar
        return;
    }

    // Заполняем текущую строку слева направо
    int xLeft = x;
    while (xLeft >= 0 && cv::norm(Scalar(img.at<Vec3b>(y, xLeft)) - targetColor) < 1e-6) {
        img.at<Vec3b>(y, xLeft) = Vec3b(fillColor[0], fillColor[1], fillColor[2]);
        xLeft--;
    }
    xLeft++;  // Возвращаемся к самому левому заполненному пикселю

    int xRight = x + 1;
    while (xRight < img.cols && cv::norm(Scalar(img.at<Vec3b>(y, xRight)) - targetColor) < 1e-6) {
        img.at<Vec3b>(y, xRight) = Vec3b(fillColor[0], fillColor[1], fillColor[2]);
        xRight++;
    }
    xRight--; // Возвращаемся к самому правому заполненному пикселю

    // Рекурсивно вызываем функцию для строк сверху и снизу
    for (int i = xLeft; i <= xRight; i++) {
        lineSeedFill8(img, i, y - 1, targetColor, fillColor); // Строка выше
        lineSeedFill8(img, i, y + 1, targetColor, fillColor); // Строка ниже
    }
}

int main() {
    // Создаем изображение (пример)
    Mat img(400, 600, CV_8UC3, Scalar(255, 255, 255)); // Белый фон

    // Рисуем прямоугольник (ограничиваем область для заполнения)
    rectangle(img, Point(20, 20), Point(50, 40), Scalar(0, 0, 0), 2); // Черный контур

    // Задаем "затравку" (начальную точку для заполнения)
    int seedX = 22;
    int seedY = 22;


    // Задаем целевой цвет (цвет области, которую нужно закрасить)
    Scalar targetColor = Scalar(255, 255, 255); // Белый

    // Задаем цвет заливки
    Scalar fillColor = Scalar(0, 255, 0); // Зеленый

    // Вызываем функцию для построчного заполнения с затравкой
    lineSeedFill8(img, seedX, seedY, targetColor, fillColor);

    vector<Point> points = {
       Point(100, 100),
       Point(150, 190),
       Point(190, 100)
    };

    DrawPolygon(img, points, Scalar(0, 0, 0));
    seedX = 110;
    seedY = 110;
    lineSeedFill8(img, seedX, seedY, targetColor, fillColor);

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

    DrawPolygon(img, points, Scalar(0, 0, 255));

    seedX = 230;
    seedY = 350;
    lineSeedFill8(img, seedX, seedY, targetColor, fillColor);
    // Отображаем изображение
    imshow("Flood Fill (Line Seed)", img);
    waitKey(0);
    destroyAllWindows();

    return 0;
}