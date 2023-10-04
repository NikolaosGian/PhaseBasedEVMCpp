#include <iostream>
#include <opencv2/opencv.hpp>


cv::Mat AmplitudeWeightedBlur(const cv::Mat& in, const cv::Mat& weight, float sigma) {
    cv::Mat out;

    if (sigma != 0) {
        int kernelSize = cvRound(4 * sigma);
        cv::Size kernelSize2D(kernelSize, kernelSize);
        cv::Mat kernel = cv::getGaussianKernel(kernelSize, sigma);
        cv::Mat weightMat = weight + cv::Scalar::all(std::numeric_limits<float>::epsilon());

        // Filter the input image and weight matrix using the Gaussian kernel
        cv::Mat filteredIn, filteredWeight;
        cv::filter2D(in.mul(weight), filteredIn, -1, kernel, cv::Point(-1, -1), 0, cv::BORDER_CONSTANT);
        cv::filter2D(weight, filteredWeight, -1, kernel, cv::Point(-1, -1), 0, cv::BORDER_CONSTANT);

        // Divide the filtered input by the filtered weight to obtain the result
        out = filteredIn / filteredWeight;
    } else {
        // If sigma is zero, return the input image as is
        out = in;
    }

    return out;
}
