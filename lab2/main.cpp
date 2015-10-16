#include <stdio.h>
#include "graph.h"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <stdlib.h>
#include <algorithm>

using namespace std;
using namespace cv;

class Pixel{
public:
    int r;
    int g;
    int b;
    double foreground;
    double background;
    double prior_energy[8];
    int whatisit;       // 2 is foreground, 4 is background

};

Mat img;
Pixel **pixel_array;
int small = 0;
int big = 100;
int selection_mode = 0;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
    if  ( event == EVENT_LBUTTONDOWN )		// foreground select
    {
        selection_mode = 1;
    }
    else if  ( event == EVENT_RBUTTONDOWN )
    {
        cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
    }
    else if  ( event == EVENT_MBUTTONDOWN )        // background select
    {
        selection_mode = 2;
        
    }
    else if(event == EVENT_LBUTTONUP){
        selection_mode = 0;
    }
    else if(event == EVENT_MBUTTONUP){
        selection_mode = 0;
    }
    else if ( event == EVENT_MOUSEMOVE )
    {
        // cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;
        if(selection_mode == 1){
            cout << "foreground selected at " << y << " " << x << endl;
            cout << "r g b value " << pixel_array[y][x].r << " " << pixel_array[y][x].g << " " << pixel_array[y][x].b << endl;
            pixel_array[y][x].foreground = small;
            pixel_array[y][x].background = big;
            pixel_array[y][x].whatisit = 2;
        }
        else if(selection_mode == 2){
            cout << "background selected at " << y << " " << x << endl;
            cout << "r g b value " << pixel_array[y][x].r << " " << pixel_array[y][x].g << " " << pixel_array[y][x].b << endl;
            pixel_array[y][x].foreground = big;
            pixel_array[y][x].background = small;
            pixel_array[y][x].whatisit = 4;
        }
    }
}

int main()
{
    // Read image from file
    img = imread("twitter.jpg");

    //if fail to read the image
    if ( img.empty() )
    {
        cout << "Error loading the image" << endl;
        return -1;
    }
    
    cout << img.type() << " " << img.channels() << endl;

    int height = img.rows;
    int width = img.cols;
    cout << "height " << height << " width " << width << endl;
    
    int total = img.total();
    cout << "total node " << total << endl;
    
    int total_edges = (height-2)*(width-2)*8;
    cout << "total edges " << total_edges << endl;

    pixel_array = new Pixel *[height];
    for(int i = 0; i < height; i++){
        pixel_array[i] = new Pixel[width];
        for(int j = 0; j < width; j++){
            Vec3b pix = img.at<Vec3b>(i,j);
            pixel_array[i][j].r = pix.val[2];
            pixel_array[i][j].g = pix.val[1];
            pixel_array[i][j].b = pix.val[0];
        }
    }

    //Create a window
    namedWindow("My Window", 1);

    //set the callback function for any mouse event
    setMouseCallback("My Window", CallBackFunc, NULL);

    //show the image
    imshow("My Window", img);

    // Wait until user press some key
    waitKey(0);
    cout << "foreground, background selection finished !" << endl;
    
    // K means
    int num_fore = 0;
    int num_back = 0;
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if(pixel_array[i][j].whatisit == 2){    // 2 for foreground
                num_fore++;
            }
            else if(pixel_array[i][j].whatisit == 4){
                num_back++;
            }
        }
    }
   
    Mat fore_mat(num_fore, 3, CV_32F);
    Mat back_mat(num_back, 3, CV_32F);

    int fore_row = 0;
    int fore_col = 0;
    int back_row = 0;
    int back_col = 0;
    cout << "num_fore " << num_fore << endl;
    cout << "num_back " << num_back << endl;

    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if(pixel_array[i][j].whatisit == 2){
                fore_mat.at<float>(fore_row, fore_col) = pixel_array[i][j].r;
                fore_col++;
                fore_mat.at<float>(fore_row, fore_col) = pixel_array[i][j].g;
                fore_col++;
                fore_mat.at<float>(fore_row, fore_col) = pixel_array[i][j].b;
                fore_col = 0;
                fore_row++;
            }
            else if(pixel_array[i][j].whatisit == 4){
                back_mat.at<float>(back_row, back_col) = pixel_array[i][j].r;
                back_col++;
                back_mat.at<float>(back_row, back_col) = pixel_array[i][j].g;
                back_col++;
                back_mat.at<float>(back_row, back_col) = pixel_array[i][j].b;
                back_col = 0;
                back_row++;
            }
        }
    }
    
    cout << "# of foreground " << fore_row << " # of background " << back_row << endl;

    Mat labels1;
    Mat labels2;
    
    int num_clusters = 3;
    Mat fore_centers;
    Mat back_centers;
    
    kmeans(fore_mat, num_clusters, labels1, TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 0.5), 5, KMEANS_PP_CENTERS, fore_centers);
    kmeans(back_mat, num_clusters, labels2, TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 0.8), 5, KMEANS_PP_CENTERS, back_centers);

    cout << "fore_centers" << endl << fore_centers << endl;

    cout << "back_centers" << endl << back_centers << endl;

    // likelihood: process all the unknown pixels(neither foreground nor background)
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if((pixel_array[i][j].whatisit != 2) && (pixel_array[i][j].whatisit != 4)){
                
                double df = min(min(abs(pixel_array[i][j].r-fore_r),abs(pixel_array[i][j].g-fore_g)),abs(pixel_array[i][j].b-fore_b));
                double db = min(min(abs(pixel_array[i][j].r-back_r),abs(pixel_array[i][j].g-back_g)),abs(pixel_array[i][j].b-back_b));
                pixel_array[i][j].foreground = df/(df+db);
                pixel_array[i][j].background = db/(df+db);
                //if(j == 128){
                //    cout << pixel_array[i][j].foreground << " " << pixel_array[i][j].background << endl;
                //}
            }
        }
    }
    
    Mat likelihood(height,width,img.type());
    cout << likelihood.type() << " " << likelihood.channels() << endl;
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            //cout << i << " " << j << endl;
            if(pixel_array[i][j].whatisit == 2){
                likelihood.at<Vec3b>(i,j).val[0] = 255;
                likelihood.at<Vec3b>(i,j).val[1] = 0;
                likelihood.at<Vec3b>(i,j).val[2] = 0;
            }
            else if(pixel_array[i][j].whatisit == 4){
                likelihood.at<Vec3b>(i,j).val[0] = 0;
                likelihood.at<Vec3b>(i,j).val[1] = 255;
                likelihood.at<Vec3b>(i,j).val[2] = 0;
            }
            else{
                likelihood.at<Vec3b>(i,j).val[0] = pixel_array[i][j].foreground*255;
                likelihood.at<Vec3b>(i,j).val[1] = pixel_array[i][j].foreground*255;
                likelihood.at<Vec3b>(i,j).val[2] = pixel_array[i][j].foreground*255;
            }
        }
    }

    namedWindow("likelihood", CV_WINDOW_AUTOSIZE);
    imshow("likelihood", likelihood);
    waitKey(0);

    // prior energy
    int neighbors = 8;
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if((i != 0) && (i != height-1) && (j != 0) && (j != width-1)){
                for(int k = 0; k < neighbors; k++){
                    double cij = 0;
                    if(k == 0){
                        cij = sqrt(pow(2.0,(pixel_array[i][j].r-pixel_array[i-1][j-1].r))+pow(2.0,(pixel_array[i][j].g-pixel_array[i-1][j-1].g))+pow(2.0,(pixel_array[i][j].b-pixel_array[i-1][j-1].b)));
                    }
                    else if(k == 1){
                        cij = sqrt(pow(2.0,(pixel_array[i][j].r-pixel_array[i-1][j].r))+pow(2.0,(pixel_array[i][j].g-pixel_array[i-1][j].g))+pow(2.0,(pixel_array[i][j].b-pixel_array[i-1][j].b)));
                    }
                    else if(k == 2){
                        cij = sqrt(pow(2.0,(pixel_array[i][j].r-pixel_array[i-1][j+1].r))+pow(2.0,(pixel_array[i][j].g-pixel_array[i-1][j+1].g))+pow(2.0,(pixel_array[i][j].b-pixel_array[i-1][j+1].b)));
                    }
                    else if(k == 3){
                        cij = sqrt(pow(2.0,(pixel_array[i][j].r-pixel_array[i][j-1].r))+pow(2.0,(pixel_array[i][j].g-pixel_array[i][j-1].g))+pow(2.0,(pixel_array[i][j].b-pixel_array[i][j-1].b)));
                    }
                    else if(k == 4){
                        cij = sqrt(pow(2.0,(pixel_array[i][j].r-pixel_array[i][j+1].r))+pow(2.0,(pixel_array[i][j].g-pixel_array[i][j+1].g))+pow(2.0,(pixel_array[i][j].b-pixel_array[i][j+1].b)));
                    }
                    else if(k == 5){
                        cij = sqrt(pow(2.0,(pixel_array[i][j].r-pixel_array[i+1][j-1].r))+pow(2.0,(pixel_array[i][j].g-pixel_array[i+1][j-1].g))+pow(2.0,(pixel_array[i][j].b-pixel_array[i+1][j-1].b)));
                    }
                    else if(k == 6){
                        cij = sqrt(pow(2.0,(pixel_array[i][j].r-pixel_array[i+1][j].r))+pow(2.0,(pixel_array[i][j].g-pixel_array[i+1][j].g))+pow(2.0,(pixel_array[i][j].b-pixel_array[i+1][j].b)));
                    }
                    else if(k == 7){
                        cij = sqrt(pow(2.0,(pixel_array[i][j].r-pixel_array[i+1][j+1].r))+pow(2.0,(pixel_array[i][j].g-pixel_array[i+1][j+1].g))+pow(2.0,(pixel_array[i][j].b-pixel_array[i+1][j+1].b)));
                    }
                    double g = 1.0/(cij+1);
                    pixel_array[i][j].prior_energy[k] = g;
                }
            }
        }
    }

    typedef Graph<int,int,int> GraphType;
    GraphType *g = new GraphType(/*estimated # of nodes*/ total, /*estimated # of edges*/ total_edges);
    
    for(int i = 0; i < total; i++){
        g -> add_node();
    }

    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            int index = i*width+j;
            g -> add_tweights(index, pixel_array[i][j].foreground, pixel_array[i][j].background);
        }
    }
    
    for(int i = 1; i < height-1; i++){
        for(int j = 1; j < width-1; j++){
            int index = i*width+j;
            for(int k = 0; k < neighbors; k++){
                if(k == 0){
                    g -> add_edge(index, index-width-1, pixel_array[i][j].prior_energy[0], pixel_array[i][j].prior_energy[0]);
                }
                else if(k == 1){
                    g -> add_edge(index, index-width, pixel_array[i][j].prior_energy[1], pixel_array[i][j].prior_energy[1]);
                }
                else if(k == 2){
                    g -> add_edge(index, index-width+1, pixel_array[i][j].prior_energy[2], pixel_array[i][j].prior_energy[2]);
                }
                else if(k == 3){
                    g -> add_edge(index, index-1, pixel_array[i][j].prior_energy[3], pixel_array[i][j].prior_energy[3]);
                }
                else if(k == 4){
                    g -> add_edge(index, index+1, pixel_array[i][j].prior_energy[4], pixel_array[i][j].prior_energy[4]);
                }
                else if(k == 5){
                    g -> add_edge(index, index+width-1, pixel_array[i][j].prior_energy[5], pixel_array[i][j].prior_energy[5]);
                }
                else if(k == 6){
                    g -> add_edge(index, index+width, pixel_array[i][j].prior_energy[6], pixel_array[i][j].prior_energy[6]);
                }
                else if(k == 7){
                    g -> add_edge(index, index+width+1, pixel_array[i][j].prior_energy[7], pixel_array[i][j].prior_energy[7]);
                }
            }
        }
    }

    int flow = g -> maxflow();

    printf("Flow = %d\n", flow);
    printf("Minimum cut:\n");
//    if (g->what_segment(0) == GraphType::SOURCE)
//        printf("node0 is in the SOURCE set\n");
//    else
//        printf("node0 is in the SINK set\n");
    
    Mat result(height, width, img.type());
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            int index = i*height+j;
            if(g->what_segment(index) == GraphType::SOURCE){
                result.at<Vec3b>(i,j).val[0] = 255;
                result.at<Vec3b>(i,j).val[1] = 255;
                result.at<Vec3b>(i,j).val[2] = 255;
            }
            else{
                result.at<Vec3b>(i,j).val[0] = 0;
                result.at<Vec3b>(i,j).val[1] = 0;
                result.at<Vec3b>(i,j).val[2] = 0;
            }
        }
    }

    namedWindow("result", 1);
    imshow("result", result);
    
    waitKey(0);

    delete g;

    return 0;
}

