#include <iostream>
#include <opencv2/opencv.hpp>


cv::Mat AmplitudeWeightedBlur(const cv::Mat& in, const cv::Mat& weight, float sigma) {
    cv::Mat out;

    if (sigma != 0) {
        int kernelSize = cvRound(4 * sigma);
        std::cout <<"after int kernelSize = cvRound(4 * sigma);"<<std::endl; 
        
        cv::Size kernelSize2D(kernelSize, kernelSize);
        std::cout <<"after cv::Size kernelSize2D(kernelSize, kernelSize);"<<std::endl;
        
        cv::Mat kernel = cv::getGaussianKernel(kernelSize, sigma);
        std::cout <<"after cv::Mat kernel = cv::getGaussianKernel(kernelSize, sigma);"<<std::endl;
        
        cv::Mat weightMat = weight + cv::Scalar::all(std::numeric_limits<float>::epsilon());
        std::cout <<"after  cv::Mat weightMat = weight + cv::Scalar::all(std::numeric_limits<float>::epsilon());"<<std::endl;

        // Filter the input image and weight matrix using the Gaussian kernel
        cv::Mat filteredIn, filteredWeight;
        
        std::string ty =  type2str( in.type() );
	printf(" in Matrix: %s cols_%dxrows_%d \n", ty.c_str(), in.cols, in.rows );
	
	ty =  type2str( weight.type() );
	printf(" weight Matrix: %s cols_%dxrows_%d \n", ty.c_str(), weight.cols, weight.rows );
	
	ty =  type2str( weightMat.type() );
	printf(" weightMat Matrix: %s cols_%dxrows_%d \n", ty.c_str(), weightMat.cols, weightMat.rows );
	
	cv::Mat temp(weightMat.size(),weightMat.type());
	
	temp.at<float>(0, 0) = in.at<float>(0, 0);
	temp.at<float>(0, 1) = in.at<float>(0, 1);
	temp.at<float>(1, 0) = 0.0;
	temp.at<float>(1, 1) = 0.0;
	    	
        cv::filter2D(temp.mul(weightMat), filteredIn, -1, kernel, cv::Point(-1, -1), 0, cv::BORDER_CONSTANT);//cv::filter2D(in.mul(weight), filteredIn, -1, kernel, cv::Point(-1, -1), 0, cv::BORDER_CONSTANT);
        std::cout <<"after   cv::filter2D(in.mul(weight), filteredIn, -1, kernel, cv::Point(-1, -1), 0, cv::BORDER_CONSTANT);"<<std::endl;
        cv::filter2D(weightMat, filteredWeight, -1, kernel, cv::Point(-1, -1), 0, cv::BORDER_CONSTANT);//cv::filter2D(weight, filteredWeight, -1, kernel, cv::Point(-1, -1), 0, cv::BORDER_CONSTANT);
        std::cout <<"after    cv::filter2D(weight, filteredWeight, -1, kernel, cv::Point(-1, -1), 0, cv::BORDER_CONSTANT);"<<std::endl;

        // Divide the filtered input by the filtered weight to obtain the result
        out = filteredIn / filteredWeight;
        std::cout <<"after    out = filteredIn / filteredWeight;"<<std::endl;
    } else {
        // If sigma is zero, return the input image as is
        out = in;
        std::cout <<"after   out = in;"<<std::endl;
    }

    return out;
}
