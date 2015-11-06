#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <math.h>

using namespace std;

int find_threshold(double threshold[], int max){
    int index;
    double maximum_value = 0;
    for(int i = 0; i < max; i++){
        if(threshold[i] > maximum_value){
            maximum_value = threshold[i];
            index = i;
        }   
    }
    return index;
}

int main(){
    
    string operation;
    string comment;
    string comment2;
    ifstream in_file("blocks.pgm");

    string temp;
    int width;
    int height;
    int max;

    getline(in_file,operation);
    cout << operation << endl;
//    getline(in_file,comment);
//    cout << comment << endl;
    
    istringstream strm;
    getline(in_file,temp);
    strm.str(temp);
    strm >> width >> height;
    cout << width << " " << height << endl;
    
    getline(in_file,temp);
    max = atoi(temp.c_str());
    cout << max << endl;

    int data_array[height][width];
    char data;

    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            in_file.get(data);
            unsigned char data_u = (unsigned char) data;
            data_array[i][j] = (int)data_u;
        }
    }


    int total_pixel = width*height;
    double threshold[max];
    for(int a = 0; a < max; a++){
        int left = 0;
        int right = 0;
        double left_mean = 0;
        double right_mean = 0;
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
                if(data_array[i][j] < a){
                    left++;
                    left_mean += data_array[i][j];
                }
                else{
                    right++;
                    right_mean += data_array[i][j];
                }
            }
        }
        if(left == 0 || right == 0){
            left_mean = 0;
            right_mean = 0;
        }
        else{
            left_mean = left_mean/left;
            right_mean = right_mean/right;
        }
        double left_prob = (double)left/total_pixel;
        double right_prob = (double)right/total_pixel;
        double sigma = left_prob*right_prob*pow((left_mean-right_mean),2);
        threshold[a] = sigma;
    }
    
    int opt_threshold = find_threshold(threshold,max);
    cout << opt_threshold << endl;
 
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if(data_array[i][j] < opt_threshold){
                data_array[i][j] = 0;
            }
            else{
                data_array[i][j] = max;
            }
 //           cout << data_array[i][j] << " ";
        }
    }

    ofstream out_file;
    out_file.open("binary.pgm");
    out_file << operation << "\n" << width << " " << height << "\n" << max << "\n";

    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){

            out_file << data_array[i][j] << " ";
        }
    }

    out_file.close();

    return 0;

}
