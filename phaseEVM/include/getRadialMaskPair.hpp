#include <opencv2/opencv.hpp>
#include <cmath>

// Function to compute radial masks HIMASK and LOMASK
void getRadialMaskPair(float r, const cv::Mat& rad, float twidth, cv::Mat& himask, cv::Mat& lomask) {    
    cv::Mat log_rad = cv::Mat::zeros(rad.size(), CV_32F);

    // Compute log_rad
    cv::Mat radTemp;
    cv::log(rad, radTemp);
    cv::Mat rTemp;
    cv::log(r,rTemp);
    
    cv::subtract(radTemp,rTemp,log_rad); 

    
    
   
    // Calculate HIMASK
    cv::Mat tmp_himask;
    log_rad.copyTo(tmp_himask);
    cv::threshold(tmp_himask, tmp_himask, -twidth, 0, cv::THRESH_TRUNC);
    
   
    
	
    tmp_himask *= CV_PI / (2 * twidth);
	
     
    // Calculate HIMASK using forEach
    tmp_himask.forEach<float>(
        [&himask](float& pixel, const int* position) -> void {
            pixel = std::abs(pixel); // Ensure positive values
            pixel = std::cos(pixel);
        }
    );

    tmp_himask.copyTo(himask);
    
     
    // Calculate LOMASK
    cv::Mat mulTemp;
    //cv::multiply(himask,himask,mulTemp);
    cv::multiply(tmp_himask,tmp_himask,mulTemp);
    cv::Mat  temp = 1 - mulTemp;
    cv::sqrt(temp, lomask);  
    
}
