#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;


Mat floydSteinbergDithering(Mat& img, int n) {

    Mat out_img = Mat::zeros(img.size(), CV_8U);  
    for (int y = 0; y < img.rows; y++) {
        for (int x = 0; x < img.cols; x++) {

            
            uchar pixel = img.at<uchar>(y, x);

            
            uchar quantized_pixel = pixel >> (8 - n) << (8 - n);
            int error = pixel - quantized_pixel;

            if (x + 1 < img.cols) {
                
                img.at<uchar>(y, x + 1) = std::min(255, std::max(0, img.at<uchar>(y, x + 1) + error * 7 / 16));
            }
            if (y + 1 < img.rows) {
                
                img.at<uchar>(y + 1, x) = std::min(255, std::max(0, img.at<uchar>(y + 1, x) + error * 5 / 16));
                if (x > 0) {
                    
                    img.at<uchar>(y + 1, x - 1) = std::min(255, std::max(0, img.at<uchar>(y + 1, x - 1) + error * 3 / 16));
                }
                if (x + 1 < img.cols) {

                    img.at<uchar>(y + 1, x + 1) = std::min(255, std::max(0, img.at<uchar>(y + 1, x + 1) + error * 1 / 16));
                }
            }

            
            out_img.at<uchar>(y, x) = quantized_pixel;
        }
    }

    return out_img;
}


int main() {
    Mat src = imread("kitten.png", IMREAD_GRAYSCALE); 

    if (src.empty()) {
        cerr << "Не удалось загрузить изображение!" << endl;
        return -1;
    }

    // Преобразуем в n-уровневое изображение
    imwrite("kitten_imput.png", src);

    src = imread("kitten.png", IMREAD_GRAYSCALE);
    int n = 1;
    Mat dst1 = floydSteinbergDithering(src, n);

    // Отображаем изображения
    imwrite("kitten_output1.png", dst1);

    src = imread("kitten.png", IMREAD_GRAYSCALE);
    n = 2;
    Mat dst2 = floydSteinbergDithering(src, n);
    imwrite("kitten_output2.png", dst2);

    src = imread("kitten.png", IMREAD_GRAYSCALE);
    n = 3;
    Mat dst3 = floydSteinbergDithering(src, n);
    imwrite("kitten_output3.png", dst3);

    src = imread("kitten.png", IMREAD_GRAYSCALE);
    n = 4;
    Mat dst4 = floydSteinbergDithering(src, n);
    imwrite("kitten_output4.png", dst4);

    waitKey(0);
    destroyAllWindows();
    return 0;
}