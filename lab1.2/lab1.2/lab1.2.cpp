#include <opencv2/opencv.hpp>
#include <iostream>

#include <opencv2/opencv.hpp>
#include <iostream>

cv::Mat blendImages(const cv::Mat& image1, const cv::Mat& image2, const cv::Mat& alpha) {
    // Проверка на совпадение размеров
    if (image1.size() != image2.size() || image1.size() != alpha.size()) {
        throw std::invalid_argument("All input images must have the same size.");
    }

    // Проверка на тип изображений
    if (image1.type() != CV_8UC3 || image2.type() != CV_8UC3 || alpha.type() != CV_8UC1) {
        throw std::invalid_argument("image1 and image2 must be CV_8UC3, alpha must be CV_8UC1.");
    }

    // Создание выходного изображения
    cv::Mat blended(image1.size(), image1.type());

    // Смешивание изображений
    for (int row = 0; row < image1.rows; ++row) {
        for (int col = 0; col < image1.cols; ++col) {
            cv::Vec3b pixel1 = image1.at<cv::Vec3b>(row, col);
            cv::Vec3b pixel2 = image2.at<cv::Vec3b>(row, col);
            uchar alphaValue = alpha.at<uchar>(row, col);

            // Нормализуем альфа-канал
            float alphaNormalized = alphaValue / 255.0f;

            // Рассчитываем смешанные значения пикселей
            cv::Vec3b blendedPixel;
            blendedPixel[0] = static_cast<uchar>(pixel1[0] * (1.0 - alphaNormalized) + pixel2[0] * alphaNormalized);
            blendedPixel[1] = static_cast<uchar>(pixel1[1] * (1.0 - alphaNormalized) + pixel2[1] * alphaNormalized);
            blendedPixel[2] = static_cast<uchar>(pixel1[2] * (1.0 - alphaNormalized) + pixel2[2] * alphaNormalized);

            blended.at<cv::Vec3b>(row, col) = blendedPixel;
        }
    }

    return blended;
}


int main() {
    // Загружаем изображения
    cv::Mat image2 = cv::imread("abhazia.png"); // Путь к первому изображению
    cv::Mat image1 = cv::imread("kitten.png"); // Путь ко второму изображению
    cv::Mat alpha = cv::imread("gradient.png", cv::IMREAD_GRAYSCALE); // Путь к альфа-каналу

    cv::Mat output;

    // Вызываем функцию для смешивания изображений
    try {
        cv::Mat result = blendImages(image1, image2, alpha);
        cv::imwrite("blended_image.png", result);
        std::cout << "Смешивание завершено. Результат сохранен как blended_image.png." << std::endl;
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }



    return 0;
}