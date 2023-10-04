#include <iostream>
#include <opencv2/opencv.hpp>


cv::Mat FIRWindowBP( const cv::Mat& delta, float fl, float fh)
{
	int timeDimension = 2; // 0-based index for time dimension
	int len = delta.cols;
    	fl = fl * 2.0f; // Scale to be a fraction of Nyquist frequency
    	fh = fh * 2.0f;
    	
    	// Design the FIR filter kernel
   	 std::vector<float> B(len);
    	for (int i = 0; i < len; i++) {
        	B[i] = static_cast<float>(i >= fl && i <= fh);
    	}
    	
    	 // Perform frequency domain filtering
    	cv::Mat transferFunction(1, 1, len, CV_32FC1);
    	cv::Mat temp;
    	cv::dft(B, temp);
    	temp.copyTo(transferFunction);
    	
    	int M = delta.rows;
    	int batches = 20;
    	int batchSize = static_cast<int>(std::ceil(static_cast<float>(M) / batches));
    	transferFunction = transferFunction.colRange(0, len);
    	
    	for (int k = 0; k < batches; k++) {
        	int startIdx = batchSize * k;
        	int endIdx = std::min(batchSize * (k + 1), M);
        	cv::Mat freqDom = delta.rowRange(startIdx, endIdx).clone();
        	freqDom = freqDom.reshape(1, freqDom.rows);

        	cv::Mat transferFunctionReplicated(freqDom.rows, 1, len, CV_32FC1);
        	for (int i = 0; i < freqDom.rows; i++) {
            		transferFunction.copyTo(transferFunctionReplicated.row(i));
        	}

        	cv::Mat freqDomComplex(freqDom.rows, freqDom.cols, CV_32FC2);
        	cv::Mat freqDomComplexSplit[] = {cv::Mat(freqDomComplex, cv::Rect(0, 0, len, freqDom.rows)),
                                         cv::Mat(freqDomComplex, cv::Rect(len, 0, len, freqDom.rows))};
        	freqDom.copyTo(freqDomComplexSplit[0]);
        	cv::Mat zeros = cv::Mat::zeros(freqDomComplexSplit[0].size(), freqDomComplexSplit[0].type());
        	zeros.copyTo(freqDomComplexSplit[1]);

		cv::Mat result;
		cv::mulSpectrums(freqDomComplex, transferFunctionReplicated, result, 0);
		cv::idft(result, result, cv::DFT_SCALE | cv::DFT_REAL_OUTPUT);

		// Copy the filtered result back to the delta matrix
		cv::Mat(result).reshape(1, freqDom.rows).copyTo(delta.rowRange(startIdx, endIdx));
    	}

    	return delta;

}
