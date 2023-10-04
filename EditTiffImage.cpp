#include "Matrix.h"
#include <string>
#include "OpenAndSave.h"
#include "EditMatrix.h"
#include "RestoreParam.h"
#include "boost/program_options.hpp"

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "ru");

    Image image;
    Matrix<RGB> current;

    std::string filename;
    std::string outFile;

    boost::program_options::options_description desc("Options");
    boost::program_options::positional_options_description pos_desc;

    pos_desc.add("input", 1);
    pos_desc.add("output", 1);

    desc.add_options()
            ("input",boost::program_options::value<std::string>()->required(),"input image")
            ("output",boost::program_options::value<std::string>()->required(),"output image")
            ("crop",boost::program_options::value<std::vector<int>>()->multitoken(),"crop image")
            ("gs","convert image to grayscale")
            ("rotate",boost::program_options::value<int>(),"rotate image")
            ("scale",boost::program_options::value<std::vector<double>>()->multitoken(),"scale image")
            ("offset",boost::program_options::value<std::vector<int>>()->multitoken(),"offset image");


    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(pos_desc).run(), vm);
    boost::program_options::notify(vm);

    filename = vm["input"].as<std::string>();
    outFile = vm["output"].as<std::string>();

    current = readTiff(filename.data(), image);

    getKeyPoint(current);
    if (vm.count("crop")){
        std::vector<int> data = vm["crop"].as<std::vector<int>>();
        current = cropImage(image,data[0],data[1],current);
    }

    if (vm.count("gs")){
        current = convertToGrayScale(current);
    }
    if (vm.count("rotate")){
        int angle = vm["rotate"].as<int>();
        current = rotateImage(current,angle);
    }
    if (vm.count("scale")){
        std::vector<double> scaleData = vm["scale"].as<std::vector<double>>();
        current = scaleImage(current, scaleData[0], scaleData[1]);
    }
    if(vm.count("offset")){
        std::vector<int> offsetData = vm["offset"].as<std::vector<int>>();
        current = offsetImage(current,offsetData[0],offsetData[1]);
    }

    getKeyPoint(current);

    writeTiff(current,outFile.data(),image);
}
