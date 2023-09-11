#include "cmath"
#include "Matrix.h"
#include "algorithm"

struct Point{
    float x;
    float y;
};


Point getCoordinate(const double* matrix, double x, double y){
    Point point;
    point.x = matrix[0]*x + matrix[1]*y + matrix[2];
    point.y = matrix[3]*x + matrix[4]*y + matrix[5];

    return point;
}

void fillMatrix(Matrix<RGB>& matrix){
    RGB rgb {0,0,0};
    for (int i = 0; i < matrix.getHeight(); i++) {
            for (int j = 0; j < matrix.getWidth(); j++) {

                matrix.set(i, j, rgb);
            }
        }
}

RGB interpolation(const Matrix<RGB>& matrix,struct Point point){
    if (matrix.getWidth()*matrix.getHeight()==1) {
        return matrix.get(0,0);
    }
    int x1 = (int) point.x;
    int y1 = (int) point.y;
    int x2 = x1 + 1;
    int y2 = y1 + 1;
    float fx = point.x - x1;
    float fy = point.y - y1;

    RGB q11 = matrix.get(y1, x1);
    RGB q12 = matrix.get(y1, x2);
    RGB q21 = matrix.get(y2, x1);
    RGB q22 = matrix.get(y2, x2);

    double w1 = (1 - fx) * (1 - fy);
    double w2 = fx * (1 - fy);
    double w3 = (1 - fx) * fy;
    double w4 = fx * fy;
    RGB rgb;
    rgb.r = round(w1 * q11.r + w2 * q12.r + w3 * q21.r + w4 * q22.r);
    rgb.g = round(w1 * q11.g + w2 * q12.g + w3 * q21.g + w4 * q22.g);
    rgb.b = round(w1 * q11.b + w2 * q12.b + w3 * q21.b + w4 * q22.b);

    return rgb;
}

Matrix<RGB> transformMatrix(const double* transofrmMatrix,const Matrix<RGB>& matrix, int centerX, int centerY){
    Matrix<RGB> matrix1(matrix.getWidth(), matrix.getHeight());

    fillMatrix(matrix1);

    for (int y = 0; y < matrix1.getHeight(); y++) {
        for (int x = 0; x < matrix1.getWidth(); x++) {
            Point point= getCoordinate(transofrmMatrix,x-centerX,y-centerY);
            if (point.x >= 0 && point.x < matrix1.getWidth() && point.y >= 0 && point.y < matrix1.getHeight()) {
                RGB rgb1 = interpolation(matrix,point);

                matrix1.set(y, x, rgb1);
            }
        }
    }

    return matrix1;
}

Matrix<RGB> cropImage(struct Image &image, unsigned int width, unsigned int height, Matrix<RGB> &matrix) {

    width = std::min(width,image.width);

    height= std::min(height,image.height);

    Matrix<RGB> cropMatrix(width, height);

    image.width = width;
    image.height = height;

    image.offsetIfd = image.width * image.height * 3 + 8;
    image.dataImageCount = image.width * image.height * 3;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            cropMatrix.set(i, j, matrix.get(i, j));
        }
    }
    return cropMatrix;
}

Matrix<RGB> convertToGrayScale(Matrix<RGB> &matrix) {
    Matrix<RGB> greyMatrix(matrix.getWidth(), matrix.getHeight());

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

Matrix<RGB> rotateImage(Matrix<RGB> &matrix, int angle) {

    double radians = angle * M_PI / 180;

    double sinA = sin(radians);
    double cosA = cos(radians);

    int centerX = matrix.getWidth() / 2;
    int centerY = matrix.getHeight() / 2;

    double rotateMatrix[6]  = {cosA,sinA,static_cast<double>(centerX),-sinA,cosA,static_cast<double>(centerY)};

    return transformMatrix(rotateMatrix, matrix, centerX, centerY);
}


Matrix<RGB> scaleImage(const Matrix<RGB> &matrix, double x, double y) {
    double matrixScale[9] = {1/x, 0, 0,0, 1/y,0,0,0,1};

    return transformMatrix(matrixScale,matrix,0,0);
}

Matrix<RGB> offsetImage(Matrix<RGB> &matrix, int x, int y) {
    double offsetMatrix[9] = {1, 0, static_cast<double>(x), 0, 1, static_cast<double>(y), 0, 0, 1};

    Matrix<RGB> matrixOffset(matrix.getWidth(), matrix.getHeight());

    fillMatrix(matrixOffset);

    for (int y = 0; y < matrixOffset.getHeight(); y++) {
        for (int x = 0; x < matrixOffset.getWidth(); x++) {
            Point point = getCoordinate(offsetMatrix, x, y);

            int newX = point.x;
            int newY = point.y;

            if (newX < 0 || newX >= matrix.getWidth() || newY < 0 || newY >= matrix.getHeight()){
                continue;
            }

            matrixOffset.set(newY,newX,matrix.get(y,x));
        }
    }
    return matrixOffset;
}