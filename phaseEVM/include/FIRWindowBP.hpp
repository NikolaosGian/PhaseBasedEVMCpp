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
   	cv::Mat transferFunction(1, 1, CV_32FC1);
    	cv::Mat BMat(1, 1, CV_32F);//cv::Mat BMat(1, 1, len, CV_64F, B.data());
    	for (int i = 0; i < len; ++i) {
        	BMat.at<double>(0, i) = B[i];
    	}
    	
    	cv::Mat temp;
    	cv::dft(BMat, temp,cv::DFT_SCALE | cv::DFT_COMPLEX_OUTPUT);
    	
    	int M = delta.rows;
    	int batches = 20;
    	int batchSize = static_cast<int>(std::ceil(static_cast<float>(M) / batches));

    	std::string ty =  type2str( transferFunction.type() );
 	printf(" transferFunction Matrix: %s %dx%d \n", ty.c_str(), transferFunction.cols, transferFunction.rows );
    	
    	ty =  type2str( delta.type() );
 	printf(" delta Matrix: %s %dx%d \n", ty.c_str(), delta.cols, delta.rows );
    	
    	temp.copyTo(transferFunction);
    	//transferFunction = temp;
    	//transferFunction = transferFunction.colRange(0, len);
    	
    	for (int k = 0; k < batches; k++) {
    		printf("k = %d\n",k);
        	int startIdx = batchSize * k;
        	int endIdx = std::min(batchSize * (k + 1), M);

        	cv::Mat freqDom = delta.rowRange(startIdx, endIdx).clone();
        	freqDom = freqDom.reshape(1, freqDom.rows);
		
        	//cv::Mat transferFunctionReplicated(freqDom.rows, freqDom.cols, transferFunction.type());
        	
        	// Split the complex matrix into its real and imaginary parts
		std::vector<cv::Mat> parts;
		cv::split(transferFunction, parts);
		
		// Replicate the real and imaginary parts
		cv::Mat realPartReplicated(parts[0].size() , parts[0].type());
		cv::Mat imagPartReplicated(parts[1].size() , parts[1].type());
		
		//cv::repeat(parts[0], freqDom.rows, freqDom.cols, realPartReplicated);
		//cv::repeat(parts[1], freqDom.rows, freqDom.cols, imagPartReplicated);
		
		
		ty =  type2str( parts[0].type() );
 		printf(" parts[0] Matrix: %s %dx%d \n", ty.c_str(), parts[0].cols, parts[0].rows );
 			ty =  type2str( parts[1].type() );
 		printf(" parts[1] Matrix: %s %dx%d \n", ty.c_str(), parts[1].cols, parts[1].rows );
		
		// Replicate the real and imaginary parts
		/*
		//Real
		for (int i = 0; i < parts[0].rows; i++) {
    			for (int j = 0; j < parts[0].cols; j++) {
    				realPartReplicated.at<float>(i, j) = parts[0].at<float>(i,j);
    			}
		}
		
		//imaginary
		for (int i = 0; i < parts[1].rows; i++) {
    			for (int j = 0; j < parts[1].cols; j++) {
    				imagPartReplicated.at<float>(i, j) = parts[1].at<float>(i,j);
    			}
		}
		*/
		
		
		realPartReplicated = parts[0].clone();
		imagPartReplicated = parts[1].clone();
		
		
		
		cv::Mat transferFunctionReplicated;
		std::vector<cv::Mat> temp;
		temp.push_back(realPartReplicated);
		temp.push_back(imagPartReplicated);

		cv::merge(temp, transferFunctionReplicated);       	
        	//cv::repeat(transferFunction, transferFunction.rows, transferFunction.cols, transferFunctionReplicated);
        	/*
        	
        	if(transferFunction.rows  <=0 && transferFunction.cols <= 0){
        		cv::repeat(transferFunction, 1, 2, transferFunctionReplicated);
		}else{

        		cv::repeat(transferFunction, transferFunction.rows, transferFunction.cols, transferFunctionReplicated);
        	}
        	*/
        	/*
        	// Regular replication for non-empty matrix
	    	for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 1; j++) {
			    transferFunctionReplicated.at<cv::Vec2f>(i, j) = transferFunction.at<cv::Vec2f>(i % transferFunction.rows, j % transferFunction.cols);
			}
	    	}
        	*/
        	transferFunctionReplicated = transferFunction.clone();
        	
        	printf(" after transferFunctionReplicated = transferFunction.clone(); \n" );
		cv::Mat freqDomComplex(freqDom.rows, freqDom.cols, CV_32FC2);
        	//cv::Mat freqDomComplex(freqDom.rows, freqDom.cols, CV_32FC1);
		printf(" after cv::Mat freqDomComplex(freqDom.rows, freqDom.cols, CV_32FC2);\n" );
        	
        	//cv::Mat freqDomComplexSplit[] = {cv::Mat(freqDomComplex, cv::Rect(0, 0, len, freqDom.rows)),cv::Mat(freqDomComplex, cv::Rect(len, 0, len, freqDom.rows))};
        	
        	
   		
        	cv::Rect roiReal(0, 0, len, freqDom.rows);
        	printf(" after cv::Rect roiReal(0, 0, len, freqDom.rows);\n" );
		cv::Rect roiImag(len, 0, len + 1, freqDom.rows);
		printf(" after cv::Rect roiImag(0, 0, len, freqDom.rows);\n" );
		
		ty =  type2str( freqDomComplex.type() );
 		printf(" freqDomComplex Matrix: %s cols_%dxrows_%d \n", ty.c_str(), freqDomComplex.cols, freqDomComplex.rows );
		
		/*

		if (roiReal.width <= freqDomComplex.cols && roiReal.height <= freqDomComplex.rows &&
    roiImag.width <= freqDomComplex.cols && roiImag.height <= freqDomComplex.rows){
    			cv::Mat freqDomReal(freqDomComplex, roiReal);
			cv::Mat freqDomImag(freqDomComplex, roiImag);
			printf("inside in If cv::Mat freqDomImag(freqDomComplex, roiImag);\n" );
    
    		}
    		
		
		
		cv::Mat freqDomReal(freqDomComplex);
		cv::Mat freqDomImag(freqDomComplex);
		
		*/
		cv::Mat freqDomReal(freqDomComplex, roiReal);
		cv::Mat freqDomImag(freqDomComplex, roiImag);
		printf(" after freqDomImag(freqDomComplex, roiImag);\n" );
	
 		
		
        	
        	//freqDom.copyTo(freqDomComplexSplit[0]);	
        	freqDom.copyTo(freqDomReal);
        	printf(" after freqDom.copyTo(freqDomReal);\n" );

        	//cv::Mat zeros = cv::Mat::zeros(freqDomComplexSplit[0].size(), freqDomComplexSplit[0].type());
        	cv::Mat zeros = cv::Mat::zeros(freqDomReal.size(), freqDomReal.type());
        	printf(" after cv::Mat zeros = cv::Mat::zeros(freqDomReal.size(), freqDomReal.type());\n" );
        	//zeros.copyTo(freqDomComplexSplit[1]);
        	zeros.copyTo(freqDomImag);
        	printf(" after zeros.copyTo(freqDomImag);\n" );
 

 				

		cv::Mat result(freqDomComplex.size(),freqDomComplex.type());
		printf(" after cv::Mat result(freqDomComplex.size(),freqDomComplex.type());\n" );
		
		ty =  type2str( freqDomComplex.type() );
	 	printf(" freqDomComplex Matrix: %s %dx%d \n", ty.c_str(), freqDomComplex.cols, freqDomComplex.rows );
 		ty =  type2str( transferFunctionReplicated.type() );
 		printf(" transferFunctionReplicated Matrix: %s %dx%d \n", ty.c_str(), transferFunctionReplicated.cols, transferFunctionReplicated.rows );
 		
 		
 		cv::Mat transferFunctionReplicatedTemp(1, 2, transferFunctionReplicated.type());
 		printf(" after cv::Mat transferFunctionReplicatedTemp(1, 2, transferFunctionReplicated.type());\n" );
 

		// Set the values of transferFunctionReplicated to match the original matrix
		transferFunctionReplicatedTemp.at<cv::Vec2f>(0, 0) = transferFunctionReplicated.at<cv::Vec2f>(0, 0);
		printf(" after transferFunctionReplicatedTemp.at<cv::Vec2f>(0, 0) = transferFunctionReplicated.at<cv::Vec2f>(0, 0);\n" );
		transferFunctionReplicatedTemp.at<cv::Vec2f>(1, 0) = transferFunctionReplicated.at<cv::Vec2f>(1, 0);
		printf(" after transferFunctionReplicatedTemp.at<cv::Vec2f>(1, 0) = transferFunctionReplicated.at<cv::Vec2f>(1, 0);\n" );
	
		ty =  type2str( transferFunctionReplicatedTemp.type() );
 		printf("transferFunctionReplicatedTemp Matrix: %s %dx%d \n", ty.c_str(), transferFunctionReplicatedTemp.cols, transferFunctionReplicatedTemp.rows );
 		
		cv::mulSpectrums(freqDomComplex, transferFunctionReplicatedTemp, result, 0);
		printf(" after cv::mulSpectrums(freqDomComplex, transferFunctionReplicatedTemp, result, 0);\n" );
		cv::idft(result, result, cv::DFT_SCALE | cv::DFT_REAL_OUTPUT);
		printf(" after cv::idft(result, result, cv::DFT_SCALE | cv::DFT_REAL_OUTPUT);\n" );


		// Copy the filtered result back to the delta matrix
		cv::Mat(result).reshape(1, freqDom.rows).copyTo(delta.rowRange(startIdx, endIdx));
		printf(" after cv::Mat(result).reshape(1, freqDom.rows).copyTo(delta.rowRange(startIdx, endIdx));\n" );
		cv::idft(result, result, cv::DFT_SCALE | cv::DFT_REAL_OUTPUT);
		printf(" after cv::idft(result, result, cv::DFT_SCALE | cv::DFT_REAL_OUTPUT);\n" );
		
		// Copy the filtered result back to the delta matrix
		cv::Mat(result).reshape(1, freqDom.rows).copyTo(delta.rowRange(startIdx, endIdx));
		printf(" after cv::Mat(result).reshape(1, freqDom.rows).copyTo(delta.rowRange(startIdx, endIdx));\n" );
		
    	}
	std::cout << "out of FIRWI" << std::endl;
    	return delta;

}

/*
	int timeDimension = 2;
    	int len = delta.cols;
    	fl = fl * 2.0; // Scale to be a fraction of the Nyquist frequency
    	fh = fh * 2.0;
    
    	// Create the FIR filter
    	std::vector<double> B(len);
    	for (int i = 0; i < len; ++i) {
        	B[i] = (i >= fl && i <= fh) ? 1.0 : 0.0;
    	}
    
    	cv::Mat transferFunction(1, 1, len, CV_64F);
    	cv::Mat BMat(1, 1, CV_64F);//cv::Mat BMat(1, 1, len, CV_64F, B.data());
    	for (int i = 0; i < len; ++i) {
        	BMat.at<double>(0, i) = B[i];
    	}
    	
    	BMat.copyTo(transferFunction);
    	std::cout <<" after BMat.copyTo(transferFunction); " << std::endl;
    
    	int M = delta.size[0];
    	int batches = 20;
    	int batchSize = static_cast<int>(std::ceil(static_cast<double>(M) / batches));
    	
    
    	// Apply the FIR filter to each batch
    	for (int k = 0; k < batches; ++k) {
       	int idx_start = 1 + batchSize * k;
        	int idx_end = std::min((k + 1) * batchSize, M);
        	cv::Range ranges[] = {cv::Range(idx_start - 1, idx_end - 1), cv::Range::all(), cv::Range::all()};
        	std::cout <<" after cv::Range ranges[] " << std::endl;
        
        	// Extract the sub-matrix for the batch
        	cv::Mat deltaBatch(delta, ranges); //cv::Mat deltaBatch = delta(ranges);
        	std::cout <<" after cv::Mat deltaBatch(delta, ranges);" << std::endl;
        
        	// Convert the deltaBatch to frequency domain
        	cv::Mat freqDom;
        	cv::dft(deltaBatch, freqDom, cv::DFT_COMPLEX_OUTPUT);
        	std::cout <<" after cv::dft(deltaBatch, freqDom, cv::DFT_COMPLEX_OUTPUT); " << std::endl;
        
        	// Apply the transfer function
        	for (int i = 0; i < freqDom.size[0]; ++i) {
            		for (int j = 0; j < freqDom.size[1]; ++j) {
                	freqDom.at<cv::Vec2d>(i, j)[0] *= transferFunction.at<double>(0, 0, i);
                	freqDom.at<cv::Vec2d>(i, j)[1] *= transferFunction.at<double>(0, 0, i);
            		}
        	}
        
        	// Convert back to time domain
        	cv::idft(freqDom, deltaBatch, cv::DFT_REAL_OUTPUT);
        	std::cout <<" after cv::idft(freqDom, deltaBatch, cv::DFT_REAL_OUTPUT); " << std::endl;
        
        	// Copy the filtered batch back to the original matrix
        	deltaBatch.copyTo(delta(ranges));
    }

    return delta;
    }
    */
	/*
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
    	
    	std::string ty =  type2str( transferFunction.type() );
 	printf(" transferFunction Matrix: %s %dx%d \n", ty.c_str(), transferFunction.cols, transferFunction.rows );
    	
    	ty =  type2str( temp.type() );
 	printf(" temp Matrix: %s %dx%d \n", ty.c_str(), temp.cols, temp.rows );
    	
    	temp.copyTo(transferFunction);
    	std::cout <<" after temp.copyTo(transferFunction); " << std::endl;
    	
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
*/
