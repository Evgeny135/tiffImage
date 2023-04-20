#pragma once
#include <vector>
#include <iostream>
using namespace std;


struct RGB {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

class Matrix {

private:
	int width =0,height=0;
	vector<vector<RGB>> imageMatrix;
//	Image image;
public:


    void addRowImageMatrix(vector<RGB> &imageMatrix){
        Matrix::imageMatrix.push_back(imageMatrix);
    }

	void setPixel(const char* mass) {
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

	void getImageDataFromMatrix(char* imageData){
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

	Matrix(unsigned int width, unsigned int height) {
		this->width = width;
		this->height = height;
	}

	Matrix(){}

    int getWidth() const {
        return width;
    }

    void setWidth(int width) {
        Matrix::width = width;
    }

    int getHeight() const {
        return height;
    }

    void setHeight(int height) {
        Matrix::height = height;
    }

    const vector<vector<RGB>> &getImageMatrix() const {
        return imageMatrix;
    }
};
