#include "type2str.hpp"
#include "phase_evm.h"
#include "maxSCFpyrHt.hpp"
//#include "filters.hpp"
#include "FIRWindowBP.hpp"
//#include "fftshift.hpp"
#include "differenceOfIIR.hpp"
#include "differenceOfButterworths.hpp"
#include  "AmplitudeWeightedBlur.hpp"
#include "buildLevel.hpp"
#include "reconLevel.hpp"

#include "getFilters.hpp"
#include "getFiltersSmoothWindow.hpp"
#include "getFilterIDX.hpp"
//#include "getPolarGrid.hpp"


#define DISPLAY_WINDOW_NAME "Motion Magnified Output"

PhaseEVM::PhaseEVM()
        : input_file_name_()
        , output_file_name_()
        , input_img_width_(0)
        , input_img_height_(0)
        , input_cap_(NULL)
        , output_img_width_(0)
        , output_img_height_(0)
        , output_cap_(NULL)
        , write_output_file_(false)
        , cutoff_freq_low(0.05)  // Hz
        , cutoff_freq_high(0.4)  // Hz
        , alpha(20)
        , frame_num_(0)
        , frame_count_(0)
        , input_fps_(30)
{
}

PhaseEVM::~PhaseEVM()
{
    if (input_cap_ != NULL)
        input_cap_->release();

    if (output_cap_ != NULL)
        output_cap_->release();
}

bool PhaseEVM::init()
{
	//Input:
	//Input file
	input_cap_ = new cv::VideoCapture(input_file_name_);
	if(!input_cap_->isOpened()){
		std::cerr << "Error: Unable to open input video file: " << input_file_name_ << std::endl;
        	return false;
	}
	
	if (input_img_width_ <= 0 || input_img_height_ <= 0)
    	{
		// Use default input image size
		input_img_width_ = input_cap_->get(cv::CAP_PROP_FRAME_WIDTH);
		input_img_height_ = input_cap_->get(cv::CAP_PROP_FRAME_HEIGHT);
    	}
	
	
	frame_count_ = input_cap_->get(cv::CAP_PROP_FRAME_COUNT);
    	input_fps_ = input_cap_->get(cv::CAP_PROP_FPS);
    	std::cout << "Input video resolution is (" << input_img_width_ << ", " << input_img_height_ << ")" << std::endl;
    	
    	// Output:
    	// Output Display Window
    	cv::namedWindow(DISPLAY_WINDOW_NAME, cv::WINDOW_AUTOSIZE);
    	if (output_img_width_ <= 0 || output_img_height_ <= 0)
    	{
		// Use input image size for output
		output_img_width_ = input_img_width_;
		output_img_height_ = input_img_height_;
    	}
    	
    	std::cout << "Output video resolution is (" << output_img_width_ << ", " << output_img_height_ << ")" << std::endl;

    	// Output File:
    	if (!output_file_name_.empty())
        	write_output_file_ = true;
    	
    	if (write_output_file_)
    	{
        	output_cap_ = new  cv::VideoWriter(	output_file_name_, 					// filename
               					getCodecNumber(output_file_name_),                	// codec to be used
						       input_fps_,                                       	// frame rate of the video
						       cv::Size(output_img_width_, output_img_height_),  	// frame size
						       true                                              	// color video
        						);  
        													// NOLINT [whitespace/braces]
        	if (!output_cap_->isOpened())
        	{
            		std::cerr << "Error: Unable to create output video file: " << output_file_name_ << std::endl;
            	return false;
        	}
    	}    	
    	
	std::cout << "Init Successful" << std::endl;
    	return true;    	  	
}


void PhaseEVM::run()
{
	std::cout << "Running Phase Method Eulerian Motion Magnification...\n" << std::endl;
	
	input_cap_->read(img_input_);
	//if (img_input_.empty())
        //    break;
            
	std::cout << "Computing spatial filters "  << std::endl;
	ht = maxSCFpyrHt(cv::Mat::zeros(img_input_.rows, img_input_.cols, CV_8UC1));
	std::cout << "ht = " << ht<<std::endl;
		
	cv::Size dimension(img_input_.rows,img_input_.cols);
	std::vector<double> rVals;
	
        if(pyrType == "octave" )
        {
        	
		for (int i = 0; i <= ht; ++i) {
    			rVals.push_back(std::pow(2.0, -i));
		}
		
		
		
              	//filters = getFilters(dimension,rVals,4);
              	getFilters(filters,dimension,rVals,4);
            	std::cout << "Using octave bandwidth pyramid " << std::endl;
        }
        else if(pyrType == "halfOctave" )
     	{
     		
		for (int i = 0; i <= ht * 2; ++i) {
		    rVals.push_back(std::pow(2.0, -0.5 * i));
		}
		std::cout << "befoar filters  "<<std::endl;
            	//filters = getFilters(dimension,rVals,8,0.75f);
            	 getFilters(filters,dimension,rVals,8,0.75f);
            	std::cout << "Using octave halfOctave pyramid " << std::endl;
     	}
      	else if(pyrType == "smoothHalfOctave" )
        {
            	
            	//filters = getFiltersSmoothWindow(dimension,8, 6, 2, true);
            	getFiltersSmoothWindow(filters,dimension,8, 6, 2, true);
            	std::cout << "Using smoothHalfOctave bandwidth pyramid " << std::endl;
        }
        else if(pyrType == "quarterOctave" )
        {
            	//filters = getFiltersSmoothWindow(dimension,8, 6, 4, true);
            	getFiltersSmoothWindow(filters,dimension,8, 6, 4, true);
            	std::cout << "Using quarterOctave bandwidth pyramid " << std::endl;
        }
        else
        {
            	std::cerr<< "Invalid Filter Types = " << pyrType << std::endl;
        }
               
       getFilterIDX(filters,filtIDX,croppedFilters);
       //croppedFilters = getFilterIDX(filters,filtIDX);
       
       
       //numLevels = filters[0].mask.rows;
       numLevels = filters.size();

       
       // Initialization of motion magnified luma component
       magnifiedLumaFFT = cv::Mat::zeros(img_input_.rows, img_input_.cols, CV_32F);


	
	std::cout << "Moving video to Fourier domain" << std::endl;
	vidFFT.resize(input_cap_->get(cv::CAP_PROP_FRAME_COUNT));


	//std::cout << "input_cap_->get(cv::CAP_PROP_FRAME_COUNT) " << input_cap_->get(cv::CAP_PROP_FRAME_COUNT) << std::endl;
	//vidFFT(input_cap_->get(cv::CAP_PROP_FRAME_COUNT));
	for(k = 0; k < input_cap_->get(cv::CAP_PROP_FRAME_COUNT); k++)
	{
		// Loop through each frame and compute the Fourier transform
		//originalFrame = img_input_;
		//std::cout << "img_input_ " << img_input_ << std::endl;
		
		img_input_.convertTo(originalFrame, CV_32F, 1.0/255.0);
		//std::cout << "originalFrame " << originalFrame << std::endl;
		
		// Convert to grayscale (assuming your frame is single-channel)
		//cv::cvtColor(originalFrameFloat, tVid, cv::COLOR_BGR2GRAY);
		cv::cvtColor(originalFrame, originalFrameNTSC, cv::COLOR_BGR2YUV);

		cv::split(originalFrameNTSC, channels1);
		tVid = channels1[0];
		
		// Resize to the desired dimensions (
		cv::resize(tVid, tVid, cv::Size(input_img_width_, input_img_height_));
		
		// Compute the Fourier transform and store it in vidFFT
		cv::dft(tVid, dfftResult, cv::DFT_COMPLEX_OUTPUT);
		fftshift(dfftResult);
		vidFFT[k] = dfftResult;
		
		input_cap_->read(img_input_);
	}
	
		
	input_cap_->set(cv::CAP_PROP_POS_FRAMES, 0); // set at start frame the parser
	
	//std::cout << "level loop " << std::endl;
	//cv::Mat copy = vidFFT[0].clone();
	//cv::resize(croppedFilters, croppedFilters, cv::Size(input_img_width_, input_img_height_));
	//cv::resize(croppedFilters, croppedFilters, cv::Size(input_img_width_, input_img_height_));
	//std::cout << "numLevels =  " << numLevels <<  std::endl;
	//std::cout << "vidFFT.size = " <<  vidFFT.size() <<std::endl;
	//std::cout << "vidFFT[0].size = " <<  vidFFT[0].size() <<std::endl;
	
	
	
	for(int level = 1; level > numLevels -2; level--)
	{
		printf("Level = %d\n", level);
		// Compute phases of the level
		//pyrRef = buildLevel(vidFFT[0], croppedFilters[level], filtIDX[level]);
		pyrRef = buildLevel(vidFFT[0], croppedFilters, filtIDX, level);
		cv::divide(pyrRef,cv::abs(pyrRef),pyrRefPhaseOrig);

		
		//std::string ty =  type2str( pyrRef.type() );
	    	//printf(" pyrRef Matrix: %s %dx%d \n", ty.c_str(), pyrRef.cols, pyrRef.rows );
	    	
		cv::Mat magnitude, phase;
		std::vector<cv::Mat> planes;
		cv::split(pyrRef, planes);
		cv::cartToPolar(planes[0], planes[1],phase, magnitude, true);

		/*
		std::string ty =  type2str( magnitude.type() );
	    	printf(" magnitude Matrix: %s %dx%d \n", ty.c_str(), magnitude.cols, magnitude.rows );
	    	ty =  type2str( phase.type() );
	    	printf(" phase Matrix: %s %dx%d \n", ty.c_str(), phase.cols, phase.rows );
	    	
		
		ty =  type2str( pyrRef.type() );
	    	printf(" pyrRef Matrix: %s %dx%d \n", ty.c_str(), pyrRef.cols, pyrRef.rows );
		
		*/
		
		cv::phase(planes[0], planes[1], phase);
		//cv::phase(pyrRef, magnitude, phase);
		//std::cout << "After cv::phase(pyrRef, magnitude, phase);" << std::endl;
		// Adjust the phase to be in the range [-π, π]
		for (int i = 0; i < phase.rows; ++i) {
    			for (int j = 0; j < phase.cols; ++j) {
        			float& phase_val = phase.at<float>(i, j);
        			while (phase_val > CV_PI)
            				phase_val -= 2 * CV_PI;
       			while (phase_val < -CV_PI)
            				phase_val += 2 * CV_PI;
    			}
		}
		
		
	    	//ty =  type2str( temp.type() );
	    	//printf(" temp Matrix: %s %dx%d \n", ty.c_str(), temp.cols, temp.rows );
	    	//pyrRefAngle = cv::Mat::zeros(temp.size(), CV_32F);
	    	
	    	//ty =  type2str( pyrRefAngle.type() );
	    	//printf(" pyrRefAngle Matrix: %s %dx%d \n", ty.c_str(), pyrRefAngle.cols, pyrRefAngle.rows );
	    	//cv::phase(temp, cv::noArray(), pyrRefAngle, true); 

		//cv::phase(pyrRef, cv::noArray(), pyrRefAngle, true);
		//std::cout << "after cv::phase(pyrRef, cv::noArray(), pyrRefAngle, true);" <<std::endl;	
		
		//int rows = pyrRef.rows;
		//int cols = pyrRef.cols;
		//delta(pyrRef.rows, pyrRef.cols, CV_32FC1, cv::Scalar(0.0));
		//delta = cv::Mat(pyrRef.rows, pyrRef.cols, CV_32F, cv::Scalar(0.0));
		
		delta = cv::Mat(phase.rows, phase.cols, CV_32F, cv::Scalar(0.0));
		//delta(cv::Size(phase.cols , phase.rows), CV_32F);
		
		
				
		std::cout << "Processing level " << level << " of " << numLevels << std::endl;
		
		for(int frameIDX = 0; frameIDX < input_cap_->get(cv::CAP_PROP_FRAME_COUNT); frameIDX++)
		{
		filterResponse = buildLevel(vidFFT[frameIDX], croppedFilters, filtIDX, level);
		//filterResponse = buildLevel(vidFFT[frameIDX], croppedFilters[level], filtIDX[level]);
		
		std::vector<cv::Mat> planes1;
		cv::split(filterResponse, planes1);
		cv::phase(planes1[0], planes1[1],pyrCurrent); 
		//cv::phase(filterResponse, cv::noArray(), pyrCurrent, true);
		// Adjust the phase to be in the range [-π, π]
		for (int i = 0; i < pyrCurrent.rows; ++i) {
    			for (int j = 0; j < pyrCurrent.cols; ++j) {
        			float& pyrCurrent_val = pyrCurrent.at<float>(i, j);
        			while (pyrCurrent_val > CV_PI)
            				pyrCurrent_val -= 2 * CV_PI;
       			while (pyrCurrent_val < -CV_PI)
            				pyrCurrent_val += 2 * CV_PI;
    			}
		}
		
		
		
		deltaFrame = cv::Mat::zeros(pyrCurrent.size(), CV_32F);
				
		cv::add(pyrCurrent, cv::Scalar(CV_PI), deltaFrame);
		cv::subtract(deltaFrame, phase, deltaFrame);
    		cv::Scalar twoPi(2 * CV_PI);
    		cv::Mat modded = cv::Mat::zeros(deltaFrame.size(), CV_32F);
    		cv::subtract(deltaFrame, twoPi, modded, deltaFrame < -CV_PI);
    		cv::add(modded, deltaFrame, deltaFrame, deltaFrame < CV_PI);
    		deltaFrame = deltaFrame - CV_PI;
    		deltaFrame.copyTo(delta(cv::Rect(0, 0, deltaFrame.cols, deltaFrame.rows)));
		
		
		/*
		for (int i = 0; i < pyrCurrent.rows; i++) {
        		for (int j = 0; j < pyrCurrent.cols; j++) {
            			float pyrCurrentVal = pyrCurrent.at<float>(i, j);
            			float pyrRefVal = phase.at<float>(i, j);
            			//float pyrRefVal = pyrRef.at<float>(i, j);
            			deltaFrame.at<float>(i, j) = std::fmod(M_PI + pyrCurrentVal - pyrRefVal, 2.0 * M_PI) - M_PI;
            			
            			std::cout << "after deltaFrame.at<float>(i, j) = std::fmod(M_PI + pyrCurrentVal - pyrRefVal, 2.0 * M_PI) - M_PI;" << std::endl;
        			}
    			}
    		// Store deltaFrame in the delta matrix
    		//std::cout << "befoar  deltaFrame.copyTo(delta.col(frameIDX));" << std::endl;
    		//deltaFrame.copyTo(delta.col(frameIDX));
    		deltaFrame.copyTo(delta.col(frameIDX));
    		//std::cout << "after  deltaFrame.copyTo(delta.col(frameIDX));" << std::endl;
		
		std::cout << "after 2 fors" << std::endl;
		*/
		}
		
		
		// Temporal Filtering
        	std::cout <<"Bandpassing phases" << std::endl;
        	if(temporalFilter == "FIRWindowBP")
        		delta1 = FIRWindowBP(delta,cutoff_freq_low,cutoff_freq_high); // not working need debug		
        	else if(temporalFilter == "differenceOfIIR")
        		delta1 = differenceOfIIR(delta,cutoff_freq_low,cutoff_freq_high);		//working
        	else if(temporalFilter == "differenceOfButterworths")
        		delta1 = differenceOfButterworths(delta,cutoff_freq_low,cutoff_freq_high);	//working
 /*
        	else if(temporalFilter == "AmplitudeWeightedBlur")
        		delta1 = AmplitudeWeightedBlur(delta,cutoff_freq_low,cutoff_freq_high);
        		*/
        	else
        	{
        		std::cerr<< "Invalid temporalFilter Type = " << temporalFilter << std::endl;
        		break;
        	}
        	
        	std::cout << "after delta1" << std::endl;
        	// Apply magnification
        	std::cout <<"Applying magnification" << std::endl;
        	for(int frameIDX = 0; frameIDX < input_cap_->get(cv::CAP_PROP_FRAME_COUNT); frameIDX++)
        	{
        		phaseOfFrame = delta1.col(frameIDX);
        		std::cout <<"after phaseOfFrame = delta1.col(frameIDX);" << std::endl;
        		
        		originalLevel = buildLevel(vidFFT[frameIDX], croppedFilters, filtIDX, level);
        		std::cout <<"after originalLevel = buildLevel(vidFFT[frameIDX], croppedFilters, filtIDX, level);" << std::endl;
        		//originalLevel = buildLevel(vidFFT[frameIDX], croppedFilters[level], filtIDX[level]);
        		
        		
        		if(sigma != 0)
        		{
        			cv::abs(originalLevel);
        			std::cout <<"std::numeric_limits<float>::epsilon()= " << std::numeric_limits<float>::epsilon() <<std::endl; 
        			std::cout <<"originalLevel + std::numeric_limits<float>::epsilon() = " << originalLevel + std::numeric_limits<float>::epsilon()<<std::endl; 
        			originalLevel = originalLevel + std::numeric_limits<float>::epsilon();
        			std::cout <<"befoar blurredPhase = AmplitudeWeightedBlur(phaseOfFrame, abs(originalLevel) + std::numeric_limits<float>::epsilon(), sigma);" << std::endl;
        			blurredPhase = AmplitudeWeightedBlur(phaseOfFrame, originalLevel, sigma);
        			std::cout <<"after blurredPhase = AmplitudeWeightedBlur(phaseOfFrame, abs(originalLevel) + std::numeric_limits<float>::epsilon(), sigma);" << std::endl;
        		}
        		
        		// Increase phase variation
			for (int row = 0; row < phaseOfFrame.rows; row++) {
    				for (int col = 0; col < phaseOfFrame.cols; col++) {
        				phaseOfFrame.at<float>(row, col) *= alpha;
    				}
			}
			std::cout <<"after phaseOfFrame.at<float>(row, col) *= alpha;" << std::endl;
			
			if(attenuateOtherFreq == 1)
			{
				tempOrig = cv::abs(originalLevel).mul(pyrRefPhaseOrig);
				std::cout <<"after tempOrig = cv::abs(originalLevel).mul(pyrRefPhaseOrig);" << std::endl;
			}else
			{
				tempOrig = originalLevel;
				std::cout <<"after tempOrig = originalLevel;" << std::endl;
			}
			
			// Create a complex matrix with the real part being cos(phaseOfFrame) and the imaginary part being sin(phaseOfFrame)
			cv::split(complexPhase, &expPhase);
			std::cout <<"after cv::split(complexPhase, &expPhase);" << std::endl;
			cv::exp(phaseOfFrame, expPhase);
			std::cout <<"after cv::exp(phaseOfFrame, expPhase);" << std::endl;
			
			channels[0] = tempOrig;
			std::cout <<"after channels[0] = tempOrig;" << std::endl;
			channels[1] = cv::Mat::zeros(tempOrig.size(), CV_32F); // Imaginary part is 0 for tempOrig
			std::cout <<"after channels[1] = cv::Mat::zeros(tempOrig.size(), CV_32F);" << std::endl;
			cv::merge(channels,2,tempOrigComplex);
			std::cout <<"after cv::merge(channels, 2, tempOrigComplex);" << std::endl;
			
			// Perform element-wise complex multiplication
			std::string ty =  type2str( expPhase.type() );
	    		printf(" expPhase Matrix: %s %dx%d \n", ty.c_str(), expPhase.cols, expPhase.rows );
	    		
	    		ty =  type2str( tempOrigComplex.type() );
	    		printf(" tempOrigComplex Matrix: %s %dx%d \n", ty.c_str(), tempOrigComplex.cols, tempOrigComplex.rows );
			
			// Create a target matrix with CV_32FC3 2x2
			cv::Mat expPhaseTemp(tempOrigComplex.size(), tempOrigComplex.type());

			// Transform the expPhase from 32FC1 1x2  -> 32FC3 2x2 to do multiply as descripe below

			for (int i = 0; i < expPhaseTemp.rows; i++) {
			    for (int j = 0; j < expPhaseTemp.cols; j++) {
				if (j < expPhase.cols) {
				    // Copy the value from expPhase to the target matrix
				    expPhaseTemp.at<cv::Vec3f>(i, j)[0] = expPhase.at<float>(0, j);
				    expPhaseTemp.at<cv::Vec3f>(i, j)[1] = 0.0f; // Set the second channel to 0
				    expPhaseTemp.at<cv::Vec3f>(i, j)[2] = 0.0f; // Set the third channel to 0
				} else {
				    // If there are no more values in expPhase, fill with zeros
				    expPhaseTemp.at<cv::Vec3f>(i, j) = cv::Vec3f(0.0f, 0.0f, 0.0f);
				}
			    }
			}
		
			cv::multiply(expPhaseTemp, tempOrigComplex,tempTransformOut);	//cv::multiply(expPhase, tempOrigComplex, tempTransformOut); 
			std::cout <<"after cv::multiply(expPhase, tempOrigComplex, tempTransformOut);" << std::endl;
			
			//Transform the tempTransformOut from 32FC3 to 32FC1 do to DFT inside of reconLevel
			cv::Mat temp2TransformOut(tempTransformOut.size(),CV_32FC1);
			for (int i = 0; i < temp2TransformOut.rows; i++) {
    				for (int j = 0; j < temp2TransformOut.cols; j++) {

        				temp2TransformOut.at<cv::Vec2f>(i, j)[0] = tempTransformOut.at<cv::Vec3f>(i, j)[0];
        				// if uncoment this the type can be CV_32FC2 temp2TransformOut.at<cv::Vec2f>(i, j)[1] = tempTransformOut.at<cv::Vec3f>(i, j)[1];
    				}
			}
			std::cout <<"befoar curLevelFrame = reconLevel(tempTransformOut, croppedFilters,level);" << std::endl;
			curLevelFrame = reconLevel(temp2TransformOut, croppedFilters,level);	//curLevelFrame = reconLevel(tempTransformOut, croppedFilters,level);
			std::cout <<"after curLevelFrame = reconLevel(tempTransformOut, croppedFilters,level);" << std::endl;
			/*
			// Update magnifiedLumaFFT using element-wise addition
			for (int i = 0; i < filtIDX.size(); ++i) {
				//int index = filtIDX[i];
			    	//int row = index / magnifiedLumaFFT.cols;  // Calculate the row from the 1D index
			    	//int col = index % magnifiedLumaFFT.cols;  // Calculate the column from the 1D index
			    
			    	int row = filtIDX[i].dim1[0]; //int row = filtIDX[i][0].y;  // Access the y-coordinate (row) from the Point
			    	std::cout <<"int row = filtIDX[i].dim1[0]; = " << row << std::endl;
    				int col = filtIDX[i].dim2[0]; //int col = filtIDX[i][0].x;  // Access the x-coordinate (column) from the Point
				std::cout <<"int col = filtIDX[i].dim2[0]; = " << col << std::endl;
				
			    	// Ensure that the indices are within the bounds of magnifiedLumaFFT
			    	if (row >= 0 && row < magnifiedLumaFFT.rows && col >= 0 && col < magnifiedLumaFFT.cols) {
					magnifiedLumaFFT.at<float>(row, col) += curLevelFrame.at<float>(i);
					std::cout <<"after magnifiedLumaFFT.at<float>(row, col) += curLevelFrame.at<float>(i);"  << std::endl;
			    	}
			}
			
			
			//magnifiedLumaFFT.at<float>(filtIDX[level].dim1[0], filtIDX[level].dim2[0];) += curLevelFrame.at<float>(filtIDX[level].dim1[0], filtIDX[level].dim2[0];);
			if (filtIDX[level].dim1[0] >= 0 && filtIDX[level].dim1[0] < magnifiedLumaFFT.rows && filtIDX[level].dim2[0] >= 0 && filtIDX[level].dim2[0] < magnifiedLumaFFT.cols) {
			    // Add curLevelFrame to the specified element
			    //magnifiedLumaFFT.at<float>(filtIDX[level].dim1[0], filtIDX[level].dim2[0]) += curLevelFrame;
			    
			}
			
			*/
			
			
			
			ty =  type2str( curLevelFrame.type() );
	    		printf(" curLevelFrame Matrix: %s %dx%d \n", ty.c_str(), curLevelFrame.cols, curLevelFrame.rows );
			
			ty =  type2str( magnifiedLumaFFT.type() );
	    		printf(" magnifiedLumaFFT Matrix: %s %dx%d \n", ty.c_str(), magnifiedLumaFFT.cols, magnifiedLumaFFT.rows );
			/*
			if (filtIDX[level].dim1[0] >= 0 && filtIDX[level].dim1[0] < magnifiedLumaFFT.rows && filtIDX[level].dim1[0] >= 0 && filtIDX[level].dim1[0] < magnifiedLumaFFT.cols) {
				magnifiedLumaFFT.at<float>(filtIDX[level].dim1[0], filtIDX[level].dim1[0]) += curLevelFrame.at<float>(frameIDX);
				std::cout <<"after magnifiedLumaFFT.at<float>(row, col) += curLevelFrame.at<float>(i);"  << std::endl;
				
			}
			*/
			cv::Mat tempCurLevelFrame(magnifiedLumaFFT.size(), magnifiedLumaFFT.type());
			cv::resize(curLevelFrame, tempCurLevelFrame, magnifiedLumaFFT.size());
			
			cv::add(magnifiedLumaFFT,tempCurLevelFrame, magnifiedLumaFFT);
			std::cout <<"after cv::add(magnifiedLumaFFT,tempCurLevelFrame, magnifiedLumaFFT);"  << std::endl;
			
        	}
        }
	
	
	
	std::cout <<"befoar // Add unmolested lowpass residual"  << std::endl;
	// Add unmolested lowpass residual
	level = filters.size();
	for(int frameIDX = 0; frameIDX < input_cap_->get(cv::CAP_PROP_FRAME_COUNT); frameIDX++)
	{	
		//extractedRegionVid = vidFFT[frameIDX](cv::Rect(filtIDX[level * 2], filtIDX[level * 2 + 1], vidFFT[frameIDX].cols, vidFFT[frameIDX].rows));
		//extractedRegionFilters = croppedFilters(cv::Rect(filtIDX[level * 2], filtIDX[level * 2 + 1], croppedFilters.cols, croppedFilters.rows));
		//extractedRegionVid = vidFFT[frameIDX](cv::Rect(filtIDX[level *2][0].y, filtIDX[level *2 + 1][0].y, vidFFT[frameIDX].cols, vidFFT[frameIDX].rows));
		//extractedRegionFilters = croppedFilters(cv::Rect(filtIDX[level *2][0].y, filtIDX[level *2 + 1][0].y, croppedFilters[0].cols, croppedFilters[0].rows));
		//extractedRegionFilters = croppedFilters[0](cv::Rect(filtIDX[level * 2][0].y, filtIDX[level * 2 + 1][0].y, croppedFilters[0].cols, croppedFilters[0].rows));
		//cv::multiply(extractedRegionVid, extractedRegionFilters, multipliedResult);
		
		int dim1_start = filtIDX[level * 2].dim1[0];
		int dim2_start = filtIDX[level * 2].dim2[0];

		int dim1_end = filtIDX[level * 2 + 1].dim1[0];
		int dim2_end = filtIDX[level * 2 + 1].dim2[0];

		cv::Rect region(dim1_start, dim2_start, dim1_end - dim1_start + 1, dim2_end - dim2_start + 1);

		extractedRegionVid = vidFFT[frameIDX](region);
		extractedRegionFilters = croppedFilters[0](region);
		cv::multiply(extractedRegionVid, extractedRegionFilters, multipliedResult);
		
		for (int i = 0; i < filtIDX.size(); ++i) {
			int index = filtIDX[i].dim1[0]; //int index = filtIDX[i][0].y;
		    	int row = index / magnifiedLumaFFT.cols;  // Calculate the row from the 1D index
		    	int col = index % magnifiedLumaFFT.cols;  // Calculate the column from the 1D index
			    
		   	// Ensure that the indices are within the bounds of magnifiedLumaFFT
		   	if (row >= 0 && row < magnifiedLumaFFT.rows && col >= 0 && col < magnifiedLumaFFT.cols) {
				magnifiedLumaFFT.at<float>(row, col) += multipliedResult.at<float>(i);
		    	}
		}
	}
	
	
	input_cap_->read(img_input_);
	//if (img_input_.empty())
        //    break;
        
        //res.resize(img_input_.rows, std::vector<std::vector<std::vector<uint8_t>>>(img_input_.cols, std::vector<std::vector<uint8_t>>(3, std::vector<uint8_t>(input_cap_.get(cv::CAP_PROP_FRAME_COUNT)))));
        
        outFrame.create(img_input_.rows, img_input_.cols, CV_8UC3);
        
     	cv::Mat tempRes(input_img_height_, input_img_width_, CV_8UC3, cv::Scalar(0));
     	
     	//cv::resize(res, res, cv::Size(input_img_width_, input_img_height_));
     	
        for(k = 0; k < input_cap_->get(cv::CAP_PROP_FRAME_COUNT); k++)
        {
        	
        	cv::dft(magnifiedLumaFFT, magnifiedLuma, cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);
        	
        	//cv::dft(magnifiedLumaFFT.col(k);, magnifiedLumaSpatial, cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);
        	//magnifiedLuma = magnifiedLumaSpatial.clone();
        	magnifiedLuma.convertTo(outFrame(cv::Rect(0, 0, input_img_width_, input_img_height_)), CV_8U);
        	
        	img_input_.convertTo(originalFrame, CV_32F, 1.0 / 255.0);
        	cv::cvtColor(originalFrame, originalFrameNTSC, cv::COLOR_BGR2YUV);
        	
        	// Resize the frame
        	cv::resize(originalFrameNTSC, resizedFrame, cv::Size(input_img_width_, input_img_height_));
        	
        	
		// Split the originalFrame into its channels (Y, U, V)
		cv::split(originalFrame, channels2);
		
		// Copy the U and V channels from the originalFrame to the corresponding channels in outFrame
		channels2[1].copyTo(resizedFrame(cv::Rect(0, 0, input_img_width_, input_img_height_))); // U channel
		channels2[2].copyTo(resizedFrame(cv::Rect(0, 0, input_img_width_, input_img_height_))); // V channel
		
		// Convert the outFrame back to RGB
		cv::cvtColor(resizedFrame, resizedFrame, cv::COLOR_YUV2BGR);
		
		outputFrame = resizedFrame.clone();	// Make a copy of resizedFrame
		outputFrame.convertTo(outputFrame, CV_8U, 255.0); // Convert to 8-bit unsigned integer format
		// Assuming res is a 4D cv::Mat with dimensions (height, width, channels, nF)
		//outputFrame.copyTo(tempRes(cv::Rect(0, 0, input_img_width_, input_img_height_)).colRange(0, 3).rowRange(0, input_img_height_).depth(0).channel(0), outputFrame.depth());
		outputFrame.copyTo(tempRes(cv::Rect(0, 0, input_img_width_, input_img_height_)).colRange(0, 3).rowRange(0, input_img_height_));

		
        }
        
	//Live video
	cv::imshow(DISPLAY_WINDOW_NAME, tempRes);
	//Writing
        if (write_output_file_)
            output_cap_->write(tempRes);


       /*char c = cv::waitKey(1);
       if (c == 27)
           break;
           */	
}

int PhaseEVM::getCodecNumber(std::string file_name)
{
    std::string file_extn = file_name.substr(file_name.find_last_of('.') + 1);

    // Currently supported video formats are AVI and MPEG-4
    if (file_extn == "avi")
    	return cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
    else if (file_extn == "mp4")
        return cv::VideoWriter::fourcc('D', 'I', 'V', 'X');
    else
        return -1;
}

