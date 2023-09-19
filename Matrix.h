#pragma once
#include <vector>
#include <iostream>

template <typename T>
class Matrix {

private:
	int width =0,height=0;
	std::vector<T> imageMatrix;
public:
    void set(unsigned int row,unsigned int column, const T& v)
    {
        imageMatrix[row*width+column] = v;
    }

    const T& get(unsigned int row,unsigned int column) const{
        return imageMatrix[row*width+column];
    }

	Matrix(unsigned int width, unsigned int height) {
		this->width = width;
		this->height = height;
        imageMatrix.resize(width*height);
	}

	Matrix(){}

    int getWidth() const {
        return width;
    }

    int getHeight() const {
        return height;
    }
};
