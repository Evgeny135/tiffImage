#include <iostream>
#include <fstream>
#include "Matrix.h"

using namespace std;

unsigned int getBit(char* buffer, int length = 4) {
    unsigned int size = 0;

    for (int i = 0; i < length; i++)
    {
        size += static_cast<uint32_t>(static_cast<uint8_t>(buffer[i])) << 8 * i;
    }
    return size;
}

void parseBit(char* buffer, unsigned int number) {
    //char buffer[4];
    for (int i = 0; i < 4; i++)
    {
        buffer[i] = number >> 8 * i;
    }
    //return buffer;
}

struct Image {
    char startImage[4];
    unsigned int offsetIfd;
    unsigned int countIfd;
    unsigned int width;
    unsigned int height;
    unsigned int offsetImage;
    unsigned int dataImageCount;
};

struct IFD {
    unsigned int tagId;
    unsigned int typeTag;
    unsigned int length;
    unsigned int offset;
    unsigned int value;

    void toString() {
        cout << "tagId: " << this->tagId << " typeId: " << this->typeTag << " length: " << this->length << " offset: " << this->offset << " value: " << this->value << endl;
    }
};


int main(int argc, char* argv[])
{
    Image image;

    ifstream in(argv[1], ios::binary);
    ofstream out(argv[2], ios::binary);

    char buffer[4];

    in.read(image.startImage, 4);

    in.read(buffer, 4);
    image.offsetIfd = getBit(buffer);

    in.seekg(image.offsetIfd);
    in.read(buffer, 2);

    image.countIfd = getBit(buffer, 2);
    IFD* ifdArray = new IFD[image.countIfd];
 
    for (int i = 0; i < image.countIfd; i++)
    {
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

        if (ifd.tagId == 256)
        {
            image.width = ifd.value;
        }
        if (ifd.tagId == 257) {
            image.height = ifd.value;
        }
        if (ifd.tagId == 258)
        {
            ifd.value = 8;
        }
        if (ifd.tagId==279)
        {
            image.dataImageCount = ifd.value;
        }
        if (ifd.tagId == 273) {
            image.offsetImage = ifd.value;
        }

        ifdArray[i] = ifd;
        ifdArray[i].toString();
    }

    int offsetBitPerSamples = 8 + image.dataImageCount + 9 * 12;


    char dataImage[3];
    out.write(image.startImage, 4);
    parseBit(buffer, image.offsetIfd);
    out.write(buffer, 4);

    Matrix matrix(image.width,image.height);


    char* imageData = new char[image.dataImageCount];

    in.seekg(image.offsetImage);
    in.read(imageData, image.dataImageCount);
    getBit(imageData, image.dataImageCount);
    
    matrix.setPixel(imageData);

    matrix.getImageMatrix(imageData);

    out.seekp(image.offsetImage);
    parseBit(imageData, image.dataImageCount);

    out.write(imageData, image.dataImageCount);

    parseBit(buffer, 9);
    out.write(buffer, 2);
    for (int i = 0; i < image.countIfd; i++)
    {
        if (ifdArray[i].tagId==256 || ifdArray[i].tagId == 257 || ifdArray[i].tagId == 258 || ifdArray[i].tagId==259
            || ifdArray[i].tagId==262 || ifdArray[i].tagId==273 || ifdArray[i].tagId == 277 || ifdArray[i].tagId == 279)
        {
            parseBit(buffer, ifdArray[i].tagId);
            out.write(buffer, 2);
            parseBit(buffer, ifdArray[i].typeTag);
            out.write(buffer, 2);
            parseBit(buffer, ifdArray[i].length);
            out.write(buffer, 4);
            if (ifdArray[i].tagId==258)
            {
                parseBit(buffer, offsetBitPerSamples);
                out.write(buffer, 4);
            }
            else {
                parseBit(buffer, ifdArray[i].value);
                out.write(buffer, 4);
            }
        }

    }
  
    out.seekp(offsetBitPerSamples);
    parseBit(buffer, 8);
    out.write(buffer, 2);
    parseBit(buffer, 8);
    out.write(buffer, 2);
    parseBit(buffer, 8);
    out.write(buffer, 2);
}