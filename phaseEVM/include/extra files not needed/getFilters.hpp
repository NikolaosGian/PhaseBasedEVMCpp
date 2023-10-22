#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>

// Define a structure to store the filter and its properties
struct Filter {
    cv::Mat mask;
    int orientation;
};

// Function to generate filters for a complex steerable pyramid
std::vector<Filter> getFilters(const cv::Size& dimension, const std::vector<double>& rVals, int orientations, double twidth = 1.0) {
    std::vector<Filter> filters;
    cv::Mat angle, log_rad;
    //cv::split(getPolarGrid(dimension), angle, log_rad);
    getPolarGrid(dimension,angle,log_rad);

    cv::Mat lomaskPrev;
    for (size_t i = 0; i < rVals.size(); ++i) {
        cv::Mat himask, lomask;
        getRadialMaskPair(rVals[i], log_rad, twidth, himask, lomask);

        if (i == 0) {
            filters.push_back({himask, -1});  // -1 indicates high-pass filter
        }
	cv::Mat anglemask;
        for (int j = 0; j < orientations; ++j) {
            getAngleMask(j, orientations, angle,anglemask);
            cv::Mat radMask = himask.mul(lomaskPrev);
            filters.push_back({radMask.mul(anglemask / 2.0), j});
        }

        lomaskPrev = lomask;
    }

    filters.push_back({lomaskPrev, -1});  // -1 indicates low-pass filter

    return filters;
}
