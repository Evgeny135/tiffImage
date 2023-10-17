#include "Matrix.h"
#include "opencv2/opencv.hpp"
#include "opencv2/xfeatures2d.hpp"

cv::Mat fillMat(Matrix<RGB>& matrix){
    cv::Mat mat(matrix.getWidth(), matrix.getHeight(), CV_8UC3);
    for (int i = 0; i < matrix.getHeight(); i++) {
        for (int j = 0; j < matrix.getWidth(); j++) {
            RGB rgb = matrix.get(i,j);

            mat.at<cv::Vec3b>(i, j) = cv::Vec3b(matrix.get(i,j).b, matrix.get(i,j).g, matrix.get(i,j).r);
        }
    }
    return mat;
}

void getKeyPoint(Matrix<RGB>& startMatrix, Matrix<RGB>& rotateMatrix){

    cv::Mat startMat = fillMat(startMatrix);
    cv::Mat rotateMat = fillMat(rotateMatrix);

    cv::Mat startGrayMat;
    cv::Mat rotateGrayMat;

    cv::cvtColor(startMat, startGrayMat, cv::COLOR_BGR2GRAY);
    cv::cvtColor(rotateMat, rotateGrayMat, cv::COLOR_BGR2GRAY);

    cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create();
    cv::Ptr<cv::xfeatures2d::SURF> extractor = cv::xfeatures2d::SURF::create();

    std::vector<cv::KeyPoint> keypointsStart;
    std::vector<cv::KeyPoint> keypointsRotate;

    cv::Mat descriptorsStart;
    cv::Mat descriptorsRotate;

    detector->detectAndCompute(startGrayMat,cv::Mat(), keypointsStart,descriptorsStart);
    detector->detectAndCompute(rotateGrayMat,cv::Mat(), keypointsRotate,descriptorsRotate);

    cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
    std::vector<cv::DMatch> matches;

    matcher->match(descriptorsStart,descriptorsRotate,matches);

    std::vector<cv::Point2f> srcPoints;
    std::vector<cv::Point2f> rotatedPoints;

    for (int i = 0; i < matches.size(); i++)
    {
        srcPoints.push_back(keypointsStart[matches[i].queryIdx].pt);
        rotatedPoints.push_back(keypointsRotate[matches[i].trainIdx].pt);
    }

    cv::Mat homographyMatrix = cv::findHomography(srcPoints, rotatedPoints, cv::RANSAC);

    double angle = atan2(homographyMatrix.at<double>(1, 0), homographyMatrix.at<double>(0, 0)) * 180.0 / CV_PI;
    cv::Point2f shift(homographyMatrix.at<double>(0, 2), homographyMatrix.at<double>(1, 2));

    std::cout<< "Угол поворота: "<< angle << " Смещение x y: "<<shift.x<< " "<< shift.y<<std::endl;
}
