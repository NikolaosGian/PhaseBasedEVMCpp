#include <opencv2/opencv.hpp>
#include <cmath>

void butterLow(int num, float fl, cv::Mat& low_a, cv::Mat& low_b, bool bValue){
	double wc = 2.0 * CV_PI * fl;  // Cutoff frequency in radians

    	// Compute the Butterworth filter coefficients for lowpass
    	low_a.create(1, num + 1, CV_64F);
    	low_b.create(1, num + 1, CV_64F);

    	for (int k = 0; k <= num; k++) {
        	double theta = (k % 2 == 0) ? 0.0 : CV_PI / 2.0;
        	double alpha = sin((2.0 * k + 1.0) * CV_PI / (4.0 * (num + 1.0)));
        	double beta = cos(theta) / (1.0 + alpha);

        	low_a.at<double>(k) = (1.0 - beta) / 2.0;
        	low_b.at<double>(k) = cos(wc * (k - num / 2.0)) * (1.0 - beta);
    	}
}

void butterHigh(int num, float fh, cv::Mat& high_a, cv::Mat& high_b, bool bValue){
	double wc = 2.0 * CV_PI * fh;  // Cutoff frequency in radians

    	// Compute the Butterworth filter coefficients for highpass
    	high_a.create(1, num + 1, CV_64F);
    	high_b.create(1, num + 1, CV_64F);

    	for (int k = 0; k <= num; k++) {
        	double theta = (k % 2 == 0) ? 0.0 : CV_PI / 2.0;
        	double alpha = sin((2.0 * k + 1.0) * CV_PI / (4.0 * (num + 1.0)));
        	double beta = cos(theta) / (1.0 + alpha);

        	high_a.at<double>(k) = (1.0 + beta) / 2.0;
        	high_b.at<double>(k) = -cos(wc * (k - num / 2.0)) * (1.0 + beta);
    	}
}


cv::Mat differenceOfButterworths(const cv::Mat& delta, float fl, float fh) {
    int timeDimension = 2; // 0-based index for time dimension
    int len = delta.cols;

    // Design Butterworth filters for lowpass and highpass
    cv::Mat lowpass1 = delta.row(0).clone();
    cv::Mat lowpass2 = lowpass1.clone();
    cv::Mat prev = lowpass1.clone();

    // Initialize the first frame of delta as zeros
    cv::Mat result = delta.clone();
    result.row(0).setTo(0);

    // Design lowpass and highpass Butterworth filters
    cv::Mat low_a, low_b, high_a, high_b;
    butterLow(1, fl, low_a, low_b, false);	// need to fix
    butterHigh(1, fh, high_a, high_b, false);	// need to fix

    for (int i = 1; i < len; i++) {
        // Update lowpass1 and lowpass2 using the highpass and lowpass filters
        lowpass1 = (-high_b.at<double>(1) * lowpass1 + high_a.at<double>(0) * delta.row(i) + high_a.at<double>(1) * prev) / high_b.at<double>(0);
        lowpass2 = (-low_b.at<double>(1) * lowpass2 + low_a.at<double>(0) * delta.row(i) + low_a.at<double>(1) * prev) / low_b.at<double>(0);
        
        prev = delta.row(i);

        // Compute the difference and store it in the corresponding frame of result
        result.row(i) = lowpass1 - lowpass2;
    }

    return result;
}


