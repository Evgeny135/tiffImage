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
//Image image;

class Matrix {

private:
	int width,height;
	vector<vector<RGB>> imageMatrix;
	Image image;
public:

	Matrix(unsigned int width, unsigned int height) {
		this->width = width;
		this->height = height;
	}

	Matrix(){}

	Matrix cropImage(int width, int height) {
		if (width>this->image.width)
		{
			width = this->image.width;
		}
		if (height>this->image.height)
		{
			height = this->image.height;
		}
		Matrix cropMatrix(width,height);
		

		cropMatrix.image = this->image;

		cropMatrix.image.width = width;
		cropMatrix.image.height = height;

		cropMatrix.image.offsetIfd = cropMatrix.image.width * cropMatrix.image.height * 3+8;
		cropMatrix.image.dataImageCount = cropMatrix.image.width * cropMatrix.image.height * 3 ;

		for (int i = 0; i <height; i++)
		{
			vector<RGB> row;
			for (int j = 0; j < width; j++)
			{
				row.push_back(this->imageMatrix[i][j]);
			}
			cropMatrix.imageMatrix.push_back(row);
		}

		return cropMatrix;

	}

	Matrix readTiff(const char* filePath) {

		ifstream in(filePath, ios::binary);
		char buffer[4];

		in.seekg(4);

		Matrix matrix;

		in.read(buffer, 4);
		matrix.image.offsetIfd = getBit(buffer);

		in.seekg(matrix.image.offsetIfd);
		in.read(buffer, 2);

		matrix.image.countIfd = getBit(buffer, 2);

		for (int i = 0; i < matrix.image.countIfd; i++)
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
				matrix.image.width = ifd.value;
			}
			if (ifd.tagId == 257) {
				matrix.image.height = ifd.value;
			}
			if (ifd.tagId == 258)
			{
				ifd.value = 8;
			}
			if (ifd.tagId == 279)
			{
				matrix.image.dataImageCount = ifd.value;
			}
			if (ifd.tagId == 273) {
				matrix.image.offsetImage = ifd.value;
			}
			/*if (ifd.tagId == 256 ||ifd.tagId == 257 || ifd.tagId == 258 || ifd.tagId == 259
				|| ifd.tagId == 262 || ifd.tagId == 273 || ifd.tagId == 277 || ifd.tagId == 279) {*/
				//ifdArray.push_back();
			ifdArray.push_back(ifd);
			//}
			//image.ifdArray[i].toString();
		}

		int offsetBitPerSamples = 8 + matrix.image.dataImageCount + 9 * 12;
		matrix.image.offsetBitPerSamples = offsetBitPerSamples;

		//Matrix matrix(image.width, image.height);

		matrix.width = matrix.image.width;
		matrix.height = matrix.image.height;

		char* imageData = new char[matrix.image.dataImageCount];

		in.seekg(matrix.image.offsetImage);
		in.read(imageData, matrix.image.dataImageCount);
		getBit(imageData, matrix.image.dataImageCount);

		matrix.setPixel(imageData);


		return matrix;

	}

	void writeTiff(Matrix& matrix,const char* filePath) {
		ofstream out(filePath, ios::binary);
		char buffer[4];
		char* imageData = new char[matrix.image.dataImageCount];

		parseBit(buffer, 2771273);
		out.write(buffer,4);
		
		parseBit(buffer, matrix.image.offsetIfd);
		out.write(buffer,4);
		
		matrix.getImageMatrix(imageData);

		out.seekp(matrix.image.offsetImage);
		parseBit(imageData, matrix.image.dataImageCount);

		out.write(imageData, matrix.image.dataImageCount);

		parseBit(buffer, 9);
		out.write(buffer, 2);
		for (int i = 0; i < matrix.image.countIfd; i++)
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
					parseBit(buffer, matrix.image.offsetBitPerSamples);
					out.write(buffer, 4);
				}
				else if(ifdArray[i].tagId==256) {
					parseBit(buffer,matrix.image.width);
					out.write(buffer, 4);
				}
				else if (ifdArray[i].tagId==257)
				{
					parseBit(buffer, matrix.image.height);
					out.write(buffer, 4);
				}
				else {
					parseBit(buffer, ifdArray[i].value);
					out.write(buffer, 4);
				}
			}

		}

		out.seekp(matrix.image.offsetBitPerSamples);
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