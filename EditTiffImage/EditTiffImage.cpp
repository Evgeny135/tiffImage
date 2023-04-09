#include <iostream>
#include <fstream>
#include "Matrix.h"

using namespace std;

int main(int argc, char* argv[])
{

    Matrix matrix = matrix.readTiff(argv[1]);

    matrix.writeTiff(matrix, argv[2]);

}