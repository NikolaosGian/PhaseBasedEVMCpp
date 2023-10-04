#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <complex>
#include <vector>
using namespace std;

// Function to build a level using the DFT
cv::Mat buildLevel(const cv::Mat& im_dft1, const std::vector<cv::Mat>& croppedFilters1, const std::vector<int>&  filtIDX1, int k1) {
	
    cv::Mat level;
    
    std::cout << "im_dft1 = " << im_dft1 << std::endl;
    // Apply ifftshift to croppedFilters[k]
    cv::Mat ifftshiftedFilter;
    
    std::cout << "croppedFilters1 [" << k1 << "] = " << croppedFilters1[k1] << std::endl;
    std::cout << "ifftshiftedFilter = " << ifftshiftedFilter << std::endl;
    std::cout << "level = " << level << std::endl;
     
    cv::dft(croppedFilters1[k1], ifftshiftedFilter, cv::DFT_INVERSE);
    
    std::cout << "croppedFilters1 [" << k1 << "] = " << croppedFilters1[k1] << std::endl;
    std::cout << "ifftshiftedFilter = " << ifftshiftedFilter << std::endl;

    // Extract row and column indices from filtIDX
    int idx = filtIDX1[k1];
    int rowIdx = idx / im_dft1.cols;
    int colIdx = idx % im_dft1.cols;
    
    cout << "Idx= " << idx << endl;
    cout << "rowIdx= " << rowIdx << endl;
    cout << "colIdx= " << colIdx << endl;

    // Create a region of interest (ROI) for im_dft
    cv::Rect roi(colIdx, rowIdx, ifftshiftedFilter.cols, ifftshiftedFilter.rows);

    // Multiply ifftshiftedFilter with im_dft ROI element-wise
    cv::Mat multiplied = ifftshiftedFilter.mul(im_dft1(roi));

    // Apply ifft2 to get the level
    cv::dft(multiplied, level, cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);

    cout << "level= " << level << endl;
    return level;
}
