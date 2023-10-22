#include <iostream>
#include <opencv2/opencv.hpp>
#include <complex>
#include "fftshift.hpp"

// Function to reconstruct a level using the DFT
cv::Mat reconLevel(const cv::Mat& im_dft, const std::vector<cv::Mat>& croppedFilters, int k) {

	//cv::Mat copyOfCroppedFilter = croppedFilters[k].clone();
	//cv::resize(copyOfCroppedFilter, copyOfCroppedFilter, im_dft.size());
	
    	//cv::Mat fftshiftedFilter = copyOfCroppedFilter.mul(fftshift(cv::dft(im_dft,im_dft,cv::DFT_INVERSE)));
    	
    	cv::Mat dest;
    	cv::dft(im_dft,dest,cv::DFT_INVERSE);
    	fftshift(dest);
    	cv::Mat fftshiftedFilter = croppedFilters[k].mul(dest);
    	return 2 * fftshiftedFilter;
}
