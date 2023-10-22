#include <opencv2/opencv.hpp>
#include <cmath>

// Function to compute radial masks HIMASK and LOMASK
void getRadialMaskPair(float r, const cv::Mat& rad, float twidth, cv::Mat& himask, cv::Mat& lomask) {
    cv::Mat log_rad = cv::Mat::zeros(rad.size(), CV_32F);
    cv::Mat tmp_himask;

    log_rad = cv::log(rad) - cv::log(r);

    tmp_himask = log_rad.clone();
    cv::threshold(tmp_himask, tmp_himask, -twidth, 0, cv::THRESH_TRUNC);
    tmp_himask *= CV_PI / (2 * twidth);
    cv::absdiff(tmp_himask, cv::Scalar(0), tmp_himask);
    cv::cos(tmp_himask, himask);

    cv::sqrt(1 - himask.mul(himask), lomask);
}
