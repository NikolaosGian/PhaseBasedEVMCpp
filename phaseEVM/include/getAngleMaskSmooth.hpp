#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>


int factorial(int n)
{
  return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}


cv::Mat getAngleMaskSmooth(int b, int nbands, const cv::Mat& angle, bool complexFilt) {
    	int order = nbands - 1;
    	double constVal = std::pow(2, 2 * order) * std::pow(factorial(order), 2) / (nbands * factorial(2 * order));

    	cv::Mat anglemask;
    	cv::Mat shiftedAngle = angle + CV_PI - (CV_PI * (b - 1) / nbands);
    	//shiftedAngle -= 2 * CV_PI * std::floor((shiftedAngle + CV_PI) / (2 * CV_PI)) - CV_PI;
	
	// Convert shiftedAngle to double type for floor operation
    	cv::Mat shiftedAngleDouble;
    	shiftedAngle.convertTo(shiftedAngleDouble, CV_64F);
	
	// Apply floor
    	cv::Mat floorResult = 2.0 * CV_PI * (shiftedAngleDouble + CV_PI) / (2.0 * CV_PI);
    	cv::Mat floorShiftedAngle;
    	floorResult.convertTo(floorShiftedAngle, CV_64F);
    	floorShiftedAngle -= CV_PI;
	
	cv::Mat absShiftedAngle;
    	cv::absdiff(shiftedAngle, CV_PI / 2, absShiftedAngle);
    
    	if (complexFilt) {
    		
    		cv::Mat cosfloorShiftedAngle(floorShiftedAngle.size(), floorShiftedAngle.type());
        	for (int i = 0; i < floorShiftedAngle.rows; i++) {
        		for (int j = 0; j < floorShiftedAngle.cols; j++) {
            			cosfloorShiftedAngle.at<double>(i, j) = cos(floorShiftedAngle.at<double>(i, j));
        		}
    		}	
    		
    		cv::Mat temp;
    		cv::pow(absShiftedAngle < CV_PI /2, order,temp);
    		
    		anglemask = std::sqrt(constVal) * cosfloorShiftedAngle.mul(temp);

    		//anglemask = std::sqrt(constVal) * std::cos(shiftedAngle) * std::pow(std::abs(shiftedAngle) < CV_PI / 2, order);

       // anglemask = std::sqrt(constVal) * cv::cos(shiftedAngle).pow(order) * (cv::abs(shiftedAngle) < CV_PI / 2);
    	} else {
    	
    	
    		cv::Mat cosfloorShiftedAngle(floorShiftedAngle.size(), floorShiftedAngle.type());
        	for (int i = 0; i < floorShiftedAngle.rows; i++) {
        		for (int j = 0; j < floorShiftedAngle.cols; j++) {
            			cosfloorShiftedAngle.at<double>(i, j) = cos(floorShiftedAngle.at<double>(i, j));
        		}
    		}
       // anglemask = cv::abs(std::sqrt(constVal) * cv::cos(shiftedAngle).pow(order));
        	//anglemask = std::abs(std::sqrt(constVal) * std::cos(shiftedAngle) * std::pow(std::abs(shiftedAngle) < CV_PI / 2, order));
        	cv::Mat temp;
        	cv::pow(absShiftedAngle < CV_PI /2, order,temp);
	        anglemask = cv::abs(std::sqrt(constVal) * floorShiftedAngle.mul(temp));

    	}

    	return anglemask;
}
