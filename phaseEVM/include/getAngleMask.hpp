#include <opencv2/opencv.hpp>
#include <cmath>

// Function to compute the angle mask
void getAngleMask(int b, int orientations, const cv::Mat& angle, cv::Mat& anglemask) {
    int order = orientations - 1;
    double const_val = pow(2.0, 2 * order) * (tgamma(order + 1) * tgamma(order + 1)) / (orientations * tgamma(2 * order + 1));

    anglemask = cv::Mat::zeros(angle.size(), CV_32F);
    
    

    for (int i = 0; i < angle.rows; ++i) {
        for (int j = 0; j < angle.cols; ++j) {
            double angle_val = angle.at<double>(i, j);
            angle_val = fmod(CV_PI + angle_val - CV_PI * (b - 1) / orientations, 2 * CV_PI) - CV_PI;
            anglemask.at<double>(i, j) = 2 * sqrt(const_val) * pow(cos(angle_val), order) * (fabs(angle_val) < CV_PI / 2);
        }
    }
}
