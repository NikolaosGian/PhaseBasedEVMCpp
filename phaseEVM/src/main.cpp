#include <fstream>
#include <iostream>

#include <boost/program_options.hpp>

#include "phase_evm.h"

namespace po = boost::program_options;

int main(int argc, char **argv)
{
    //cv::setBreakOnError(true);
    std::string input_filename;
    std::string output_filename;
    
    int input_width;
    int input_height;
    int output_width;
    int output_height;
    int attenuateOtherFreq;
    
    /*notes 
    samplingrate analoga to video pou exoume san input   
    */
    double alpha;
    double cutoff_freq_low;
    double cutoff_freq_high;
    double sigma;
   
    std::string pyrType;
    std::string temporalFilter;


    if (argc <= 1)
    {
        std::cerr << "Error: Input param filename must be specified!" << std::endl;
        return 1;
    }

    // Read input param file
    std::ifstream file(argv[1]);
    if (!file.is_open())
    {
        std::cerr << "Error: Unable to open param file: " << std::string(argv[1]) << std::endl;
        return 1;
    }

    // Parse param file for getting parameter values
    po::options_description desc("Phase-EVM");
    desc.add_options()
        ("help,h", "produce help message")
        ("input_filename", po::value<std::string>(&input_filename)->default_value( "" ))  // NOLINT [whitespace/parens]
        ("output_filename", po::value<std::string>(&output_filename)->default_value( "" ))  // NOLINT [whitespace/parens]
        ("input_width", po::value<int>(&input_width)->default_value( 0 ))  // NOLINT [whitespace/parens]
        ("input_height", po::value<int>(&input_height)->default_value( 0 ))  // NOLINT [whitespace/parens]
        ("output_width", po::value<int>(&output_width)->default_value( 0 ))  // NOLINT [whitespace/parens]
        ("output_height", po::value<int>(&output_height)->default_value( 0 ))  // NOLINT [whitespace/parens]
        ("alpha", po::value<double>(&alpha)->default_value( 20 ))  // NOLINT [whitespace/parens]
        ("cutoff_freq_low", po::value<double>(&cutoff_freq_low)->default_value( 0.05 ))  // NOLINT [whitespace/parens]
        ("cutoff_freq_high", po::value<double>(&cutoff_freq_high)->default_value( 0.4 ))  // NOLINT [whitespace/parens]
	("sigma", po::value<double>(&sigma)->default_value( 4 ))  // NOLINT [whitespace/parens]
	("attenuateOtherFreq", po::value<int>(&attenuateOtherFreq)->default_value( 0 ))  // NOLINT [whitespace/parens]
	("pyrType", po::value<std::string>(&pyrType)->default_value( "octave" ))  // NOLINT [whitespace/parens]
	("temporalFilter", po::value<std::string>(&temporalFilter)->default_value( "FIRWindowBP" ))  // NOLINT [whitespace/parens]
    ;  // NOLINT [whitespace/semicolon]
    po::variables_map vm;
    po::store(po::parse_config_file(file, desc), vm);
    po::notify(vm);

    // EulerianMotionMag
    PhaseEVM* motion_mag = new PhaseEVM();

    // Set params
    motion_mag->setInputFileName(input_filename);
    motion_mag->setOutputFileName(output_filename);
    motion_mag->setInputImgWidth(input_width);
    motion_mag->setInputImgHeight(input_height);
    motion_mag->setOutputImgWidth(output_width);
    motion_mag->setOutputImgHeight(output_height);
    
    motion_mag->setAttenuateOtherFreq(attenuateOtherFreq);
    motion_mag->setAlpha(alpha);
    motion_mag->setCutoff_freq_low(cutoff_freq_low);
    motion_mag->setCutoff_freq_high(cutoff_freq_high);
    motion_mag->setSigma(sigma);
    motion_mag->setPyrType(pyrType);
    motion_mag->setTemporalFilter(temporalFilter);

    // Init Motion Magnification object
    bool init_status = motion_mag->init();
    if (!init_status)
        return 1;

    // Run Motion Magnification
    motion_mag->run();

    // Exit
    delete motion_mag;
    return 0;
}

