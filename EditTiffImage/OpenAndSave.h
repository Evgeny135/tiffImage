#include "Matrix.h"
#include <iostream>
#include <fstream>
#include "vector"

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

Matrix readTiff(const char *filePath, Image &image, std::vector<IFD> &ifdArray) {

    std::ifstream in(filePath, std::ios::binary);
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

void writeTiff(Matrix &matrix, const char *filePath, struct Image image, std::vector<IFD> &ifdArray) {
    std::ofstream out(filePath, std::ios::binary);
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