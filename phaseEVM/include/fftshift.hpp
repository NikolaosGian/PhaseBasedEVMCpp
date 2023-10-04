#include <opencv2/opencv.hpp>
#include <complex>

// Function to shift the zero frequency components to the center
void fftshift(cv::Mat& complexImage) {
    int cx = complexImage.cols / 2;
    int cy = complexImage.rows / 2;

    cv::Mat q0(complexImage, cv::Rect(0, 0, cx, cy));
    cv::Mat q1(complexImage, cv::Rect(cx, 0, cx, cy));
    cv::Mat q2(complexImage, cv::Rect(0, cy, cx, cy));
    cv::Mat q3(complexImage, cv::Rect(cx, cy, cx, cy));

    cv::Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
}
