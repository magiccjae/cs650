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
//Mat roi;
Mat m_backproj;
MatND m_model3d;

Rect rect_roi;
Point point1, point2;

const char* src_window = "Select ROI";

int drag = 0, select_flag = 0;

// for color histogram
int hist_size = 256;
float vrange[] = {0,255};
const float* ranges[] = {vrange,vrange,vrange}; // B,G,R
int channels[] = {0,1,2};
int hist_sizes[] = {hist_size, hist_size, hist_size};

bool setup = false;

// function prototype
void mouseHandler(int event, int x, int y, int flags, void* param);


int main(int argc, char** argv)
{
    VideoCapture cap("messi.mp4");

    if( !cap.isOpened() )
    {
        cout << "Could not initialize capturing...\n";
        return -1;
    }
    
    namedWindow(src_window,CV_WINDOW_AUTOSIZE);
    setMouseCallback(src_window,mouseHandler,0);
    
    for(;;){
        cap >> frame;
//    frame = imread("car.jpg");
//        split(frame, bgr_planes);
        if(setup){
            calcBackProject(&frame, 1, channels, m_model3d, m_backproj, ranges);
            int itrs = meanShift(m_backproj, rect_roi, TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 100, 0.01));
            rectangle(frame, rect_roi, Scalar(0, 0, 255), 3, CV_AA);
        }

        imshow(src_window,frame);
//        waitKey(0);
        if(waitKey(30) == 27){
            cout <<"esc key is pressed by user" << endl;
            break;
        }
    }

    return 0;
}

void mouseHandler(int event, int x, int y, int flags, void* param)
{
    if (event == CV_EVENT_LBUTTONDOWN && !drag && !select_flag)
    {
        /* left button clicked. ROI selection begins */
        point1 = Point(x, y);
        drag = 1;
    }

    if (event == CV_EVENT_MOUSEMOVE && drag && !select_flag)
    {
        /* mouse dragged. ROI being selected */
        Mat img1 = frame.clone();
        point2 = Point(x, y);
        rectangle(img1, point1, point2, CV_RGB(255, 0, 0), 1, 8, 0);
        
        imshow(src_window, img1);
    }

    if (event == CV_EVENT_LBUTTONUP && drag && !select_flag)
    {
        Mat img2 = frame.clone();

        point2 = Point(x, y);
        drag = 0;
        select_flag = 1;

        cout << "point1  " << point1.x << "  " << point1.y << endl;
        cout << "point2  " << point2.x << "  " << point2.y << endl;
        rect_roi = Rect(point1.x, point1.y, (point2.x-point1.x+1), (point2.y-point1.y+1));
        Mat roi(img2, rect_roi);

        int total_pixel = (point2.x-point1.x+1)*(point2.y-point1.y+1);
        cout << "total pixel: " << total_pixel << endl;

        namedWindow("roi",WINDOW_NORMAL);
        imshow("roi",roi);        

        /// Compute the histograms:
        calcHist( &roi, 1, channels, Mat(), m_model3d, 3, hist_sizes, ranges);
        setup = true;
    }
}
