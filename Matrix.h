#pragma once
#include <vector>
#include <iostream>

//struct RGB {
//	uint8_t r;
//	uint8_t g;
//	uint8_t b;
//};

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
//	void setPixel(const char* mass) {
//		int k = -1;
//        int size = this->width* this->height;
//        for (int i = 0; i< size; i++)
//			{
//				RGB rgb;
//				rgb.r = mass[++k];
//				rgb.g = mass[++k];
//				rgb.b = mass[++k];
//				imageMatrix.push_back(rgb);
//			}
//	}

//    void fill(){
//        const RGB rgb{0,0,0};
//
//        for (int i = 0; i < this->height; i++) {
//            for (int j = 0; j < this->width; j++) {
//
//                this->set(i, j, rgb);
//            }
//        }
//    }

//	void getImageDataFromMatrix(char* imageData){
//		int k = -1;
//		for (int i = 0; i < height*width; i++)
//		{
//				imageData[++k] = imageMatrix[i].r;
//				imageData[++k] = imageMatrix[i].g;
//				imageData[++k] = imageMatrix[i].b;
//		}
//	}

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
