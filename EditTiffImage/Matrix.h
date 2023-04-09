#pragma once
#include <vector>
#include <iostream>
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

struct IFD {
	unsigned int tagId;
	unsigned int typeTag;
	unsigned int length;
	unsigned int offset;
	unsigned int value;

	/*void toString() {
		cout << "tagId: " << this->tagId << " typeId: " << this->typeTag << " length: " << this->length << " offset: " << this->offset << " value: " << this->value << endl;
	}*/
};

struct Image {
	char startImage[4];
	unsigned int offsetIfd;
	unsigned int countIfd;
	unsigned int width;
	unsigned int height;
	unsigned int offsetImage;
	unsigned int dataImageCount;
	unsigned int offsetBitPerSamples;
};

struct RGB {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

vector<IFD> ifdArray;
Image image;

class Matrix {

private:
	int width,height;
	vector<vector<RGB>> imageMatrix;

public:

	Matrix(unsigned int width, unsigned int height) {
		this->width = width;
		this->height = height;
	}

	Matrix(){}

	Matrix readTiff(const char* filePath) {

		ifstream in(filePath, ios::binary);
		char buffer[4];


		in.read(image.startImage, 4);

		in.read(buffer, 4);
		image.offsetIfd = getBit(buffer);

		in.seekg(image.offsetIfd);
		in.read(buffer, 2);

		image.countIfd = getBit(buffer, 2);

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
			if (ifd.tagId == 279)
			{
				image.dataImageCount = ifd.value;
			}
			if (ifd.tagId == 273) {
				image.offsetImage = ifd.value;
			}
			/*if (ifd.tagId == 256 ||ifd.tagId == 257 || ifd.tagId == 258 || ifd.tagId == 259
				|| ifd.tagId == 262 || ifd.tagId == 273 || ifd.tagId == 277 || ifd.tagId == 279) {*/
				//ifdArray.push_back();
			ifdArray.push_back(ifd);
			//}
			//image.ifdArray[i].toString();
		}

		int offsetBitPerSamples = 8 + image.dataImageCount + 9 * 12;
		image.offsetBitPerSamples = offsetBitPerSamples;

		Matrix matrix(image.width, image.height);

		char* imageData = new char[image.dataImageCount];

		in.seekg(image.offsetImage);
		in.read(imageData, image.dataImageCount);
		getBit(imageData, image.dataImageCount);

		matrix.setPixel(imageData);


		return matrix;

	}

	void writeTiff(Matrix& matrix,const char* filePath) {
		ofstream out(filePath, ios::binary);
		char buffer[4];
		char* imageData = new char[image.dataImageCount];

		parseBit(buffer, 2771273);
		out.write(buffer,4);
		
		parseBit(buffer, image.offsetIfd);
		out.write(buffer,4);
		
		matrix.getImageMatrix(imageData);

		out.seekp(image.offsetImage);
		parseBit(imageData, image.dataImageCount);

		out.write(imageData, image.dataImageCount);

		parseBit(buffer, 9);
		out.write(buffer, 2);
		for (int i = 0; i < image.countIfd; i++)
		{
			if (ifdArray[i].tagId == 256 || ifdArray[i].tagId == 257 || ifdArray[i].tagId == 258 || ifdArray[i].tagId == 259
				|| ifdArray[i].tagId == 262 || ifdArray[i].tagId == 273 || ifdArray[i].tagId == 277 || ifdArray[i].tagId == 279)
			{
				parseBit(buffer, ifdArray[i].tagId);
				out.write(buffer, 2);
				parseBit(buffer, ifdArray[i].typeTag);
				out.write(buffer, 2);
				parseBit(buffer, ifdArray[i].length);
				out.write(buffer, 4);
				if (ifdArray[i].tagId == 258)
				{
					parseBit(buffer, image.offsetBitPerSamples);
					out.write(buffer, 4);
				}
				else {
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

	void setPixel(char* mass) {
		int k = -1;
		for (int i = 0; i < this->height; i++)
		{
			vector<RGB> row;
			for (int j = 0; j < this->width; j++)
			{
				RGB rgb;
				rgb.r = mass[++k];
				rgb.g = mass[++k];
				rgb.b = mass[++k];
				row.push_back(rgb);
			}
			imageMatrix.push_back(row);
		}
	}

	void getImageMatrix(char* imageData) {
		int k = -1;
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				imageData[++k] = imageMatrix[i][j].r;
				imageData[++k] = imageMatrix[i][j].g;
				imageData[++k] = imageMatrix[i][j].b;
			}
		}

	}

};