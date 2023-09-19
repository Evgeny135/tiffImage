#include "Matrix.h"
#include <iostream>
#include <fstream>
#include "vector"

struct RGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

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


struct Image {
    unsigned int offsetIfd = 1;
    unsigned int width = 1;
    unsigned int height = 1;
    unsigned int offsetImage = 8;
    unsigned int dataImageCount = 1;
    unsigned int offsetBitPerSamples = 1;
};

void outTag(std::ofstream &out,unsigned int id, unsigned int type, unsigned int length, unsigned int value){
    char buffer[4];
    parseBit(buffer, id);
    out.write(buffer,2);

    parseBit(buffer,type);
    out.write(buffer,2);

    parseBit(buffer,length);
    out.write(buffer,4);

    parseBit(buffer,value);
    out.write(buffer,4);
}

Matrix<RGB> readTiff(const char *filePath, Image &image) {

    std::ifstream in(filePath, std::ios::binary);
    char buffer[4];

    in.seekg(4);


    in.read(buffer, 4);
    image.offsetIfd = getBit(buffer);

    in.seekg(image.offsetIfd);
    in.read(buffer, 2);

    in.seekg(8,std::ios::cur);
    in.read(buffer,4);
    image.width = getBit(buffer,4);

    in.seekg(8,std::ios::cur);
    in.read(buffer,4);
    image.height = getBit(buffer,4);

    image.dataImageCount = image.height*image.width*3;
    image.offsetBitPerSamples = (8 + image.dataImageCount + 9 * 12) + 12;
    image.offsetIfd = image.height * image.width * 3 + 8;

    Matrix<RGB> matrix(image.width,image.height);

    char *imageData = new char[image.dataImageCount];

    in.seekg(image.offsetImage);
    in.read(imageData, image.dataImageCount);
    getBit(imageData, image.dataImageCount);

    int k = -1;
    for (int i = 0; i < image.height; i++) {
        for (int j = 0; j < image.width; j++) {
            RGB rgb;
            rgb.r=imageData[++k];
            rgb.g=imageData[++k];
            rgb.b=imageData[++k];
            matrix.set(i,j,rgb);
        }
    }
    return matrix;
}

void writeTiff(Matrix<RGB> &matrix, const char *filePath, struct Image image) {
    std::ofstream out(filePath, std::ios::binary);
    char buffer[4];
    char *imageData = new char[image.dataImageCount];

    parseBit(buffer, 2771273);
    out.write(buffer, 4);

    parseBit(buffer, image.offsetIfd);
    out.write(buffer, 4);

    int k = -1;
    for (int i = 0; i < matrix.getHeight(); i++) {
        for (int j = 0; j < matrix.getWidth(); j++) {
                RGB el = matrix.get(i, j);
				imageData[++k] = el.r;
				imageData[++k] = el.g;
				imageData[++k] = el.b;
        }
    }

    out.write(imageData, image.dataImageCount);

    parseBit(buffer, 8);
    out.write(buffer, 2);

    outTag(out,256,3,1,image.width); // Ширина
    outTag(out,257,3,1,image.height); // Высота
    outTag(out,258,3,3,image.offsetBitPerSamples); // бит на пиксель
    outTag(out,259,3,1,1); // сжатие
    outTag(out,262,3,1,2);
    outTag(out,273,4,1,8);
    outTag(out,277,3,1,3);
    outTag(out,279,4,1,image.dataImageCount);

    out.seekp(image.offsetBitPerSamples);
    parseBit(buffer, 8);
    out.write(buffer, 2);
    parseBit(buffer, 8);
    out.write(buffer, 2);
    parseBit(buffer, 8);
    out.write(buffer, 2);
}