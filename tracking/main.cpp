#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

cv::Mat frame;
cv::Mat mask;

const char* src_window = "Select ROI";
const char* sift_window = "SIFT";

int drag = 0, select_flag = 0;

cv::Point point1, point2;
bool callback = false;

// for SIFT
cv::SiftFeatureDetector detector;
std::vector<cv::KeyPoint> keypoints;

// for color histogram
vector<Mat> bgr_planes;
int histSize = 256;
float range[] = {0,256};
const float* histRange = {range};
bool uniform = true;
bool collect = false;
Mat b_hist, g_hist, r_hist;

void mouseHandler(int event, int x, int y, int flags, void* param);

int main(int argc, char** argv)
{
    cv::VideoCapture cap("carchase.mp4");

    if( !cap.isOpened() )
    {
        std::cout << "Could not initialize capturing...\n";
        return 0;
    }
    
    cv::namedWindow(src_window,CV_WINDOW_AUTOSIZE);
    cv::namedWindow(sift_window,CV_WINDOW_AUTOSIZE);
    cv::setMouseCallback(src_window,mouseHandler,0);
    
    for(;;){
        cap >> frame;
//    frame = imread("car.jpg");
        mask = Mat::zeros(frame.size(),CV_8U);
        split(frame, bgr_planes);
        cv::imshow(src_window,frame);
        if(waitKey(30)>=0) break;
    }
//    for (;;)
//    {
//        if(callback)
//        {
//            cap >> frame;
//            if( frame.empty() )
//                    break;
//            cv::imshow(src_window,frame);
//        }
//
//        cv::waitKey(5);
//    }

    return 0;
}

void mouseHandler(int event, int x, int y, int flags, void* param)
{
    if (event == CV_EVENT_LBUTTONDOWN && !drag && !select_flag)
    {
        /* left button clicked. ROI selection begins */
        point1 = cv::Point(x, y);
        drag = 1;
    }

    if (event == CV_EVENT_MOUSEMOVE && drag && !select_flag)
    {
        /* mouse dragged. ROI being selected */
        cv::Mat img1 = frame.clone();
        point2 = cv::Point(x, y);
        cv::rectangle(img1, point1, point2, CV_RGB(255, 0, 0), 1, 8, 0);
        
        cv::imshow(src_window, img1);
    }

    if (event == CV_EVENT_LBUTTONUP && drag && !select_flag)
    {
        cv::Mat img2 = frame.clone();

        point2 = cv::Point(x, y);
        drag = 0;
        select_flag = 1;

        cout << "point1  " << point1.x << "  " << point1.y << endl;
        cout << "point2  " << point2.x << "  " << point2.y << endl;
        
        Mat roi(mask, cv::Rect(point1.x, point1.y, (point2.x-point1.x), (point2.y-point1.y)));
        roi = Scalar(255, 255, 255);
//        namedWindow("mask");
//        namedWindow("roi");
//        imshow("mask",mask);
//        imshow("roi",roi);
        detector.detect(img2,keypoints,mask);
        
        Mat roi2(img2, cv::Rect(point1.x, point1.y, (point2.x-point1.x), (point2.y-point1.y)));
//        namedWindow("roi2");
//        imshow("roi2",roi2);        
        split(roi2, bgr_planes);
        
        /// Compute the histograms:
        calcHist( &bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, collect );
        calcHist( &bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, collect );
        calcHist( &bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, collect );
      
        // Draw the histograms for B, G and R
        int hist_w = 512; int hist_h = 400;
        int bin_w = cvRound( (double) hist_w/histSize );
      
        Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
      
        /// Normalize the result to [ 0, histImage.rows ]
        normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
        normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
        normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
      
        /// Draw for each channel
        for( int i = 1; i < histSize; i++ )
        {
            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
                             Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
                             Scalar( 255, 0, 0), 2, 8, 0  );
            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
                             Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
                             Scalar( 0, 255, 0), 2, 8, 0  );
            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
                             Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
                             Scalar( 0, 0, 255), 2, 8, 0  );
        }
      
        /// Display
        namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE );
        imshow("calcHist Demo", histImage );
        
        cv::rectangle(img2, point1, point2, CV_RGB(255, 0, 0), 1, 8, 0);
        cv::drawKeypoints(img2, keypoints, img2);
        cv::imshow(sift_window, img2);

//        	callback = true;
    }
}
