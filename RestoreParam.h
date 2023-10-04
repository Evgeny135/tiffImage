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

void getKeyPoint(Matrix<RGB>& matrix){

    cv::Mat rgbMat = fillMat(matrix);
    cv::Mat grayImage;
    cv::cvtColor(rgbMat, grayImage, cv::COLOR_BGR2GRAY);

    cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create();

    std::vector<cv::KeyPoint> keypoints;
    detector->detect(grayImage, keypoints);

    for (int i = 0; i < 3; i++)
    {
        std::cout << "Point " << i + 1 << ": " << keypoints[i].pt << std::endl;
    }

    for (int i = 0; i < 3; i++)
    {
        cv::KeyPoint keypoint = keypoints[i];
        cv::Point center(keypoint.pt.x, keypoint.pt.y);
        int radius = cvRound(keypoint.size * 1.2 / 9.0);
        cv::Scalar color(0, 0, 255);
        int thickness = 1;
        int lineType = cv::LINE_8;

        circle(rgbMat, center, radius, color, thickness, lineType);
    }

    cv::imshow("Wn",rgbMat);
    cv::waitKey(0);
}
