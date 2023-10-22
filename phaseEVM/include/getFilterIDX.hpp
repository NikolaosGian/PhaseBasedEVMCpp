#include <opencv2/opencv.hpp>
#include <vector>
#include "getIDXFromFilter.hpp"

/*
// Define a structure to store the filter
struct Filter {
    cv::Mat mask;
    int orientation;
};
*/


std::ostream& operator<<(std::ostream& os, const FilterIndices& indices) {
    os << "dim1: ";
    for (int value : indices.dim1) {
        os << value << " ";
    }
    os << "dim2: ";
    for (int value : indices.dim2) {
        os << value << " ";
    }
    return os;
}

void getFilterIDX(const std::vector<cv::Mat>& filters, std::vector<FilterIndices>& filtIDX , std::vector<cv::Mat>& croppedFilters) {

    int nFilts = filters.size();
    filtIDX.resize(nFilts);
    croppedFilters.resize(nFilts);
    
    

    for (int k = 0; k < nFilts; ++k) {
        FilterIndices indices = getIDXFromFilter(filters[k]);
        //std::cout << "indices.dim1[0] = " << indices.dim1[0] << std::endl;
        //std::cout << "indices.dim2[0] = " << indices.dim2[0] << std::endl;
        //std::cout << "indices.dim1[1] = " << indices.dim1[1] << std::endl;
        //std::cout << "indices.dim2[1] = " << indices.dim2[1] << std::endl;
        
        filtIDX[k].dim1 = indices.dim1;
    	filtIDX[k].dim2 = indices.dim2;
        //filtIDX[k] = indices;
        //filtIDX[k].dim1[0]= indices.dim1[0];
        //filtIDX[k].dim1[1]= indices.dim1[1];
        
        //filtIDX[k].dim2[0]= indices.dim2[0];
        //filtIDX[k].dim2[1]= indices.dim2[1];
        //filtIDX[k].dim2= indices.dim2;
       // std::cout << k<< " filtIDX[k] = " << filtIDX[k] << std::endl;
        croppedFilters[k] = filters[k](cv::Rect(indices.dim2[0], indices.dim1[0], indices.dim2[1] - indices.dim2[0] + 1, indices.dim1[1] - indices.dim1[0] + 1));
        
        
    }
/*
    // Convert FilterIndices to a vector of vectors for compatibility
    std::vector<std::vector<int>> filtIDXVec(nFilts);
    for (int k = 0; k < nFilts; ++k) {
        filtIDXVec[k] = {filtIDX[k].dim1[0], filtIDX[k].dim1[1], filtIDX[k].dim2[0], filtIDX[k].dim2[1]};
    }
*/

}


   /*
  
// Function to get non-zero indices and cropped filters
void getFilterIDX(const std::vector<Filter>& filters, std::vector<std::vector<cv::Point>>& filtIDX, std::vector<cv::Mat>& croppedFilters) {


    
    
 
    int nFilts = filters.size();
    filtIDX.resize(nFilts);
    croppedFilters.resize(nFilts);
    
   cv::Mat nonZeroIdx;
   std::vector<cv::Point> indices;
   std::cout << "nFilts = " <<  nFilts << std::endl;
    for (int k = 0; k < nFilts; ++k) {
        // Find non-zero indices using cv::findNonZero
        
        cv::findNonZero(filters[k].mask, nonZeroIdx); /// error here!
        

        // Extract the points from the non-zero indices
        
        for (int i = 0; i < nonZeroIdx.total(); ++i) {
            indices.push_back(nonZeroIdx.at<cv::Point>(i));
            
        }
	
        filtIDX[k] = indices;
       
        croppedFilters[k] = filters[k].mask.clone();  // Clone the mask
        
        croppedFilters[k] = cv::Scalar(0);           // Set all elements to 0
         
        for (const cv::Point& idx : indices) {
            croppedFilters[k].at<double>(idx) = filters[k].mask.at<double>(idx);
            
        }
        std::cout << "again at loop 1  " << std::endl;
    }
    
}
*/
