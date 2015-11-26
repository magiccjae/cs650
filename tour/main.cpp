#include "opencv2/highgui/highgui.hpp"
#include <iostream>

using namespace std;
using namespace cv;

Mat img, rear_img, box_img;
const char* src_window = "original";
Point vanishing, left_top, right_top, left_bottom, right_bottom;


void mouseCallback(int event, int x, int y, int flags, void* userdata);

int main(){

    img = imread("hallway.jpg");

    if(img.empty()){
        cout << "Error loading the image" << endl;
        return -1;
    }

    namedWindow(src_window,WINDOW_AUTOSIZE);
    setMouseCallback(src_window,mouseCallback,0);
    
    imshow(src_window,img);
    waitKey(0);
    
    return 0;
}

void mouseCallback(int event, int x, int y, int flags, void* userdata){
    
    if(flags == (EVENT_FLAG_CTRLKEY+EVENT_FLAG_LBUTTON)){     // ctrl + click sets a vanishing point
        box_img = rear_img.clone();
        cout << "vanishing point:  " << y << "  " << x << endl;
        vanishing = Point(x,y);
        circle(box_img,vanishing, 2, CV_RGB(255,0,0),1,8,0);
        line(box_img,vanishing,left_top,CV_RGB(255,0,0),1,8,0);
        line(box_img,vanishing,right_top,CV_RGB(255,0,0),1,8,0);
        line(box_img,vanishing,left_bottom,CV_RGB(255,0,0),1,8,0);
        line(box_img,vanishing,right_bottom,CV_RGB(255,0,0),1,8,0);
        imshow(src_window,box_img);
    }
    else if(event == EVENT_LBUTTONDOWN){
        cout << "rear box started:  " << y << "  " << x << endl;
        left_top = Point(x,y);
    }
    else if(event == EVENT_LBUTTONUP){
        rear_img = img.clone();
        cout << "rear box ended:  " << y << "  " << x << endl;
        right_top = Point(x,left_top.y);
        left_bottom = Point(left_top.x,y);
        right_bottom = Point(x,y);
        rectangle(rear_img, left_top, right_bottom, CV_RGB(255,0,0),1,8,0);
        imshow(src_window,rear_img);
    }

}
