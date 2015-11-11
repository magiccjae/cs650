#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <algorithm>
#include <map>
#include <set>
#include <string>

using namespace cv;
using namespace std;

Mat image;
int b_background;
int g_background;
int r_background;
int** label_array;
vector<Mat> shape_container;

void connected_component(int row, int col);
int find_howmany(int row, int col);
Mat show_connected(int row, int col);

int main(){
	
    image = image = imread("shapes.pgm");
    if(! image.data ){                              // Check for invalid input
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }
    
    namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Display window", image );                   // Show our image inside it.

    int row = image.rows;
    int col = image.cols;
    cout << row << " x " << col << " image" << endl;
    
    label_array = new int *[row];
    for(int i = 0; i < row; i++){
        label_array[i] = new int[col];
        for(int j = 0; j < col; j++){
            label_array[i][j] = 0;
        }
    }

    b_background = (int)image.at<Vec3b>(0,0)[0];
    g_background = (int)image.at<Vec3b>(0,0)[1];
    r_background = (int)image.at<Vec3b>(0,0)[2];
    cout << "background rgb  " << b_background << " " << g_background << " " << r_background << endl;
    

    connected_component(row, col);
    
    Mat check = show_connected(row, col);

    int howmany = find_howmany(row, col);
    cout << "the number of shapes:  " << shape_container.size() << endl;
    for(int i = 0; i < shape_container.size(); i++){
        namedWindow("hey",WINDOW_NORMAL);
        imshow("hey",shape_container.at(i));
        waitKey(0);
    }

    waitKey(0);                                          // Wait for a keystroke in the window
	
    return 0;

}

// find how many different shapes are in the image
int find_howmany(int row, int col){
    set<int> myset;
    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            if(label_array[i][j] != 0){
                myset.insert(label_array[i][j]);
            }
        }
    }

    for(set<int>::iterator it=myset.begin(); it!=myset.end(); ++it){
        int count = 0;
        int xmin = col;
        int xmax = 0;
        int ymin = row;
        int ymax = 0;
        for(int i = 0; i < row; i++){
            for(int j = 0; j < col; j++){
                if(label_array[i][j] == *it){
                    count++;
                    if(xmin > j){
                        xmin = j;
                    }
                    if(xmax < j){
                        xmax = j;
                    }
                    if(ymin > i){
                        ymin = i;
                    }
                    if(ymax < i){
                        ymax = i;
                    }
                }
            }
        }
        cout << xmin << "  " << xmax << endl;
        cout << ymin << "  " << ymax << endl;
        Mat object(image, Rect(xmin, ymin, xmax-xmin+1, ymax-ymin+1));
        shape_container.push_back(object);
//        namedWindow("object",WINDOW_NORMAL);
//        imshow("object",object);
//        waitKey(0);
        cout <<"for label: " << *it << "  area:  " << count << endl;

    }

    return myset.size();

}

// display the result image of connected component
Mat show_connected(int row, int col){
    Mat check(row,col,image.type());
    
    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            if(label_array[i][j] != 0){
//                check.at<Vec3b>(i,j)[0] = 255-label_array[i][j];
//                check.at<Vec3b>(i,j)[1] = 255;
//                check.at<Vec3b>(i,j)[2] = label_array[i][j];
                check.at<Vec3b>(i,j)[0] = label_array[i][j];
                check.at<Vec3b>(i,j)[1] = label_array[i][j];
                check.at<Vec3b>(i,j)[2] = label_array[i][j];
            }
            else{
                check.at<Vec3b>(i,j)[0] = 0;
                check.at<Vec3b>(i,j)[1] = 0;
                check.at<Vec3b>(i,j)[2] = 0;
            }
        }
    }
    
    namedWindow( "connected", WINDOW_AUTOSIZE );
    imshow( "connected", check );
    return check;
}


// figure out different shapes in the image
void connected_component(int row, int col){
    map<int,int> mymap;
//    int label_array[row][col];
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
    
}
