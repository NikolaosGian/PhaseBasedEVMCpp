#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>


std::vector<cv::Mat> generateOctaveFilters(const cv::Size&, int ht, int numFilters) {
    // Create an empty matrix to store the filters
    cv::Mat filters(ht + 1, numFilters, CV_32FC1);

    for (int i = 0; i <= ht; i++) {
        float filterValue = std::pow(2.0f, -i);
        for (int j = 0; j < numFilters; j++) {
            filters.at<float>(i, j) = filterValue;
        }
    }

    return filters;
}

cv::Mat generateHalfOctaveFilters(int h, int w, int ht, int numFilters, float twidth) {
    // Create an empty matrix to store the filters
    cv::Mat filters(ht * 2 + 1, numFilters, CV_32FC1);

    for (int i = 0; i <= ht * 2; i++) {
        float filterValue = std::pow(2.0f, -i * 0.5f);
        for (int j = 0; j < numFilters; j++) {
            filters.at<float>(i, j) = filterValue;
        }
    }

    // Apply twidth to the filters
    filters *= twidth;

    return filters;
}


cv::Mat generateSmoothHalfOctaveFilters(int h, int w, int filtersPerOctave) {
    // Calculate the number of octaves based on the image dimensions and filters per octave
    int numOctaves = static_cast<int>(std::log2(std::min(h, w))) - 2;
    
    // Calculate the total number of scales based on octaves and filters per octave
    int numScales = numOctaves * filtersPerOctave;

    // Create an empty matrix to store the filters
    cv::Mat filters(numScales, 1, CV_32FC1);

    for (int i = 0; i < numScales; i++) {
        float filterValue = std::pow(2.0f, i / static_cast<float>(filtersPerOctave));
        filters.at<float>(i, 0) = filterValue;
    }

    return filters;
}

cv::Mat generateQuarterOctaveFilters(int h, int w, int filtersPerOctave) {
    // Calculate the number of octaves based on the image dimensions and filters per octave
    int numOctaves = static_cast<int>(std::log2(std::min(h, w))) - 2;
    
    // Calculate the total number of scales based on octaves and filters per octave
    int numScales = numOctaves * filtersPerOctave;

    // Create an empty matrix to store the filters
    cv::Mat filters(numScales, 1, CV_32FC1);

    for (int i = 0; i < numScales; i++) {
        float filterValue = std::pow(2.0f, i / static_cast<float>(filtersPerOctave));
        filters.at<float>(i, 0) = filterValue;
    }

    return filters;
}

cv::Mat getFilterIDX(const cv::Mat& filters, std::vector<int>& filtIDX) {
    int numFilters = filters.rows;
    int cropSize = numFilters / 2; // Crop size (adjust as needed)

    // Initialize the filter indices vector
    filtIDX.resize(numFilters);

    // Populate filter indices (0 to numFilters-1)
    for (int i = 0; i < numFilters; i++) {
        filtIDX[i] = i;
    }

    // Crop the filters based on filter indices
    cv::Mat croppedFilters = filters.rowRange(0, cropSize).clone(); // Clone the cropped part

    // Optionally, you can also create croppedFilters as a separate matrix instead of cloning
    return croppedFilters;
}
