#include <iostream>
#include <opencv2/opencv.hpp>

cv::Mat differenceOfIIR(const cv::Mat& delta, float rl, float rh) {
    int timeDimension = 2; // 0-based index for time dimension
    int len = delta.cols;

    // Create lowpass1 and lowpass2 matrices
    cv::Mat lowpass1 = delta.row(0).clone();
    cv::Mat lowpass2 = lowpass1.clone();

    // Initialize the first frame of delta as zeros
    cv::Mat result = delta.clone();
    result.row(0).setTo(0);

    for (int i = 1; i < len; i++) {
        // Update lowpass1 and lowpass2
        lowpass1 = (1 - rh) * lowpass1 + rh * delta.row(i);
        lowpass2 = (1 - rl) * lowpass2 + rl * delta.row(i);

        // Compute the difference and store it in the result
        result.row(i) = lowpass1 - lowpass2;
    }

    return result;
}
