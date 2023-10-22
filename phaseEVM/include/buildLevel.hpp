#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <complex>
#include <vector>
#include <string>


// Function to build a level using the DFT
cv::Mat buildLevel(const cv::Mat& im_dft, const std::vector<cv::Mat>& croppedFilters, const std::vector<FilterIndices>& filtIDX, int k) {
//cv::Mat buildLevel(const cv::Mat& im_dft, const cv::Mat& croppedFilter, const FilterIndices& filtIDX){

    cv::Mat im_dft_roi;
    cv::Mat ifftshiftedFilter;

    cv::Rect rect(filtIDX[k].dim1[0], filtIDX[k].dim2[0], filtIDX[k].dim1[1] - filtIDX[k].dim1[0] + 1, filtIDX[k].dim2[1] - filtIDX[k].dim2[0] + 1);
   
    
    im_dft_roi = im_dft(rect);

    // Perform element-wise multiplication of croppedFilter and im_dft based on filtIdx
    //cv::Mat multiplied;
    //cv::multiply(croppedFilters[k],im_dft_roi, multiplied);
    
    cv::Mat croppedFiltersFloat(croppedFilters[k].rows, croppedFilters[k].cols, CV_32FC2);; // This will store the converted matrix of type CV_32FC2
    // Assign real values from croppedFilters[1] to the real component of croppedFiltersFloat
    for (int i = 0; i < croppedFiltersFloat.rows; i++) {
    	for (int j = 0; j < croppedFiltersFloat.cols; j++) {
        	croppedFiltersFloat.at<cv::Vec2f>(i, j)[0] = croppedFilters[1].at<float>(i, j);
        	croppedFiltersFloat.at<cv::Vec2f>(i, j)[1] = 0.0;  // Imaginary component is set to 0
    }
}
       
    cv::Mat multiplied = croppedFiltersFloat.mul(im_dft_roi);
    
    //cv::Mat multiplied = croppedFilters[k].mul(im_dft_roi);
    //cv::Mat multiplied = croppedFilters[k].mul(im_dft_roi_resized);
    // Perform inverse 2D inverse Fourier transform
    cv::dft(multiplied, ifftshiftedFilter, cv::DFT_INVERSE);

    return ifftshiftedFilter;
}
  /*
    cv::Mat ifftshiftedFilter;
    cv::Mat croppedFiltersFloat;
    
   
   
    //croppedFilters[k].convertTo(croppedFiltersFloat, CV_32F);
    croppedFiltersFloat.create(croppedFilters[k].size(), CV_32F);
    ifftshiftedFilter.create(croppedFilters[k].size(), CV_32F);
    
    for (int row = 0; row < croppedFilters[k].rows; ++row) {
    	for (int col = 0; col < croppedFilters[k].cols; ++col) {
        	float value = static_cast<float>(croppedFilters[k].at<uchar>(row, col));
        	croppedFiltersFloat.at<float>(row, col) = value;
        	std::cout << "value" << value << std::endl;
   	 }
     }
    
     std::cout << "ifftshiftedFilter Matrix dimensions: " << ifftshiftedFilter.rows << " rows x " << ifftshiftedFilter.cols << " columns" << std::endl;
     std::cout << "croppedFiltersFloat Matrix dimensions: " << croppedFiltersFloat.rows << " rows x " << croppedFiltersFloat.cols << " columns" << std::endl;
    
    cv::dft(croppedFiltersFloat, ifftshiftedFilter, cv::DFT_INVERSE);//cv::dft(croppedFilters[k], ifftshiftedFilter, cv::DFT_INVERSE);
    
    std::cout << "after cv::dft(croppedFiltersFloat, ifftshiftedFilter, cv::DFT_INVERSE); " <<std::endl;
    
    for (int i = 0; i < filtIDX[k].dim1.size(); ++i) {
    	int value = filtIDX[k].dim1[i];
    	std::cout << "dim1[" << i << "] = " << value << std::endl;
	}

	
    int start_x = filtIDX[k].dim1[0];//cv::Point2i start(filtIDX[k].dim1[0], filtIDX[k].dim2[0]); //cv::Point2i start = filtIDX[k][0];
    std::cout << "start_x = " << start_x <<std::endl;
    int start_y = filtIDX[k].dim2[0];
    
        std::cout << "after start_y = filtIDX[k].dim2[0];" <<std::endl;
    int end_x = filtIDX[k].dim1[1];//cv::Point2i end(filtIDX[k].dim1[1], filtIDX[k].dim2[1]); //cv::Point2i end = filtIDX[k][1];
    int end_y = filtIDX[k].dim2[1];
     std::cout << "after cv::Point2i end(filtIDX[k].dim1[1], filtIDX[k].dim2[1]);" <<std::endl;
    cv::Rect roi(start_x, start_y, end_x - start_x + 1, end_y - start_y + 1);//cv::Rect roi(start.x, start.y, end.x - start.x + 1, end.y - start.y + 1);
	
	
     std::cout << "after cv::Rect roi(start.x, start.y, end.x - start.x + 1, end.y - start.y + 1);" <<std::endl;
     
     
    cv::Mat multiplied = ifftshiftedFilter.mul(im_dft(roi));
    std::cout << "after cv::Mat multiplied = ifftshiftedFilter.mul(im_dft(roi));" <<std::endl;

    cv::Mat level;
    cv::dft(multiplied, level, cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);
    
     std::cout << "after cv::dft(multiplied, level, cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);" <<std::endl;
     
    return level;
}
*/

