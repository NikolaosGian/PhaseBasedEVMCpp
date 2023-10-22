#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>
#include <string>
#include "getPolarGrid.hpp"
#include "getRadialMaskPair.hpp"
#include "getAngleMask.hpp"


/*
// Define a structure to store the filter and its properties
struct Filter {
    cv::Mat mask;
    int orientation;
};
*/



// Function to generate filters for a complex steerable pyramid
//std::vector<Filter> getFilters(const cv::Size& dimension, const std::vector<double>& rVals, int orientations, double twidth = 1.0) {
void getFilters(std::vector<Filter>& filters,const cv::Size& dimension, const std::vector<double>& rVals, int orientations, double twidth = 1.0) {
    //std::vector<Filter> filters;
    cv::Mat angle, log_rad;
    //cv::split(getPolarGrid(dimension), angle, log_rad);
    getPolarGrid(dimension,angle,log_rad);

    cv::Mat lomaskPrev;
    cv::Mat himask;
    getRadialMaskPair(rVals[0], log_rad, twidth, himask, lomaskPrev);
    
    
    //filters[counter].mask = himask;
    
    filters.push_back({himask});//himask.copyTo(filters[counter].mask);
       
  
	
    // Find the maximum element in rVals
    
    double maxRVal = rVals.size();
    cv::Mat lomask;
    cv::Mat himask1;
    cv::Mat radMask;
    cv::Mat anglemask;
    cv::Mat t;
    
    std::cout << "getFilters maxRVal = " << maxRVal << std::endl;
    for(size_t k = 1; k < maxRVal; ++k){
    	std::cout << "getFilters k = " << k << std::endl;

    	getRadialMaskPair(rVals[k], log_rad, twidth, himask1, lomask);
    	
    	cv::multiply(himask1, lomaskPrev, radMask);
    	  
    	for(int j = 0; j < orientations; ++j){
    	
    		getAngleMask(j, orientations, angle,anglemask);
    	
    		anglemask = anglemask / 2.0;
    		
    		cv::multiply(radMask,anglemask,t);
    		filters.push_back({t});//himask1.copyTo(filters[counter].mask);
    		std::cout << "after filters.push_back({t});" << std::endl;
    	}
    	lomask.copyTo(lomaskPrev);
    }
     filters.push_back({lomask});//lomask.copyTo(filters[counter].mask);
     std::cout << "after filters.push_back({lomask});" << std::endl;
    std::cout << "filters.size()" << filters.size() << std::endl;
   // return filters;
}
