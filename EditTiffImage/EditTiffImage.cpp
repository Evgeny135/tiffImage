#include <iostream>
#include <fstream>
#include "Matrix.h"
#include <string>
#include "cmath"

using namespace std;

unsigned int getBit(char *buffer, int length = 4) {
    unsigned int size = 0;

    for (int i = 0; i < length; i++) {
        size += static_cast<uint32_t>(static_cast<uint8_t>(buffer[i])) << 8 * i;
    }
    return size;
}

void parseBit(char *buffer, unsigned int number) {
    for (int i = 0; i < 4; i++) {
        buffer[i] = number >> 8 * i;
    }
}

struct IFD {
    unsigned int tagId = 256;
    unsigned int typeTag = 1;
    unsigned int length = 1;
    unsigned int offset = 1;
    unsigned int value = 1;

    /*void toString() {
        cout << "tagId: " << this->tagId << " typeId: " << this->typeTag << " length: " << this->length << " offset: " << this->offset << " value: " << this->value << endl;
    }*/
};

struct Image {
    unsigned int offsetIfd = 1;
    unsigned int countIfd = 8;
    unsigned int width = 1;
    unsigned int height = 1;
    unsigned int offsetImage = 8;
    unsigned int dataImageCount = 1;
    unsigned int offsetBitPerSamples = 1;
};

Matrix cropImage(struct Image &image, int width, int height, Matrix &matrix) {

    if (width > image.width) {
        width = image.width;
    }
    if (height > image.height) {
        height = image.height;
    }
    Matrix cropMatrix(width, height);

    image.width = width;
    image.height = height;

    image.offsetIfd = image.width * image.height * 3 + 8;
    image.dataImageCount = image.width * image.height * 3;

    //cropMatrix.setSize(image.width * image.height);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            cropMatrix.set(i, j, matrix.get(i, j));
        }
        //cropMatrix.addRowImageMatrix(row);
    }
    return cropMatrix;
}

Matrix readTiff(const char *filePath, Image &image, vector<IFD> &ifdArray) {

    ifstream in(filePath, ios::binary);
    char buffer[4];

    in.seekg(4);

    Matrix matrix;

    in.read(buffer, 4);
    image.offsetIfd = getBit(buffer);

    in.seekg(image.offsetIfd);
    in.read(buffer, 2);

    image.countIfd = getBit(buffer, 2);

    for (int i = 0; i < image.countIfd; i++) {
        IFD ifd;

        in.read(buffer, 2);
        ifd.tagId = getBit(buffer, 2);

        in.read(buffer, 2);
        ifd.typeTag = getBit(buffer, 2);

        in.read(buffer, 4);
        ifd.length = getBit(buffer, 4);

        in.read(buffer, 4);
        ifd.offset = getBit(buffer, 4);

        ifd.value = ifd.offset;

        if (ifd.tagId == 256) {
            image.width = ifd.value;
        }
        if (ifd.tagId == 257) {
            image.height = ifd.value;
        }
        if (ifd.tagId == 258) {
            ifd.value = 8;
        }
        if (ifd.tagId == 279) {
            image.dataImageCount = image.width * image.height * 3;
            ifd.value = image.dataImageCount;
        }
        if (ifd.tagId == 273) {
            image.offsetImage = 8;
            ifd.value = image.offsetImage;
            ifd.offset = image.offsetImage;
            ifd.length = 1;
        }
        if (ifd.tagId == 278) {
            ifd.value = image.height;
        }
        if (ifd.tagId == 279) {
            ifd.value = image.dataImageCount;
            ifd.length = 1;
        }
        ifdArray.push_back(ifd);
    }

    image.offsetBitPerSamples = (8 + image.dataImageCount + 9 * 12) + 12;
    image.offsetIfd = image.height * image.width * 3 + 8;

    matrix.setWidth(image.width);
    matrix.setHeight(image.height);

    char *imageData = new char[image.dataImageCount];

    in.seekg(image.offsetImage);
    in.read(imageData, image.dataImageCount);
    getBit(imageData, image.dataImageCount);

    matrix.setPixel(imageData);

    return matrix;

}

void writeTiff(Matrix &matrix, const char *filePath, struct Image image, vector<IFD> &ifdArray) {
    ofstream out(filePath, ios::binary);
    char buffer[4];
    char *imageData = new char[image.dataImageCount];

    parseBit(buffer, 2771273);
    out.write(buffer, 4);

    parseBit(buffer, image.offsetIfd);
    out.write(buffer, 4);

    matrix.getImageDataFromMatrix(imageData);

    out.write(imageData, image.dataImageCount);

    parseBit(buffer, 8);
    out.write(buffer, 2);
    for (int i = 0; i < image.countIfd; i++) {
        if (ifdArray[i].tagId == 256 || ifdArray[i].tagId == 257 || ifdArray[i].tagId == 258 || ifdArray[i].tagId == 259
            || ifdArray[i].tagId == 262 || ifdArray[i].tagId == 273 || ifdArray[i].tagId == 277 ||
            ifdArray[i].tagId == 279) {
            parseBit(buffer, ifdArray[i].tagId);
            out.write(buffer, 2);
            parseBit(buffer, ifdArray[i].typeTag);
            out.write(buffer, 2);
            parseBit(buffer, ifdArray[i].length);
            out.write(buffer, 4);
            if (ifdArray[i].tagId == 258) {
                parseBit(buffer, image.offsetBitPerSamples);
                out.write(buffer, 4);
            } else if (ifdArray[i].tagId == 256) {
                parseBit(buffer, image.width);
                out.write(buffer, 4);
            } else if (ifdArray[i].tagId == 257) {
                parseBit(buffer, image.height);
                out.write(buffer, 4);
            } else {
                parseBit(buffer, ifdArray[i].value);
                out.write(buffer, 4);
            }
        }

    }

    out.seekp(image.offsetBitPerSamples);
    parseBit(buffer, 8);
    out.write(buffer, 2);
    parseBit(buffer, 8);
    out.write(buffer, 2);
    parseBit(buffer, 8);
    out.write(buffer, 2);
}

Matrix convertToGrayScale(Matrix &matrix) {
    Matrix greyMatrix(matrix.getWidth(), matrix.getHeight());

    //greyMatrix.setSize(greyMatrix.getWidth() * greyMatrix.getHeight());

    for (int i = 0; i < greyMatrix.getHeight(); i++) {
        for (int j = 0; j < greyMatrix.getWidth(); j++) {
            RGB rgb = matrix.get(i, j);
            int grey = rgb.r * 0.299 + 0.587 * rgb.g + 0.114 * rgb.b;
            rgb.r = grey;
            rgb.g = grey;
            rgb.b = grey;
            greyMatrix.set(i, j, rgb);
        }
    }
    return greyMatrix;
}

Matrix rotateImage(Matrix &matrix, int angle, Image &image) {

    double radians = angle * M_PI / 180;

    double sinA = sin(radians);
    double cosA = cos(radians);

    int centerX = matrix.getWidth()/2;
    int centerY = matrix.getHeight()/2;

    //int newWidth = ceil(abs(cosA) * matrix.getWidth() + abs(sinA) * matrix.getHeight());
    //int newHeight = ceil(abs(cosA) * matrix.getHeight() + abs(sinA) * matrix.getWidth());

    Matrix matrix1(matrix.getWidth(),matrix.getHeight());

    for (int i = 0; i < matrix1.getHeight(); i++) {
        for (int j = 0; j < matrix1.getWidth(); j++) {
            RGB rgb;
            rgb.r = 0;
            rgb.g = 0;
            rgb.b = 0;

            matrix1.set(i, j, rgb);
        }
    }

    for (int y = 0; y < matrix1.getHeight(); y++) {
        for (int x = 0; x < matrix1.getWidth(); x++) {
            int srcX = (x - centerX) * cosA + (y - centerY) * sinA + centerX;
            int srcY = -(x - centerX) * sinA + (y - centerY) * cosA + centerY;

            if (srcX >= 0 && srcX < matrix.getWidth() && srcY >= 0 && srcY < matrix.getHeight()){
                int x1 = floor(srcX);
                int y1 = floor(srcY);
                int x2 = ceil(srcX);
                int y2 = ceil(srcY);
                RGB q11 =  matrix.get(y1,x1);
                RGB q12 = matrix.get(y1,x2);
                RGB q21 = matrix.get(y2,x1);
                RGB q22 = matrix.get(y2,x2);

                double fx = srcX - x1;
                double fy = srcY - y1;
                double w1 = (1 - fx) * (1 - fy);
                double w2 = fx * (1 - fy);
                double w3 = (1 - fx) * fy;
                double w4 = fx * fy;
                RGB rgb;
                rgb.r = round(w1 * q11.r + w2 * q12.r + w3 * q21.r + w4 * q22.r);
                rgb.g = round(w1 * q11.g + w2 * q12.g + w3 * q21.g + w4 * q22.g);
                rgb.b = round(w1 * q11.b + w2 * q12.b + w3 * q21.b + w4 * q22.b);

                matrix1.set(y,x,rgb);

            }
        }

    }

    return matrix1;

}


int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "ru");

    Image image;
    vector<IFD> ifdArray;

    Matrix matrix = readTiff(argv[1], image, ifdArray);

    Matrix matrix1 = rotateImage(matrix, 45, image);

    writeTiff(matrix1, argv[2], image, ifdArray);

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