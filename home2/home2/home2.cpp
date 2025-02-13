#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>

using namespace cv;
using namespace std;


void drawControlPoints(Mat& img, const vector<Point2f>& controlPoints, const Scalar& color) {
    for (auto& point : controlPoints) {
        cv::Point2f t1, t2, t3, t4; // Используем cv::Point2f
        t1.x = point.x - 4;
        t1.y = point.y;

        t2.x = point.x + 4;
        t2.y = point.y;

        // Предполагаем, что drawLine определена (или используйте cv::line)
        line(img, t1, t2, color, 1, LINE_AA);  //рисуем горизонтальную линию

        t3.x = point.x;
        t3.y = point.y - 4;

        t4.x = point.x;
        t4.y = point.y + 4;
        line(img, t3, t4, color, 1, LINE_AA); //рисуем вертикальную линию
    }
}


// Функция для вычисления точки на кривой Безье n-го порядка (алгоритм де Кастельжо)
Point2f deCasteljau(const vector<Point2f>& controlPoints, double t) {
    vector<Point2f> points = controlPoints; // Создаем копию контрольных точек
    int n = points.size() - 1;

    for (int r = 1; r <= n; ++r) {
        for (int i = 0; i <= n - r; ++i) {
            points[i].x = (1 - t) * points[i].x + t * points[i + 1].x;
            points[i].y = (1 - t) * points[i].y + t * points[i + 1].y;
        }
    }

    return points[0]; // Результат - первая точка в массиве
}


// Функция для построения составной кривой Безье
void drawCompositeBezierCurve(Mat& img, const vector<vector<Point2f>>& controlPoints, const Scalar& color, int thickness = 1, double step = 0.01) {
    if (controlPoints.empty()) {
        cerr << "Error: No Bezier curve segments provided." << endl;
        return;
    }

    Point2f prevPoint;
    bool firstSegment = true;

    for (const auto& segmentControlPoints : controlPoints) {
        if (segmentControlPoints.size() < 2) {
            cerr << "Error: Bezier curve segment must have at least 2 control points." << endl;
            continue;
        }
        drawControlPoints(img, segmentControlPoints, (0, 0, 0));


        for (double t = 0; t <= 1; t += step) {
            Point2f currentPoint = deCasteljau(segmentControlPoints, t);

            if (firstSegment) {
                prevPoint = currentPoint;
                firstSegment = false;
                continue;
            }

            line(img, prevPoint, currentPoint, color, thickness, LINE_AA);
            prevPoint = currentPoint;
        }

        // Connect the last point of the current segment with the first point of the next
        if (controlPoints.size() > 1 && &segmentControlPoints != &controlPoints.back()) { // If it is not the last segment
            prevPoint = deCasteljau(segmentControlPoints, 1.0);
        }
    }


}


int main() {
    int width = 800;
    int height = 600;
    Mat img(height, width, CV_8UC3, Scalar(255, 255, 255));

    vector<vector<Point2f>> controlPoints = {
        {{100, 300}, {150, 100}, {250, 100}, {300, 300}},  // Сегмент 1 (Кубическая кривая Безье)
        {{300, 300}, {350, 500}, {450, 500}, {500, 300}},  // Сегмент 2 (Кубическая кривая Безье)
        {{500, 300}, {550, 100}, {650, 100}, {700, 300}}   // Сегмент 3 (Кубическая кривая Безье)
    };


    Scalar color(255, 0, 0); // Синий цвет
    drawCompositeBezierCurve(img, controlPoints, color, 2, 0.005);

    imshow("Composite", img);
    waitKey(0);
    destroyAllWindows();

    return 0;
}