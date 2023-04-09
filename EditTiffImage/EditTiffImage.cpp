#include <iostream>
#include <fstream>
#include "Matrix.h"
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "ru");

    Matrix matrix = matrix.readTiff(argv[1]);
	
	Matrix cropMatrix = matrix.cropImage(stoi(argv[4]), stoi(argv[5]));

	cropMatrix.writeTiff(cropMatrix,argv[2]);
	

}