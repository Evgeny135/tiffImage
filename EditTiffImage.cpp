#include "Matrix.h"
#include <string>
#include "OpenAndSave.h"
#include "EditMatrix.h"
#include "boost/program_options.hpp"

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "ru");

    Image image;

    Matrix matrix;

    Matrix current;

    std::string filename;
    std::string outFile;

    boost::program_options::options_description desc("Options");
    boost::program_options::positional_options_description pos_desc;

    pos_desc.add("-input", 1);
    pos_desc.add("-output", 1);

    desc.add_options()
            ("input",boost::program_options::value<std::string>(),"input image")
            ("output",boost::program_options::value<std::string>(),"output image")
            ("-crop",boost::program_options::value<std::vector<std::string>>()->multitoken(),"crop image")
            ("-gs","convert image to grayscale");
//            ("-rotate",boost::program_options::value<int>(),"rotate image");
//            ("-scale",boost::program_options::value<std::vector<double>>()->multitoken(),"scale image")
//            ("-offset",boost::program_options::value<std::vector<int>>()->multitoken(),"offset image");


    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(pos_desc).run(), vm);
    boost::program_options::notify(vm);

    if (vm.count("input")){
        filename = vm["input"].as<std::string>();
    }
    if (vm.count("output")){
        outFile = vm["output"].as<std::string>();
    }

    matrix = readTiff(filename.data(), image);
    current = matrix;

    if (vm.count("-crop")){
        std::vector<std::string> data = vm["-crop"].as<std::vector<std::string>>();
        current = cropImage(image,std::stoi(data[1]),std::stoi(data[2]),current);
    }
    if (vm.count("-gs")){
        current = convertToGrayScale(current);
    }
    if (vm.count("-rotate")){
        current = rotateImage(current,vm["-rotate"].as<int>());
    }
//    if (vm.count("-scale")){
//
//    }
//    if(vm.count("-offset")){
//
//    }

    writeTiff(current,outFile.data(),image);



    //matrix.convertToGreyScale();
//    if (argc == 3) {
//        writeTiff(matrix, argv[2], image, ifdArray);
//    } else if (argc == 4) {
//        if (argv[3] == string("-gs")) {
//            Matrix greyMatrix = convertToGrayScale(matrix);
//            writeTiff(greyMatrix, argv[2], image, ifdArray);
//        }
//    } else if (argc == 6) {
//        if (argv[3] == string("-crop")) {
//            try {
//                stoi(argv[4]);
//                stoi(argv[5]);
//            } catch (...) {
//                cout << "Введено неверное число";
//                exit(1);
//            }
//            Matrix cropMatrix = cropImage(image, stoi(argv[4]), stoi(argv[5]), matrix);
//            writeTiff(cropMatrix, argv[2], image, ifdArray);
//        }
//    } else if (argc == 7) {
//        if (argv[3] == string("-crop")) {
//            if (argv[6] == string("-gs")) {
//                try {
//                    stoi(argv[4]);
//                    stoi(argv[5]);
//                } catch (...) {
//                    cout << "Введено неверное число";
//                    exit(1);
//                }
//                Matrix cropMatrix = cropImage(image, stoi(argv[4]), stoi(argv[5]), matrix);
//                Matrix greyMatrix = convertToGrayScale(cropMatrix);
//                writeTiff(greyMatrix, argv[2], image, ifdArray);
//            }
//        }
//    }
}