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

    for (int i = 0; i < 3; i++)
    {
        cv::Point2f sourcePoint = keypointsStart[matches[i].queryIdx].pt;
        cv::Point2f rotatedPoint = keypointsRotate[matches[i].trainIdx].pt;

        std::cout << "Исходные координаты: " << sourcePoint << ", Повернуто-сдвинутые координаты: " << rotatedPoint << std::endl;
    }


    for (int i = 0; i < 3; i++)
    {
        cv::KeyPoint keypoint = keypointsStart[matches[i].queryIdx];
        cv::Point center(keypoint.pt.x, keypoint.pt.y);
        int radius = cvRound(keypoint.size * 1.2 / 9.0);
        cv::Scalar color(0, 255, 0);
        int thickness = 2;
        int lineType = cv::LINE_8;

        circle(startGrayMat, center, radius, color, thickness, lineType);
    }

    cv::imshow("Wn",startGrayMat);
    cv::waitKey(0);

    for (int i = 0; i < 3; i++)
    {
        cv::KeyPoint keypoint = keypointsRotate[matches[i].trainIdx];
        cv::Point center(keypoint.pt.x, keypoint.pt.y);
        int radius = cvRound(keypoint.size * 1.2 / 9.0);
        cv::Scalar color(0, 255, 0);
        int thickness = 2;
        int lineType = cv::LINE_8;

        circle(rotateGrayMat, center, radius, color, thickness, lineType);
    }

    cv::imshow("Wn",rotateGrayMat);
    cv::waitKey(0);
}
