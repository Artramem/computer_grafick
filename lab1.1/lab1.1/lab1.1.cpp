#include <opencv2/opencv.hpp>
#include <iostream>
#include <iostream>
#include <vector>
#include <cmath>


struct BGRPixel {
    unsigned char b;
    unsigned char g;
    unsigned char r;
};

// Структура для представления полутонового пикселя
struct GrayPixel {
    unsigned char gray;
};

// Функция для преобразования изображения BGR в полутоновое
cv::Mat bgrToGray(const cv::Mat& bgrImage) {
    if (bgrImage.empty()) {
        return cv::Mat(); // Возвращаем пустую матрицу, если изображение пустое
    }

    int rows = bgrImage.rows;
    int cols = bgrImage.cols;
    cv::Mat grayImage(rows, cols, CV_8UC1);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            const cv::Vec3b& bgrPixel = bgrImage.at<cv::Vec3b>(i, j);
            // Рассчитываем значение яркости
            unsigned char grayValue = static_cast<unsigned char>(
                std::round(0.114 * bgrPixel[0] + 0.587 * bgrPixel[1] + 0.299 * bgrPixel[2])
                );
            grayImage.at<unsigned char>(i, j) = grayValue;
        }
    }
    return grayImage;
}


cv::Mat applyCircularMask(const cv::Mat& grayImage) {
    if (grayImage.empty()) {
        return cv::Mat(); // Возвращаем пустую матрицу, если изображение пустое
    }

    int rows = grayImage.rows;
    int cols = grayImage.cols;
    cv::Mat circularImage = cv::Mat::zeros(rows, cols, CV_8UC1);


    cv::Point center(cols / 2, rows / 2);
    int radius = std::min(cols, rows) / 3;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            // Вычисляем расстояние от текущего пикселя до центра
            double distance = std::sqrt(std::pow(i - center.y, 2) + std::pow(j - center.x, 2));

            if (distance <= radius) {
                circularImage.at<unsigned char>(i, j) = grayImage.at<unsigned char>(i, j);
            }
        }
    }
    return circularImage;
}

void displayCircularGrayscaleImage(const cv::Mat& image, const std::string& output_filename) {
    // Проверяем, что входящее изображение не пустое
    if (image.empty()) {
        std::cerr << "Ошибка: входящее изображение пустое!" << std::endl;
        return;
    }

    // Преобразуем изображение в полутоновое 
    cv::Mat grayImage = bgrToGray(image);
    if (grayImage.empty())
    {
        std::cerr << "Ошибка при преобразовании в полутоновое изображение" << std::endl;
        return;
    }

    cv::Mat circularImage = applyCircularMask(grayImage);
    if (circularImage.empty())
    {
        std::cerr << "Ошибка при применении маски к изображению" << std::endl;
        return;
    }


    // Показываем результирующее круглое полутоновое изображение
    cv::imshow("Circular Grayscale Image", circularImage);
    cv::waitKey(0); // Ждем нажатия клавиши перед закрытием окна
    cv::imwrite(output_filename, circularImage);
}

int main() {
    // Загружаем изображение
    cv::Mat image = cv::imread("ball.png"); // Укажите путь к вашему изображению

    // Вызываем функцию для отображения круглого полутонового изображения
    displayCircularGrayscaleImage(image, "ball_out.png");

    return 0;
}
