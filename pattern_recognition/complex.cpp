#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <cmath>

using namespace cv;
using namespace std;

class Feature{
public:
    Mat object;
    int xmin;
    int xmax;
    int ymin;
    int ymax;
    int label;
    int area;
    int perimeter;
    double compactness;
    double fb_ratio;   // background pixel / foreground pixel
    int max_crossing;
    int cluster;
};


Mat image;
int b_background;
int g_background;
int r_background;
int** label_array;
vector<Feature> shape_container;

void connected_component(int row, int col);
int find_howmany(int row, int col);
Mat show_connected(int row, int col);
void calculate_compactness();
void count_zerocrossing();
void get_profile();
void linear_discriminant(Mat centers);
Mat show_cluster();

int main(){
	
//====== TRANING PART

    image = imread("train1.pgm");   // shapes.pgm  train1.pgm  train2.pgm
    if(! image.data ){                              // Check for invalid input
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }
    
    namedWindow( "Training image", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Training image", image );                   // Show our image inside it.

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
    namedWindow("Training connected", WINDOW_AUTOSIZE);
    imshow("Training connected", check);

    int howmany = find_howmany(row, col);
    cout << "the number of shapes:  " << shape_container.size() << endl;
    calculate_compactness();
    count_zerocrossing();
//    get_profile();

    for(int i = 0; i < shape_container.size(); i++){
        Feature temp = shape_container.at(i);
        namedWindow("object",WINDOW_NORMAL);
        imshow("object",temp.object);
        cout << "label:  " << temp.label << "  area:  " << temp.area << endl;
        cout << "perimeter:  " << temp.perimeter << "  compactness:  " << temp.compactness << endl;
        cout << "fb_ratio:  " << temp.fb_ratio << endl;
        cout << "max_crossing:  " << temp.max_crossing << endl;
        waitKey(0);
    }
    int num_feature = 2;
    Mat input_samples(shape_container.size(), num_feature, CV_32F);
    for(int i = 0; i < shape_container.size(); i++){
        input_samples.at<float>(i,0) = shape_container.at(i).compactness;
        input_samples.at<float>(i,1) = shape_container.at(i).fb_ratio;
    }
    Mat result_clusters;
    int num_clusters = 5;
    Mat centers;
    int attempts = 10;
    kmeans(input_samples, num_clusters, result_clusters, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,10,0.01), attempts, KMEANS_PP_CENTERS, centers);
//    cout << "clusters:  " << result_clusters << endl;
//    cout << "centroids:  " << centers << endl;
    
    // assign cluster number for each shape in shape_container
    for(int i = 0; i < shape_container.size(); i++){
       shape_container.at(i).cluster = result_clusters.at<int>(i,0);
//       cout << shape_container.at(i).cluster << endl;
    }

    Mat cluster1 = show_cluster();
    namedWindow("Training cluster",WINDOW_AUTOSIZE);
    imshow("Training cluster",cluster1);

//====== TEST CLASSIFICATION PART

//    image = imread("match1.pgm");       // testshapes.pgm  match1.pgm   match2.pgm
//    if(! image.data ){                              // Check for invalid input
//        cout <<  "Could not open or find the image" << std::endl ;
//        return -1;
//    }
//    
//    namedWindow( "Testing image", WINDOW_AUTOSIZE );// Create a window for display.
//    imshow( "Testing image", image );                   // Show our image inside it.
//    
//    connected_component(row, col);
//
//    Mat check2 = show_connected(row, col);
//    namedWindow("Testing connected", WINDOW_AUTOSIZE);
//    imshow("Testing connected", check2);
//
//    int howmany_test = find_howmany(row, col);
//    cout << "the number of shapes:  " << shape_container.size() << endl << endl;
//    
//    calculate_compactness();
//    
//    linear_discriminant(centers);
//    Mat cluster2 = show_cluster();
//    namedWindow("Testing cluster",WINDOW_AUTOSIZE);
//    imshow("Testing cluster",cluster2);

    waitKey(0);                                          // Wait for a keystroke in the window
    
    return 0;
}

// show image with clusters
Mat show_cluster(){

    Mat cluster_image(image.rows, image.cols, image.type());
//    cout << image.rows << " " << image.cols << endl;
    for(int i = 0; i < shape_container.size(); i++){
        Mat temp_shape = shape_container.at(i).object;
//        namedWindow("what", WINDOW_NORMAL);
//        imshow("what", temp_shape);
//        waitKey(0);
        if(shape_container.at(i).cluster == 0){
            for(int j = 0; j < temp_shape.rows; j++){
                for(int k = 0; k < temp_shape.cols; k++){
                    if(temp_shape.at<Vec3b>(j,k)[0] != 0){
                        temp_shape.at<Vec3b>(j,k)[0] = 255;
                        temp_shape.at<Vec3b>(j,k)[1] = 1;
                        temp_shape.at<Vec3b>(j,k)[2] = 1;
                    }
                }
            }
        }
        else if(shape_container.at(i).cluster == 1){
            for(int j = 0; j < temp_shape.rows; j++){
                for(int k = 0; k < temp_shape.cols; k++){
                    if(temp_shape.at<Vec3b>(j,k)[0] != 0){
                        temp_shape.at<Vec3b>(j,k)[0] = 1;
                        temp_shape.at<Vec3b>(j,k)[1] = 255;
                        temp_shape.at<Vec3b>(j,k)[2] = 1;
                    }
                }
            }
        }
        else if(shape_container.at(i).cluster == 2){
            for(int j = 0; j < temp_shape.rows; j++){
                for(int k = 0; k < temp_shape.cols; k++){
                    if(temp_shape.at<Vec3b>(j,k)[0] != 0){
                        temp_shape.at<Vec3b>(j,k)[0] = 1;
                        temp_shape.at<Vec3b>(j,k)[1] = 1;
                        temp_shape.at<Vec3b>(j,k)[2] = 255;
                    }
                }
            }
        }
        else if(shape_container.at(i).cluster == 3){
            for(int j = 0; j < temp_shape.rows; j++){
                for(int k = 0; k < temp_shape.cols; k++){
                    if(temp_shape.at<Vec3b>(j,k)[0] != 0){
                        temp_shape.at<Vec3b>(j,k)[0] = 255;
                        temp_shape.at<Vec3b>(j,k)[1] = 255;
                        temp_shape.at<Vec3b>(j,k)[2] = 1;
                    }
                }
            }
        }
        else if(shape_container.at(i).cluster == 4){
            for(int j = 0; j < temp_shape.rows; j++){
                for(int k = 0; k < temp_shape.cols; k++){
                    if(temp_shape.at<Vec3b>(j,k)[0] != 0){
                        temp_shape.at<Vec3b>(j,k)[0] = 255;
                        temp_shape.at<Vec3b>(j,k)[1] = 1;
                        temp_shape.at<Vec3b>(j,k)[2] = 255;
                    }
                }
            }
        }
        int xmin = shape_container.at(i).xmin;
        int xmax = shape_container.at(i).xmax;
        int ymin = shape_container.at(i).ymin;
        int ymax = shape_container.at(i).ymax;
//        cout << xmin << " " << xmax << " " << ymin << " " << ymax << endl;
        Rect roi(Point(xmin,ymin),Size(xmax-xmin+1,ymax-ymin+1));
        Mat destinationROI = cluster_image(roi);
        temp_shape.copyTo(cluster_image(roi));
    }
//    namedWindow("cluster image", WINDOW_AUTOSIZE);
//    imshow("cluster image", cluster_image);
//    waitKey(0);
    return cluster_image;
}

// run linear discriminant
void linear_discriminant(Mat centers){
    for(int i = 0; i < shape_container.size(); i++){
        double temp_compactness = shape_container.at(i).compactness;
        double temp_ratio = shape_container.at(i).fb_ratio;
        cout << "compactness:  " << temp_compactness << "  fb_ratio:  " << temp_ratio << endl;
        int max_index = -1;
        double max_value = -100;
        double g1 = (temp_compactness*centers.at<float>(0,0)+temp_ratio*centers.at<float>(0,1))-0.5*(pow(centers.at<float>(0,0),2)+pow(centers.at<float>(0,1),2));
        double g2 = (temp_compactness*centers.at<float>(1,0)+temp_ratio*centers.at<float>(1,1))-0.5*(pow(centers.at<float>(1,0),2)+pow(centers.at<float>(1,1),2));
        if(g1 >= g2){
            max_index = 0;
            max_value = g1;
        }
        else{
            max_index = 1;
            max_value = g2;
        }
        double g3 = (temp_compactness*centers.at<float>(2,0)+temp_ratio*centers.at<float>(2,1))-0.5*(pow(centers.at<float>(2,0),2)+pow(centers.at<float>(2,1),2));
        if(g3 >= max_value){
            max_index = 2;
            max_value = g3;
        }
        double g4 = (temp_compactness*centers.at<float>(3,0)+temp_ratio*centers.at<float>(3,1))-0.5*(pow(centers.at<float>(3,0),2)+pow(centers.at<float>(3,1),2));
        if(g4 >= max_value){
            max_index = 3;
            max_value = g4;
        }
        double g5 = (temp_compactness*centers.at<float>(4,0)+temp_ratio*centers.at<float>(4,1))-0.5*(pow(centers.at<float>(4,0),2)+pow(centers.at<float>(4,1),2));
        if(g5 >= max_value){
            max_index = 4;
            max_value = g5;
        }
        shape_container.at(i).cluster = max_index;
        cout << "g12345:  " << g1 << "  " << g2 << "  " << g3 << "  " << g4 << "  " << g5 << endl;
        cout << i << " shape belongs to " << max_index << " cluster" << endl << endl;
    }
}

// count zerocrossing
void count_zerocrossing(){
    for(int i = 0; i < shape_container.size(); i++){
        Mat temp_object = shape_container.at(i).object;
        int v_crossing = 0;
        int h_crossing = 0;
        int horizontal_half = temp_object.cols/2;
        int vertical_half = temp_object.rows/2;
        for(int j = 0; j < temp_object.rows; j++){
            if(j == 0){
                if(temp_object.at<Vec3b>(j,horizontal_half)[0] != 0){
                    v_crossing = 1;
                }
            }
            else{
                if(temp_object.at<Vec3b>(j,horizontal_half)[0] != 0 && temp_object.at<Vec3b>(j-1,horizontal_half)[0] == 0){
                    v_crossing++;
                }
            }

        }
        for(int j = 0; j < temp_object.cols; j++){
            if(j == 0){
                if(temp_object.at<Vec3b>(vertical_half,j)[0] != 0){
                    h_crossing = 1;
                }
            }
            else{
                if(temp_object.at<Vec3b>(vertical_half,j)[0] != 0 && temp_object.at<Vec3b>(vertical_half,j-1)[0] == 0){
                    h_crossing++;
                }
            }

        }
//        cout << i << " th " << v_crossing << " " << h_crossing << endl;
        int max_crossing = max(v_crossing, h_crossing);
        shape_container.at(i).max_crossing = max_crossing;
    }
}

// calculate compactness
void calculate_compactness(){
    // calculate perimeter first, then compactness
    for(int i = 0; i < shape_container.size(); i++){
        Mat temp_image = shape_container.at(i).object;
        int perimeter = 0;
        int num_back = 0;
        for(int j = 0; j < temp_image.rows; j++){
            for(int k = 0; k < temp_image.cols; k++){
                if(temp_image.at<Vec3b>(j,k)[0] != 0){
                    if(j == 0 || j == temp_image.rows-1 || k == 0 || k == temp_image.cols-1){
                        perimeter++;
                    }
                    else{
                        if(temp_image.at<Vec3b>(j-1,k)[0] == 0 || temp_image.at<Vec3b>(j+1,k)[0] == 0 || temp_image.at<Vec3b>(j,k-1)[0] == 0 || temp_image.at<Vec3b>(j,k+1)[0] == 0){   // if any neighboring pixel is background, it's on edge
                            perimeter++;
                        }
                    }
                }
                else{
                    num_back++;
                }
            }
        }
        shape_container.at(i).perimeter = perimeter;
        double compactness = pow(perimeter,2)/(double)shape_container.at(i).area;
        if(compactness > 45){
            compactness = 45;
        }
        shape_container.at(i).compactness = compactness;
        double fb_ratio = (double)num_back/(double)shape_container.at(i).area * 10;    // *100 put more weight fb_ratio
        if(fb_ratio > 200){
            fb_ratio = 200;
        }
        shape_container.at(i).fb_ratio = fb_ratio;
//        cout << "perimeter:  " << perimeter << endl;
//        cout << "foreground:  " << shape_container.at(i).area << "  background:  " << num_back << endl;
//        cout << "total:  " << shape_container.at(i).area + num_back << "  row x col:  " << temp_image.rows*temp_image.cols << endl;
//        namedWindow("temp",WINDOW_NORMAL);
//        imshow("temp",shape_container.at(i).object);
//        waitKey(0);
    }
}

// find how many different shapes are in the image
int find_howmany(int row, int col){
    shape_container.clear();
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
        Mat copy_image = image.clone();
        for(int i = 0; i < row; i++){
            for(int j = 0; j < col; j++){
                if(label_array[i][j] != *it){
                    copy_image.at<Vec3b>(i,j)[0] = 0;
                    copy_image.at<Vec3b>(i,j)[1] = 0;
                    copy_image.at<Vec3b>(i,j)[2] = 0;
                }
                else{
                    copy_image.at<Vec3b>(i,j)[0] = 255;
                    copy_image.at<Vec3b>(i,j)[1] = 255;
                    copy_image.at<Vec3b>(i,j)[2] = 255;
                }
            }
        }
//        namedWindow("copy", WINDOW_AUTOSIZE);
//        imshow("copy",copy_image);
//        waitKey(0);

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
//        cout << xmin << "  " << xmax << endl;
//        cout << ymin << "  " << ymax << endl;
        Mat object(copy_image, Rect(xmin, ymin, xmax-xmin+1, ymax-ymin+1));
        Feature new_shape;
        new_shape.object = object.clone();
        new_shape.label = *it;
        new_shape.area = count;
        new_shape.xmin = xmin;
        new_shape.xmax = xmax;
        new_shape.ymin = ymin;
        new_shape.ymax = ymax;
        shape_container.push_back(new_shape);
//        namedWindow("object",WINDOW_NORMAL);
//        imshow("object",object);
//        waitKey(0);
//        cout <<"for label: " << *it << "  area:  " << count << endl;

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
    
//    namedWindow( "connected", WINDOW_AUTOSIZE );
//    imshow( "connected", check );
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

//// profile
//void get_profile(){
//    ofstream horizontal;
//    ofstream vertical;
//    horizontal.open("horizontal.txt");
//    vertical.open("vertical.txt");
//
//    for(int i = 0; i < shape_container.size(); i++){
//        Mat temp_image = shape_container.at(i).object;
//        int row = temp_image.rows;
//        int col = temp_image.cols;
//        int horizontal_profile[col];    // pixel points for each column
//        int vertical_profile[row];      // pixel points for each row
//        for(int j = 0; j < row; j++){
//            vertical_profile[j] = 0;
//        }
//        for(int j = 0; j < col; j++){
//            horizontal_profile[j] = 0;
//        }
//        for(int j = 0; j < row; j++){
//            for(int k = 0; k < col; k++){
//                if(temp_image.at<Vec3b>(j,k)[0] != 0){
//                   vertical_profile[j]++;
//                }
//            }
//            vertical << vertical_profile[j] << " ";
//        }
//        vertical << "\n\n";
//        for(int j = 0; j < col; j++){
//            for(int k = 0; k < row; k++){
//                if(temp_image.at<Vec3b>(j,k)[0] != 0){
//                   horizontal_profile[j]++;
//                }
//            }
//            horizontal << horizontal_profile[j] << " ";
//        }
//        horizontal << "\n\n";
//    }
//    horizontal.close();
//    vertical.close();
//}
