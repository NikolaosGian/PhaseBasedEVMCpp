#include <opencv2/opencv.hpp>

// Function to generate polar grids of dimensions DIMENSIONS
void getPolarGrid(const cv::Size& dimension, cv::Mat& angle, cv::Mat& rad) {
    cv::Point center((dimension.width + 1) / 2, (dimension.height + 1) / 2);

    // Create rectangular grid
    cv::Mat xramp(dimension, CV_32F);
    cv::Mat yramp(dimension, CV_32F);
    for (int i = 0; i < dimension.height; ++i) {
        for (int j = 0; j < dimension.width; ++j) {
            xramp.at<float>(i, j) = (j - center.x) / (static_cast<float>(dimension.width) / 2.0);
            yramp.at<float>(i, j) = (i - center.y) / (static_cast<float>(dimension.height) / 2.0);
        }
    }

    // Convert to polar coordinates
    cv::cartToPolar(xramp, yramp, rad, angle, true);

    // Eliminate places where rad is zero, so logarithm is well-defined
    rad.at<float>(center.y, center.x) = rad.at<float>(center.y, center.x - 1);
}
