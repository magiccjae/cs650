#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/video.hpp"

using namespace cv;
using namespace std;

Mat frame;
Mat hsv;
Mat hue;
Mat mask;

Point point1, point2;

const char* src_window = "Select ROI";

int drag = 0;
bool setup = false;

Rect rect_roi;

// histogram stuff
MatND hist;
float h_range[] = {0, 179};
float s_range[] = {0, 255};
const float* ranges[] = {h_range, s_range};
int channels[] = {0, 1};

// function prototype
void mouseHandler(int event, int x, int y, int flags, void* param);
void getHistogram(Mat roi);

// track bar
int lo = 50;
int up = 50;

int main(int argc, char** argv)
{
    VideoCapture cap(0);

    if( !cap.isOpened() )
    {
        cout << "Could not initialize capturing...\n";
        return -1;
    }
    
    namedWindow(src_window,CV_WINDOW_AUTOSIZE);
    setMouseCallback(src_window,mouseHandler,0);
    
    for(;;){
        cap >> frame;
        cvtColor(frame, hsv, CV_BGR2HSV);

        if(setup){
            MatND backproj;
            calcBackProject(&hsv, 1, channels, hist, backproj, ranges, 1, true);
            imshow("BackProj", backproj);
            int max_iteration = 10;
            double desired_accuracy = 1;
            int itrs = meanShift(backproj, rect_roi, TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, max_iteration, desired_accuracy));
            rectangle(frame, rect_roi, Scalar(255,0,0), 2, CV_AA);

        }
        imshow(src_window,frame);
        createTrackbar("Low threshold", src_window, &lo, 255, 0);
        createTrackbar("High threshold", src_window, &up, 255, 0);
        
        if(waitKey(30) == 27){
            cout <<"esc key is pressed by user" << endl;
            break;
        }
    }

    return 0;
}

void getHistogram(){
    int ch[] = {0, 0};
    int h_bins = 30;    // initial 30
    int s_bins = 32;    // initial 32
    int histSize[] = {h_bins,s_bins};
    
    calcHist(&hsv, 1, channels, mask, hist, 2, histSize, ranges, true, false);
    normalize(hist,hist,0,255,NORM_MINMAX,-1,Mat());
   
}

void mouseHandler(int event, int x, int y, int flags, void* param)
{

    if(event != EVENT_LBUTTONDOWN){
        return;
    }

    Point seed = Point(x,y);
    int newMaskVal = 255;
    Scalar newVal = Scalar(120,120,120);
    int connectivity = 8;
    int flood_flag = connectivity + (newMaskVal << 8) + FLOODFILL_FIXED_RANGE + FLOODFILL_MASK_ONLY;
    
    Mat mask2 = Mat::zeros(frame.rows+2,frame.cols+2,CV_8UC1);
    floodFill(frame, mask2, seed, newVal, &rect_roi, Scalar(lo,lo,lo), Scalar(up,up,up),flood_flag);
    mask = mask2(Range(1,mask2.rows-1),Range(1,mask2.cols-1));

    imshow("Mask", mask);
    getHistogram();
    setup = true;
//    if (event == CV_EVENT_LBUTTONDOWN && !drag)
//    {
//        /* left button clicked. ROI selection begins */
//        point1 = Point(x, y);
//        drag = 1;
//        setup = false;
//    }
//
//    if (event == CV_EVENT_MOUSEMOVE && drag)
//    {
//        /* mouse dragged. ROI being selected */
//        Mat img1 = frame.clone();
//        point2 = Point(x, y);
//        rectangle(img1, point1, point2, CV_RGB(255, 0, 0), 1, 8, 0);
//        
//        imshow(src_window, img1);
//    }
//
//    if (event == CV_EVENT_LBUTTONUP && drag)
//    {
//        Mat img2 = frame.clone();
//
//        point2 = Point(x, y);
//        drag = 0;
//
//        cout << "point1  " << point1.x << "  " << point1.y << endl;
//        cout << "point2  " << point2.x << "  " << point2.y << endl;
//        rect_roi = Rect(point1.x, point1.y, (point2.x-point1.x+1), (point2.y-point1.y+1));
//        Mat roi(img2, rect_roi);
//        cvtColor(roi, roi, CV_BGR2HSV);
//        
//        namedWindow("roi",WINDOW_NORMAL);
//        imshow("roi",roi);        
//        getHistogram(roi);
//        setup = true;
//    }
}
