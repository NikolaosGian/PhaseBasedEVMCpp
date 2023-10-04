#include <iostream>
#include <opencv2/opencv.hpp>
#include <complex>
#include "fftshift.hpp"

// Function to reconstruct a level using FFT
cv::Mat reconLevel(const cv::Mat& im_dft, const cv::Mat& croppedFilter) {
    cv::Mat recon;

    // Apply fft2 to im_dft
    cv::dft(im_dft, recon, cv::DFT_COMPLEX_OUTPUT);

    // Apply fftshift
    fftshift(recon);

    // Multiply by croppedFilter
    cv::Mat multiplied = croppedFilter.mul(recon);

    // Multiply by 2
    cv::multiply(multiplied, 2.0, recon);

    return recon.clone();
}
