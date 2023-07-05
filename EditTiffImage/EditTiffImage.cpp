#include <iostream>
#include <fstream>
#include "Matrix.h"
#include <string>
#include "cmath"
#include "OpenAndSave.h"
#include "EditMatrix.h"

using namespace std;

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "ru");

    Image image;
    vector<IFD> ifdArray;

    Matrix matrix = readTiff(argv[1], image, ifdArray);

    Matrix matrix2 = scaleImage(matrix,1.75,1);

    writeTiff(matrix2, argv[2], image, ifdArray);

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