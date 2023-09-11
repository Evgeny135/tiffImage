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

    void addElement(T el){
        imageMatrix.push_back(el);
    }

    T getElementByIndex(int index){
        return imageMatrix[index];
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

    void setWidth(int width) {
        Matrix::width = width;
    }

    int getHeight() const {
        return height;
    }

    void setHeight(int height) {
        Matrix::height = height;
    }

    const std::vector<T> &getImageMatrix() const {
        return imageMatrix;
    }
};
