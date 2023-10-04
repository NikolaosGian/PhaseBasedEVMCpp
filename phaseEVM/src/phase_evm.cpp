#include "phase_evm.h"
#include "maxSCFpyrHt.hpp"
#include "filters.hpp"
#include "FIRWindowBP.hpp"
//#include "fftshift.hpp"
#include "differenceOfIIR.hpp"
#include "differenceOfButterworths.hpp"
#include  "AmplitudeWeightedBlur.hpp"
#include "buildLevel.hpp"
#include "reconLevel.hpp"


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
        if(pyrType == "octave" )
        {
              	filters = generateOctaveFilters(img_input_.rows, img_input_.cols,ht,CV_8UC1);
            	std::cout << "Using octave bandwidth pyramid " << std::endl;
        }
        else if(pyrType == "halfOctave" )
     	{
            	filters = generateHalfOctaveFilters(img_input_.rows, img_input_.cols,ht,8,0.75f);
            	std::cout << "Using octave halfOctave pyramid " << std::endl;
     	}
      	else if(pyrType == "smoothHalfOctave" )
        {
            	filters = generateSmoothHalfOctaveFilters(img_input_.rows, img_input_.cols,2);
            	std::cout << "Using smoothHalfOctave bandwidth pyramid " << std::endl;
        }
        else if(pyrType == "quarterOctave" )
        {
            	filters = generateQuarterOctaveFilters(img_input_.rows, img_input_.cols,4);
            	std::cout << "Using quarterOctave bandwidth pyramid " << std::endl;
        }
        else
        {
            	std::cerr<< "Invalid Filter Types = " << pyrType << std::endl;
        }
            	
       croppedFilters = getFilterIDX(filters,filtIDX);
       std::cout<< "croppedFilters " << croppedFilters.size()<<endl;
       std::cout<< "filtIDX " << filtIDX.size()<<endl;
       numLevels = filters.rows;
	//std::cout<< "numLevels " << numLevels<<endl;	
       
       // Initialization of motion magnified luma component
       magnifiedLumaFFT = cv::Mat::zeros(img_input_.rows, img_input_.cols, CV_32F);
	//std::cout<< "magnifiedLumaFFT " << magnifiedLumaFFT<<endl;
	
	/*
	std::cout<< "im_dft " << im_dft<<endl;
	std::cout<< "croppedFilters " << croppedFilters<<endl;

	std::cout << "filtIDX: ";
	for (int i = 0; i < filtIDX.size(); ++i) {
        	std::cout << filtIDX[i] << " ";
    	}
    	std::cout << std::endl;
	std::cout<< "k " << k<<endl;
	*/
	
	std::cout << "Moving video to Fourier domain" << std::endl;
	vidFFT.resize(input_cap_->get(cv::CAP_PROP_FRAME_COUNT));
	/*
	
	for (size_t i = 0; i < vidFFT.size(); i++) {
        	std::cout << "Matrix " << i << ":\n";
        	std::cout << vidFFT[i] << std::endl;
    	}
	*/
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
		
		//if (img_input_.empty())
            	//	break;
	}
	
	
	/*
	for (size_t i = 0; i < vidFFT.size(); i++) {
        	std::cout << "Matrix vidFFT " << i << ":\n";
        	std::cout << vidFFT[i] << std::endl;
    	}
    	*/
	
	
	input_cap_->set(cv::CAP_PROP_POS_FRAMES, 0); // set at start frame the parser
	std::cout << "level loop " << std::endl;
	//cv::Mat copy = vidFFT[0].clone();
	//cv::resize(croppedFilters, croppedFilters, cv::Size(input_img_width_, input_img_height_));
	//cv::resize(croppedFilters, croppedFilters, cv::Size(input_img_width_, input_img_height_));
	std::cout << "numLevels =  " << numLevels <<  std::endl;
	std::cout << "vidFFT.size = " <<  vidFFT.size() <<std::endl;
	std::cout << "vidFFT[0].size = " <<  vidFFT[0].size() <<std::endl;
	
	const cv::Mat& element = vidFFT[0];
	
	for(int level = 2; level < numLevels -1; level++)
	{
		// Compute phases of the level
		pyrRef = buildLevel(element, croppedFilters, filtIDX, level);
		std::cout << "pyrRef " << pyrRef <<  std::endl;
		
		pyrRefPhaseOrig = pyrRef / cv::abs(pyrRef);
		std::cout << "pyrRefPhaseOrig " << pyrRefPhaseOrig <<  std::endl;
		
		cv::phase(pyrRef, cv::noArray(), pyrRefAngle, true);
		
		//int rows = pyrRef.rows;
		//int cols = pyrRef.cols;
		//delta(pyrRef.rows, pyrRef.cols, CV_32FC1, cv::Scalar(0.0));
		delta = cv::Mat(pyrRef.rows, pyrRef.cols, CV_32F, cv::Scalar(0.0));
		
		std::cout << "Processing level " << level << " of " << numLevels << std::endl;
		
		for(int frameIDX = 0; frameIDX < input_cap_->get(cv::CAP_PROP_FRAME_COUNT); frameIDX++)
		{
		//filterResponse = buildLevel(vidFFT[frameIDX], croppedFilters, filtIDX, level);
		cv::phase(filterResponse, cv::noArray(), pyrCurrent, true);
		deltaFrame = cv::Mat::zeros(pyrCurrent.size(), CV_32FC1);
		for (int i = 0; i < pyrCurrent.rows; i++) {
        		for (int j = 0; j < pyrCurrent.cols; j++) {
            			float pyrCurrentVal = pyrCurrent.at<float>(i, j);
            			float pyrRefVal = pyrRef.at<float>(i, j);
            			deltaFrame.at<float>(i, j) = std::fmod(M_PI + pyrCurrentVal - pyrRefVal, 2.0 * M_PI) - M_PI;
        			}
    			}
    		// Store deltaFrame in the delta matrix
    		//deltaFrame.copyTo(delta.col(frameIDX));
		}
		
		// Temporal Filtering
		
        	std::cout <<"Bandpassing phases" << std::endl;
        	
        	if(temporalFilter == "FIRWindowBP") 
        		delta1 = FIRWindowBP(delta,cutoff_freq_low,cutoff_freq_high);
        	else if(temporalFilter == "differenceOfIIR")
        		delta1 = differenceOfIIR(delta,cutoff_freq_low,cutoff_freq_high);
        	else if(temporalFilter == "differenceOfButterworths")
        		delta1 = differenceOfButterworths(delta,cutoff_freq_low,cutoff_freq_high);
 /*
        	else if(temporalFilter == "AmplitudeWeightedBlur")
        		delta1 = AmplitudeWeightedBlur(delta,cutoff_freq_low,cutoff_freq_high);
        		*/
        	else
        	{
        		std::cerr<< "Invalid temporalFilter Type = " << temporalFilter << std::endl;
        		break;
        	}
        		
        	// Apply magnification
        	std::cout <<"Applying magnification" << std::endl;
        	for(int frameIDX = 0; frameIDX < input_cap_->get(cv::CAP_PROP_FRAME_COUNT); frameIDX++)
        	{
        		phaseOfFrame = delta1.col(frameIDX);
        		
        		originalLevel = buildLevel(vidFFT[frameIDX], croppedFilters, filtIDX, level);
        		
        		if(sigma != 0)
        		{
        			blurredPhase = AmplitudeWeightedBlur(phaseOfFrame, abs(originalLevel) + std::numeric_limits<float>::epsilon(), sigma);
        		}
        		 // Increase phase variation
        		 
			for (int row = 0; row < phaseOfFrame.rows; row++) {
    				for (int col = 0; col < phaseOfFrame.cols; col++) {
        				phaseOfFrame.at<float>(row, col) *= alpha;
    				}
			}
			
			if(attenuateOtherFreq == 1)
			{
				tempOrig = cv::abs(originalLevel).mul(pyrRefPhaseOrig);
			}else
			{
				tempOrig = originalLevel;
			}
			
			// Create a complex matrix with the real part being cos(phaseOfFrame) and the imaginary part being sin(phaseOfFrame)
			cv::split(complexPhase, &expPhase);
			cv::exp(phaseOfFrame, expPhase);
			
			channels[0] = tempOrig;
			channels[1] = cv::Mat::zeros(tempOrig.size(), CV_32F); // Imaginary part is 0 for tempOrig
			cv::merge(channels, 2, tempOrigComplex);
			
			// Perform element-wise complex multiplication
			cv::multiply(expPhase, tempOrigComplex, tempTransformOut);
			
			//curLevelFrame = reconLevel(tempTransformOut, croppedFilters);
			
			// Update magnifiedLumaFFT using element-wise addition
			for (int i = 0; i < filtIDX.size(); ++i) {
				int index = filtIDX[i];
			    	int row = index / magnifiedLumaFFT.cols;  // Calculate the row from the 1D index
			    	int col = index % magnifiedLumaFFT.cols;  // Calculate the column from the 1D index
			    
			    	// Ensure that the indices are within the bounds of magnifiedLumaFFT
			    	if (row >= 0 && row < magnifiedLumaFFT.rows && col >= 0 && col < magnifiedLumaFFT.cols) {
					magnifiedLumaFFT.at<float>(row, col) += curLevelFrame.at<float>(i);
			    	}
			}
        	}	
	}
	

	// Add unmolested lowpass residual
	level = filters.rows;
	for(int frameIDX = 0; frameIDX < input_cap_->get(cv::CAP_PROP_FRAME_COUNT); frameIDX++)
	{
		extractedRegionVid = vidFFT[frameIDX](cv::Rect(filtIDX[level * 2], filtIDX[level * 2 + 1], vidFFT[frameIDX].cols, vidFFT[frameIDX].rows));
		extractedRegionFilters = croppedFilters(cv::Rect(filtIDX[level * 2], filtIDX[level * 2 + 1], croppedFilters.cols, croppedFilters.rows));
		cv::multiply(extractedRegionVid, extractedRegionFilters, multipliedResult);
		
		for (int i = 0; i < filtIDX.size(); ++i) {
			int index = filtIDX[i];
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


       // char c = cv::waitKey(1);
       // if (c == 27)
        //    break;	
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

