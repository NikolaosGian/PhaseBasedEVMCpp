//#include "getPolarGrid.hpp"
//#include "getRadialMaskPair.hpp"
//#include "getAngleMask.hpp"
#include "getAngleMaskSmooth.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>

/*
// Define a structure to store the filter and its properties
struct Filter {
    cv::Mat mask;
    int orientation;
};

int factorial(int n)
{
  return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}
*/



const double pi = M_PI;

void getFiltersSmoothWindow(std::vector<cv::Mat>& filters,const cv::Size& dims, int orientations, int cosOrder, int filtersPerOctave, bool complexFilt = true, int height = 0) {
    //std::vector<Filter> filters;
    cv::Mat angle, rad;

    // Call the getPolarGrid function to obtain angle and rad matrices
    getPolarGrid(dims,angle,rad);
    

  

    // Compute Radial Filters first in 1D
    int htOct = height;
    //rad = cv::log2(rad);
    
    cv::Mat logRAD;
    cv::log(rad,logRAD);
    cv::Mat divRes;
    //divRes = lograd/log(2);
    cv::divide(logRAD, std::log(2), divRes);
    
    
    rad = (htOct + rad) / htOct;
    int filts = filtersPerOctave * htOct;
    rad = rad * (pi / 2 + pi / 7 * filts);

    // Define a windowing function
    auto windowFnc = [](cv::Mat x, double center) -> cv::Mat {
        return cv::abs(x - center) < pi / 2;
    };

    std::vector<cv::Mat> radFilters;
    cv::Mat total(dims, CV_64F, cv::Scalar(0));
    double constVal = std::pow(2, 2 * cosOrder) * std::pow(factorial(cosOrder), 2) / ((cosOrder + 1) * factorial(2 * cosOrder));



    for (int k = filts; k >= 1; --k) {
        double shift = pi / (cosOrder + 1) * k + 2 * pi / 7;
        
        cv::Mat shifted_rad = rad - shift;
        cv::Mat cos_rad(shifted_rad.size(), shifted_rad.type());
        for (int i = 0; i < shifted_rad.rows; i++) {
        	for (int j = 0; j < shifted_rad.cols; j++) {
            		cos_rad.at<double>(i, j) = cos(shifted_rad.at<double>(i, j));
        	}
    	}
        /*
        cv::Mat filter = std::sqrt(constVal) * cv::pow(cv::cos(rad - shift), cosOrder) * windowFnc(rad, shift);
        radFilters.push_back(filter);
        total += filter.mul(filter);
        */
        cv::Mat filter;
    	cv::sqrt(1 - cos_rad.mul(cos_rad), filter);
    	filter = filter.mul(windowFnc(rad, shift));
    	filter = filter * std::sqrt(constVal);
    	radFilters.push_back(filter);
    	total += filter.mul(filter);
    }
    
    

    	// Compute lopass residual
    	cv::Point center(dims.width / 2, dims.height / 2);
    	cv::Size lodims((center.x + 1) / 4, (center.y + 1) / 4);
    	//cv::Mat totalCrop = total(cv::Rect(center - lodims, center + lodims));
    
    	// Calculate top-left and bottom-right points of the rectangle
	cv::Point tl = center - cv::Point(lodims.width / 2, lodims.height / 2);
	cv::Point br = center + cv::Point(lodims.width / 2, lodims.height / 2);

	// Create the rectangle
	cv::Rect roi(tl, br);
	cv::Mat totalCrop = total(roi);
	
    	//cv::sqrt(1 - totalCrop, lopass(cv::Rect(center - lodims, center + lodims)));
    	
	// Calculate lopass as before
	cv::Mat lopass(dims, CV_64F, cv::Scalar(0));
	cv::sqrt(1 - totalCrop, lopass(roi));
    	
    	

    	// Compute high pass residual
    	cv::Mat tempRes;
    	total += lopass.mul(lopass);
    	cv::sqrt(1 - total,tempRes);
    	cv::Mat hipass = cv::abs(tempRes); //error here

    	// If either dimension is even, this fixes some errors
    	if (dims.width % 2 == 0) {
        	for (cv::Mat& filter : radFilters) {
            	filter.at<double>(0, 0) = 0;
        	}
        	hipass.at<double>(0, 0) = 1;
        	lopass.at<double>(0, 0) = 0;
    	}
    	if (dims.height % 2 == 0) {
        	for (cv::Mat& filter : radFilters) {
            	filter.at<double>(0, 0) = 0;
        	}
        	hipass.at<double>(0, 0) = 1;
        	lopass.at<double>(0, 0) = 0;
    	}

    	std::vector<cv::Mat> anglemasks;
    	for (int k = 0; k < orientations; ++k) {
        	anglemasks.push_back(getAngleMaskSmooth(k, orientations, angle, complexFilt));
    	}

    	// Create and populate filters vector
    	filters.push_back({ hipass}); // -1 indicates high-pass filter

    	for (int k = 0; k < filts; ++k) {
        	for (int j = 0; j < orientations; ++j) {
           	 cv::Mat filter = anglemasks[j].mul(radFilters[k]);
            	filters.push_back({filter});
        	}
    	}

    	filters.push_back({ lopass}); // -1 indicates low-pass filter
}
