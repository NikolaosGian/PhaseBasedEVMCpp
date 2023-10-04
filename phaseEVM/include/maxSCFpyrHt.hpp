/* MAXHEIGHT = maxSCFpyrHt(IM)
 
 Specifies the maximum number of octaves that can be in a steerable
 pyramid of image IM.

 Based on buildSCFpyr in matlabPyrTools
*/
#include <iostream>
#include <opencv2/opencv.hpp> // Include OpenCV for image size calculation


int maxSCFpyrHt(const cv::Mat& im ){
	int minDimension = std::min(im.rows, im.cols); // Minimum image dimension
	int maxHeight = static_cast<int>(std::floor(std::log2(minDimension))) - 2;
	
	return maxHeight;
}
