#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <math.h>

using namespace std;

const int PADDING = 1;

int** dilate(int **data_array, int height, int width){
  
    int **data_copy;
    int count = 0;
    data_copy = new int *[height+2*PADDING];   
    for(int i = 0; i < height+2*PADDING; i++){
        data_copy[i] = new int[width+2*PADDING];
        for(int j = 0; j < width+2*PADDING; j++){
            data_copy[i][j] = data_array[i][j];
        }
    }

    for(int i = PADDING; i < height+PADDING; i++){
        for(int j = PADDING; j < width+PADDING; j++){
            if(data_array[i-PADDING][j-PADDING] == 1 || data_array[i-PADDING][j] == 1 || data_array[i-PADDING][j+PADDING] == 1 || data_array[i][j-PADDING] == 1 || data_array[i][j] == 1 || data_array[i][j+PADDING] == 1 || data_array[i+PADDING][j-PADDING] == 1 || data_array[i+PADDING][j] == 1 || data_array[i+PADDING][j+PADDING] == 1){
                data_copy[i][j] = 1;
                count++;
            }
            else{
                data_copy[i][j] = 0;
            }
        }
    }
    cout << "count in dilate: " << count << "\n";
    return data_copy;
}
int** erode(int **data_array, int height, int width){

    int **data_copy;
    data_copy = new int *[height+2*PADDING];
    int count = 0;
    for(int i = 0; i < height+2*PADDING; i++){
        data_copy[i] = new int[width+2*PADDING];
        for(int j = 0; j < width+2*PADDING; j++){
            data_copy[i][j] = data_array[i][j];
        }
    }

    for(int i = PADDING; i < height+PADDING; i++){
        for(int j = PADDING; j < width+PADDING; j++){
            if(data_array[i-PADDING][j-PADDING] == 1 && data_array[i-PADDING][j] == 1 && data_array[i-PADDING][j+PADDING] == 1 && data_array[i][j-PADDING] == 1 && data_array[i][j] == 1 && data_array[i][j+PADDING] == 1 && data_array[i+PADDING][j-PADDING] == 1 && data_array[i+PADDING][j] == 1 && data_array[i+PADDING][j+PADDING] == 1){
                data_copy[i][j] = 1;
                count ++;
            }
            else{
                data_copy[i][j] = 0;
            }
        }
    }
    cout << "count in erode: " << count << "\n";
    return data_copy;

}

int main(){
    
    string operation;
    ifstream in_file("binary.pgm");
    string temp;
    int width;
    int height;
    int max;
    string data_line;

    getline(in_file,operation);
    cout << operation << endl;
    istringstream strm;
    getline(in_file,temp);
    strm.str(temp);
    strm >> width >> height;
    cout << width << " " << height << endl;
    
    getline(in_file,temp);
    max = atoi(temp.c_str());
    cout << max << endl;
    
    int **data_array;
    data_array = new int *[height+2*PADDING];
    getline(in_file,data_line);
    
    istringstream strm1;
    strm1.str(data_line);
    int initial_count = 0;
    
    for(int i = 0; i < height+2*PADDING; i++){
        data_array[i] = new int[width+2*PADDING];
        for(int j = 0; j < width+2*PADDING; j++){
            if(i < PADDING || i >= height+PADDING || j < PADDING || j >= width+PADDING){
                data_array[i][j] = 0;
            }
            else{
               int data;
               strm1 >> data;
               if(data == 0){
                    data_array[i][j] = 1;
                    initial_count++;
                }
                else{
                    data_array[i][j] = 0;
                }
            }
//            cout << data_array[i][j] << " ";
        }
//       cout << "\n";
    }
    
    cout << "initial count: " << initial_count << "\n";

    // open
    data_array = erode(data_array, height, width);
    data_array = dilate(data_array, height, width);

    // close
//    data_array = dilate(data_array, height, width);
//    data_array = erode(data_array, height, width);

    int count = 0;
    for(int i = 0; i < height+2*PADDING; i++){
        for(int j = 0; j < width+2*PADDING; j++){
            if(i < PADDING || i >= height+PADDING || j < PADDING || j >= width+PADDING){
            }
            else{
                if(data_array[i][j] == 1){
                    count++;
                }
            }
         }
    }
    cout << "count in main: " << count << "\n";
 
    ofstream out_file;
    out_file.open("open.pgm");
    out_file << operation << "\n" << width << " " << height << "\n" << max << "\n";    

    for(int i = PADDING; i < height+PADDING; i++){
        for(int j = PADDING; j < width+PADDING; j++){
                if(data_array[i][j] == 1){
                    out_file << 0 << " ";
                }
                else{
                    out_file << 255 << " ";
                }
            }
    }

    return 0;

}
