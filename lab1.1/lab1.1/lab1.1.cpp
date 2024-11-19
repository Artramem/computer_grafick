#include <opencv2/opencv.hpp>
#include <iostream>

void displayCircularGrayscaleImage(const cv::Mat& image, const std::string& output_filename) {
    // Проверяем, что входящее изображение не пустое
    if (image.empty()) {
        std::cerr << "Ошибка: входящее изображение пустое!" << std::endl;
        return;
    }

    // Преобразуем изображение в полутоновое
    cv::Mat grayImage(image.size(), CV_8UC1, cv::Scalar(255));
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

    // Создаем маску в форме круга
    cv::Mat mask = cv::Mat::zeros(grayImage.size(), CV_8UC1);
    cv::Point center(mask.cols / 2, mask.rows / 2); // Центр круга
    int radius = std::min(mask.cols, mask.rows) / 3; // Радиус круга
    cv::circle(mask, center, radius, cv::Scalar(255), -1); // Заполняем круг белым цветом

    // Применяем маску к полутоновому изображению
    cv::Mat circularImage(grayImage.size(), CV_8UC1, cv::Scalar(255));
    cv::bitwise_and(grayImage, grayImage, circularImage, mask);

    // Показываем результирующее круглое полутоновое изображение
    cv::imshow("Circular Grayscale Image", circularImage);
    cv::waitKey(0); // Ждем нажатия клавиши перед закрытием окна
    cv::imwrite(output_filename, circularImage);
}

int main() {
    // Загружаем изображение
    cv::Mat image = cv::imread("ball.png"); // Укажите путь к вашему изображению

    // Вызываем функцию для отображения круглого полутонового изображения
    displayCircularGrayscaleImage(image, "ball.png");

    return 0;
}