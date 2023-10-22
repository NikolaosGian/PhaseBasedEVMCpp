#ifndef PHASE_EVM_H_
#define PHASE_EVM_H_


#include <math.h>
#include <omp.h>

#include <iostream>
#include <string>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <cstdint> // for uint8_t


/*
// Define a structure to store the filter and its properties
struct Filter {
    cv::Mat mask;
};
*/

// Define a structure for the filter indices in both dimensions
struct FilterIndices {
    std::vector<int> dim1;
    std::vector<int> dim2;
};

class PhaseEVM
{

 public:
 	PhaseEVM();
 	~PhaseEVM();
 	
 	bool init();
 	void run();
 	
 private:
	int getCodecNumber(std::string file_name);
	
public:
	const std::string& getInputFileName() const { return input_file_name_; }
    	void setInputFileName(const std::string& inputFileName) { input_file_name_ = inputFileName; }
    	
    	int getInputImgWidth() const { return input_img_width_; }
    	void setInputImgWidth(int width) { input_img_width_ = width; }

    	int getInputImgHeight() const { return input_img_height_; }
    	void setInputImgHeight(int height) { input_img_height_ = height; }

    	const std::string& getOutputFileName() const { return output_file_name_; }
    	void setOutputFileName(const std::string& outputFileName) { output_file_name_ = outputFileName; }

    	int getOutputImgWidth() const { return output_img_width_; }
    	void setOutputImgWidth(int width) { output_img_width_ = width; }

    	int getOutputImgHeight() const { return output_img_height_; }
    	void setOutputImgHeight(int height) { output_img_height_ = height; }
    	
    	int getAttenuateOtherFreq() const { return attenuateOtherFreq; }
    	void setAttenuateOtherFreq(int attenOtherFreq) { attenuateOtherFreq = attenOtherFreq; }
    	
    	double getAlpha() const {return alpha;}
    	void setAlpha(double a) {alpha = a;}
    	
    	double getCutoff_freq_low() const {return cutoff_freq_low;}
    	void setCutoff_freq_low(double lowf) {cutoff_freq_low = lowf;}

    	double getCutoff_freq_high() const {return cutoff_freq_high;}
    	void setCutoff_freq_high(double highf) {cutoff_freq_high = highf;}
    	
     	double getSigma() const {return sigma;}
    	void setSigma(double s) {sigma = s;}  
    	
      	std::string getPyrType() const {return pyrType;}
    	void setPyrType(std::string type) {pyrType = type;}    
    	
      	std::string getTemporalFilter() {return temporalFilter;}
    	void setTemporalFilter(std::string filter) {temporalFilter = filter;}       		 	
   
private:
	std::string input_file_name_;
    	std::string output_file_name_;

	int attenuateOtherFreq;
    	int input_img_width_;
    	int input_img_height_;
    	cv::VideoCapture* input_cap_;

    	int output_img_width_;
   	int output_img_height_;
    	cv::VideoWriter* output_cap_;
    	bool write_output_file_;
    	
    	double alpha;
	double cutoff_freq_low;
	double cutoff_freq_high;
    	double sigma;
    
    	std::string pyrType;
    	std::string temporalFilter;
	
	int frame_num_;
    	int frame_count_;
    	double input_fps_;
    	
    	int ht;
    	int k = 1;
    	int numLevels;
    	int level;
    	
	
	//std::vector<int> filtIDX;
	//std::vector<std::vector<cv::Point>> filtIDX;
	std::vector<FilterIndices> filtIDX;
	std::vector<cv::Mat> vid;
	std::vector<cv::Mat> vidFFT;
	std::vector<cv::Mat> channels1;
	std::vector<cv::Mat> channels2;
	
	cv::Mat im_dft;
	cv::Mat img_input_;
	
	std::vector<cv::Mat> filters;//std::vector<Filter> filters;
	
	//cv::Mat croppedFilters;
	std::vector<cv::Mat> croppedFilters;
	
	cv::Mat magnifiedLumaFFT;
	cv::Mat buildLevel1;
	cv::Mat reconLevel1;
	cv::Mat oroginalFrame;
	cv::Mat tVid;
	cv::Mat dfftResult;
	
	cv::Mat pyrRef, pyrRefPhaseOrig;
	cv::Mat pyrRefAngle;
	cv::Mat delta;
	cv::Mat delta1;
	cv::Mat filterResponse;
	cv::Mat pyrCurrent;
	cv::Mat deltaFrame;
	cv::Mat phaseOfFrame;
	
	cv::Mat originalLevel;
	cv::Mat originalFrame;
	//cv::Mat originalFrameFloat;
	cv::Mat originalFrameNTSC;
	
	cv::Mat blurredPhase;
	cv::Mat tempOrig;
	cv::Mat tempTransformOut;
	cv::Mat complexPhase;
	cv::Mat expPhase;
	cv::Mat tempOrigComplex;
	cv::Mat channels[2];
	cv::Mat curLevelFrame;
	cv::Mat extractedRegionVid;
	cv::Mat extractedRegionFilters;
	cv::Mat multipliedResult;
	
	
	cv::Mat res; 	//std::vector<std::vector<std::vector<std::vector<uint8_t>>>> res; //
	cv::Mat magnifiedLuma;
	//cv::Mat magnifiedLumaSpatial;
	
	cv::Mat outFrame;
	cv::Mat outputFrame;
	cv::Mat resizedFrame;
	
	
	
	
};

#endif
