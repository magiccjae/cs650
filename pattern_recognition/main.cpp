#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <algorithm>
#include <map>

using namespace cv;
using namespace std;

Mat image;
int b_background;
int g_background;
int r_background;

void connected_component(int row, int col);

int main(){
	
	image = image = imread("shapes.pgm");
    if(! image.data ){                              // Check for invalid input
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }
    int row = image.rows;
    int col = image.cols;
    cout << row << " x " << col << " image" << endl;
    b_background = (int)image.at<Vec3b>(0,0)[0];
    g_background = (int)image.at<Vec3b>(0,0)[1];
    r_background = (int)image.at<Vec3b>(0,0)[2];
    cout << "background rgb  " << b_background << " " << g_background << " " << r_background << endl;
    connected_component(row, col);

	namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Display window", image );                   // Show our image inside it.

    waitKey(0);                                          // Wait for a keystroke in the window

	
	return 0;

}

void connected_component(int row, int col){
    map<int,int> mymap;
    int label_array[row][col];
    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            label_array[i][j] = 0;
        }
    }
    int label = 1;
    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
           if(image.at<Vec3b>(i,j)[0] != b_background){
               if(i == 0){
                    if(j != 0){
                        if(label_array[i][j-1] == 0){   // create new label
                            label_array[i][j] = label;
                            label++;
                        }
                        else{       // assign the same label as the left pixel to the current pixel
                            label_array[i][j] = label_array[i][j-1];
                        }
                    }
                    cout << "it must not be here" << endl;
               }
               else{
                    if(j != 0){
                        if(label_array[i-1][j] == 0 && label_array[i][j-1] == 0){   // create new label
                            label_array[i][j] = label;
                            label++;
                            //cout << j << " " << i << " " << label_array[i][j] << endl;
                        }
                        else if(label_array[i-1][j] == 0 && label_array[i][j-1] != 0){  // assign the same label as the left pixel to the current pixel
                            label_array[i][j] = label_array[i][j-1];
                        }
                        else if(label_array[i][j-1] == 0 && label_array[i-1][j] != 0){  // assign the same label as the top pixel to the current pixel
                            label_array[i][j] = label_array[i-1][j];
                        }
                        else if(label_array[i-1][j] != 0 && label_array[i][j-1] != 0){  // assign a smaller label between left and up pixel to the current pixel
                            label_array[i][j] = min(label_array[i-1][j],label_array[i][j-1]);
                            if(label_array[i-1][j] != label_array[i][j-1]){     // greater label maps to smaller label
                                if(min(label_array[i-1][j],label_array[i][j-1]) == label_array[i-1][j]){
                                    mymap.insert(pair<int,int>(label_array[i][j-1],label_array[i-1][j]));
                                }
                                else{
                                    mymap.insert(pair<int,int>(label_array[i-1][j],label_array[i][j-1]));
                                }
                            }
                        }
                    }
               }
           }
        } 
    }
    
    // merging labels in mymap
    map<int,int>::iterator it = mymap.begin();
    for(it=mymap.begin(); it!=mymap.end(); ++it){
        //cout << it->first << " => " << it->second << endl;
        if(mymap.find(it->second) != mymap.end()){
            it->second = mymap.find(it->second)->second;
        }
    }
    
//    for(it=mymap.begin(); it!=mymap.end(); ++it){
//        cout << it->first << " => " << it->second << endl;
//    }

    // merging labels in label_array
    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            if(label_array[i][j] != 0){
                map<int,int>::iterator current;
                current = mymap.find(label_array[i][j]);
                if(current != mymap.end()){
                    label_array[i][j] = current->second;
                }
            }
        }
    }
    
    Mat check(row, col, image.type());
    
    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            check.at<Vec3b>(i,j)[0] = label_array[i][j];
            check.at<Vec3b>(i,j)[1] = label_array[i][j];
            check.at<Vec3b>(i,j)[2] = label_array[i][j];

        }
    }
	namedWindow( "check", WINDOW_AUTOSIZE );
    imshow( "check", check );

}
