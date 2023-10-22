#include <opencv2/opencv.hpp>
#include <vector>

/*
// Define a structure for the filter indices in both dimensions
struct FilterIndices {
    std::vector<int> dim1;
    std::vector<int> dim2;
};
*/
FilterIndices getIDXFromFilter(const cv::Mat& filter) {
    
    
    FilterIndices filtIDX;
    cv::Mat aboveZero;
    cv::compare(filter, 1e-10, aboveZero, cv::CMP_GT);
     
    
    cv::Mat dim1 = cv::Mat::zeros(1, filter.rows, CV_8U);
    cv::Mat dim2 = cv::Mat::zeros(1, filter.cols, CV_8U);
    
    for (int i = 0; i < filter.rows; ++i) {
        cv::Scalar sumRow = cv::sum(aboveZero.row(i));
        if (sumRow[0] > 0) {
            dim1.at<uchar>(0, i) = 1;
        }
    }

    for (int i = 0; i < filter.cols; ++i) {
        cv::Scalar sumCol = cv::sum(aboveZero.col(i));
        if (sumCol[0] > 0) {
            dim2.at<uchar>(0, i) = 1;
        }
    }
    
     dim1 = dim1 | dim1.clone().at<uchar>(0, 0);//dim1 = dim1 | dim1; //dim1 = dim1 | dim1.clone().at<uchar>(0, 0);
     dim2 = dim2 | dim2.clone().at<uchar>(0, 0);//dim2 = dim2 | dim2; //dim2 = dim2 | dim2.clone().at<uchar>(0, 0);
    
    /*
      // Print the content of dim1
    for (int i = 0; i < dim1.cols; ++i) {
        std::cout << "dim1(0, " << i << "): " << static_cast<int>(dim1.at<uchar>(0, i)) << std::endl;
    }

    // Print the content of dim2
    for (int i = 0; i < dim2.cols; ++i) {
        std::cout << "dim2(0, " << i << "): " << static_cast<int>(dim2.at<uchar>(0, i)) << std::endl;
    }
    */
    
    std::vector<int> idx1, idx2;
     for (int i = 0; i < filter.rows; ++i) {
        if (dim1.at<uchar>(0, i) == 1) {
            idx1.push_back(i);
        }
    }

    for (int i = 0; i < filter.cols; ++i) {
        if (dim2.at<uchar>(0, i) == 1) {
            idx2.push_back(i);
        }
    }
    filtIDX.dim1 = idx1;
    filtIDX.dim2 = idx2;	
/*
    cv::Mat idx1(1, filter.rows, CV_32S);
    for (int i = 0; i < filter.rows; ++i) {
        idx1.at<int>(0, i) = i;
    }

    cv::Mat idx2(1, filter.cols, CV_32S);
    for (int i = 0; i < filter.cols; ++i) {
        idx2.at<int>(0, i) = i;
    }
    */
	/*
	  std::cout << "befoar idx1 = idx1(cv::Rect(0, 0, filter.cols, filter.rows)); "  << std::endl;
    idx1 = idx1(cv::Rect(0, 0, filter.cols, filter.rows));
      std::cout << "after idx1 = idx1(cv::Rect(0, 0, filter.cols, filter.rows)); "  << std::endl;
    idx2 = idx2(cv::Rect(0, 0, filter.cols, filter.rows));
    std::cout << "after idx2 = idx2(cv::Rect(0, 0, filter.cols, filter.rows)); "  << std::endl;

    
    //idx1 = idx1(dim1);
   // idx2 = idx2(dim2);
   
   
   
    idx1.setTo(cv::Scalar(0), dim1 == 0);
    idx2.setTo(cv::Scalar(0), dim2 == 0);
    
     // Print the content of idx1
    for (int i = 0; i < idx1.cols; ++i) {
        std::cout << "idx1(0, " << i << "): " << idx1.at<int>(0, i) << std::endl;
    }

    // Print the content of idx2
    for (int i = 0; i < idx2.cols; ++i) {
        std::cout << "idx2(0, " << i << "): " << idx2.at<int>(0, i) << std::endl;
    }
    

    FilterIndices indices;
    cv::Point minLoc, maxLoc;

    cv::minMaxLoc(idx1, nullptr, nullptr, &minLoc, &maxLoc);
    indices.dim1 = { minLoc.x, maxLoc.x };
    std::cout << "indices.dim1: ";
    for (int i = 0; i < indices.dim1.size(); ++i) {
    	std::cout << indices.dim1[i] << " ";
	}
    std::cout << std::endl;

    cv::minMaxLoc(idx2, nullptr, nullptr, &minLoc, &maxLoc);
    indices.dim2 = { minLoc.x, maxLoc.x };
    std::cout << "indices.dim2: ";
    for (int i = 0; i < indices.dim1.size(); ++i) {
    	std::cout << indices.dim2[i] << " ";
    }
    std::cout << std::endl;
    */
    
    

    return filtIDX;
}
