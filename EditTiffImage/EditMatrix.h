#include "Matrix.h"

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

RGB interpolation(const Matrix& matrix,struct Point point){
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

Matrix cropImage(struct Image &image, int width, int height, Matrix &matrix) {

    if (width > image.width) {
        width = image.width;
    }
    if (height > image.height) {
        height = image.height;
    }
    Matrix cropMatrix(width, height);

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

Matrix convertToGrayScale(Matrix &matrix) {
    Matrix greyMatrix(matrix.getWidth(), matrix.getHeight());

    //greyMatrix.setSize(greyMatrix.getWidth() * greyMatrix.getHeight());

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

Matrix rotateImage(Matrix &matrix, int angle) {

    double radians = angle * M_PI / 180;

    double sinA = sin(radians);
    double cosA = cos(radians);

    int centerX = matrix.getWidth() / 2;
    int centerY = matrix.getHeight() / 2;

    Matrix matrix1(matrix.getWidth(), matrix.getHeight());

    const RGB rgb{0,0,0};

    for (int i = 0; i < matrix1.getHeight(); i++) {
        for (int j = 0; j < matrix1.getWidth(); j++) {

            matrix1.set(i, j, rgb);
        }
    }
    double rotateMatrix[6]  = {cosA,sinA,static_cast<double>(centerX),-sinA,cosA,static_cast<double>(centerY)};

    for (int y = 0; y < matrix1.getHeight(); y++) {
        for (int x = 0; x < matrix1.getWidth(); x++) {
            Point point= getCoordinate(rotateMatrix,x-centerX,y-centerY);
            if (point.x >= 0 && point.x < matrix.getWidth() && point.y >= 0 && point.y < matrix.getHeight()) {
                RGB rgb1 = interpolation(matrix,point);

                matrix1.set(y, x, rgb1);
            }
        }
    }
    return matrix1;
}


Matrix scaleImage(const Matrix &matrix, double x, double y) {
    double matrixScale[9] = {1/x, 0, 0,0, 1/y,0,0,0,1};
    Matrix scaleMatrix(matrix.getWidth(), matrix.getHeight());

    const RGB rgb{0,0,0};
    for (int i = 0; i < scaleMatrix.getHeight(); i++) {
        for (int j = 0; j < scaleMatrix.getWidth(); j++) {
            scaleMatrix.set(i, j, rgb);
        }
    }

    for (int j = 0; j < matrix.getHeight(); j++) {
        for (int i = 0; i < matrix.getWidth(); i++) {
            Point point = getCoordinate(matrixScale,i,j);

            if (point.x >= 0 && point.x < matrix.getWidth() && point.y >= 0 && point.y < matrix.getHeight()) {
                RGB rgb1 = interpolation(matrix,point);

                scaleMatrix.set(j, i, rgb1);
            }
        }
    }

    return scaleMatrix;
}

Matrix offsetImage(Matrix &matrix, int x, int y) {
    double offsetMatrix[9] = {1, 0, static_cast<double>(x), 0, 1, static_cast<double>(y), 0, 0, 1};

    Matrix matrixOffset(matrix.getWidth(), matrix.getHeight());

    const RGB rgb{0,0,0};

    for (int i = 0; i < matrixOffset.getHeight(); i++) {
        for (int j = 0; j < matrixOffset.getWidth(); j++) {
            matrixOffset.set(i, j, rgb);
        }
    }

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