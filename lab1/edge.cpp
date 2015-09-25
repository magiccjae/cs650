#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <math.h>

using namespace std;


void calculate_gradient(int **data_array, double **gradient_magnitude, double **gradient_orientation, int height, int width){

    int **data_copy;
    int count = 0;

    data_copy = new int *[height];   
    for(int i = 0; i < height; i++){
        data_copy[i] = new int[width];
        for(int j = 0; j < width; j++){
            data_copy[i][j] = data_array[i][j];
        }
    }
    
    int kernel[3][3] = {{-1,0,1},{-1,0,1},{-1,0,1}};

    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if(i == 0 || j == 0 || i == height-1 || j == width-1){
                count++;
            }
            else{
               double didx = data_array[i-1][j-1]*kernel[0][0]+data_array[i-1][j]*kernel[0][1]+data_array[i-1][j+1]*kernel[0][2]+data_array[i][j-1]*kernel[1][0]+data_array[i][j]*kernel[1][1]+data_array[i][j+1]*kernel[1][2]+data_array[i+1][j-1]*kernel[2][0]+data_array[i+1][j]*kernel[2][1]+data_array[i+1][j+1]*kernel[2][2];
               
               double didy = data_array[i-1][j-1]*kernel[0][0]+data_array[i-1][j]*kernel[1][0]+data_array[i-1][j+1]*kernel[2][0]+data_array[i][j-1]*kernel[0][1]+data_array[i][j]*kernel[1][1]+data_array[i][j+1]*kernel[2][1]+data_array[i+1][j-1]*kernel[0][2]+data_array[i+1][j]*kernel[1][2]+data_array[i+1][j+1]*kernel[2][2];

               gradient_magnitude[i][j] =  sqrt(pow(didx,2.0)+pow(didy,2.0));
               gradient_orientation[i][j] = atan2(didy,didx);     // in radian
               
                count++;
            }
        }
    }
    cout << "count in calculate_gradient: " << count << "\n";
}

void zero_crossing(int **laplacian_array, int height, int width){
    
    int **data_copy;
    data_copy = new int *[height];   
    for(int i = 0; i < height; i++){
        data_copy[i] = new int[width];
        for(int j = 0; j < width; j++){
            data_copy[i][j] = laplacian_array[i][j];
        }
    }

    int count = 0;
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if(i == 0 || j == 0 || i == height-1 || j == width-1){
                count++;
            }
            else{
                if(data_copy[i][j] <= 0){
                    if((data_copy[i-1][j] < 0 && data_copy[i+1][j] > 0) || (data_copy[i-1][j] > 0 && data_copy[i+1][j] < 0)){   // top to bottom
                        laplacian_array[i][j] = 255;
                    }
                    else if((data_copy[i][j-1] < 0 && data_copy[i][j+1] > 0) || (data_copy[i][j-1] > 0 && data_copy[i][j+1] < 0)){   // left to right
                        laplacian_array[i][j] = 255;
                    }
                    else if((data_copy[i-1][j-1] < 0 && data_copy[i+1][j+1] > 0) || (data_copy[i-1][j-1] > 0 && data_copy[i+1][j+1] < 0)){   // top-left to bottom-right
                        laplacian_array[i][j] = 255;
                    }
                    else if((data_copy[i-1][j+1] < 0 && data_copy[i+1][j-1] > 0) || (data_copy[i-1][j+1] > 0 && data_copy[i+1][j-1] < 0)){   // top-right to bottom-left
                        laplacian_array[i][j] = 255;
                    }
                    else{
                        laplacian_array[i][j] = 0;
                    }
                    
                }
                count++;
            }
        }
    }

    cout << "count in zero_crossing: " << count << "\n";
}

void laplacian(int **data_array, int **laplacian_array, int height, int width){
    int count = 0;
    int kernel[3][3] = {{0,1,0},{1,-4,1},{0,1,0}};

    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if(i == 0 || j == 0 || i == height-1 || j == width-1){
                count++;
            }
            else{

               laplacian_array[i][j] = data_array[i-1][j-1]*kernel[0][0]+data_array[i-1][j]*kernel[0][1]+data_array[i-1][j+1]*kernel[0][2]+data_array[i][j-1]*kernel[1][0]+data_array[i][j]*kernel[1][1]+data_array[i][j+1]*kernel[1][2]+data_array[i+1][j-1]*kernel[2][0]+data_array[i+1][j]*kernel[2][1]+data_array[i+1][j+1]*kernel[2][2];
                
	        count++;
            }
        }
    }

    cout << "count in laplacian: " << count << "\n";
    // to find zero crossing
    zero_crossing(laplacian_array, height, width);
    
}

void threshold_gradient(double **gradient_magnitude, int **threshold_array, int height, int width){
    int threshold = 70;
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if(gradient_magnitude[i][j] > threshold){
                threshold_array[i][j] = 255;
            }
            else{
                threshold_array[i][j] = 0;
            }

        }
    }
   
}

void joint(int **laplacian_array, int **threshold_array, int height, int width){
     for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if(laplacian_array[i][j] == 255 && threshold_array[i][j] == 255){
                threshold_array[i][j] = 255;
            }
            else{
                threshold_array[i][j] = 0;
            }

        }
    }
   
}

void write_file(int **output_array, string operation, int height, int width, int max){
    ofstream out_file;
    out_file.open("result.pgm");
    operation = "P2";
    out_file << operation << "\n" << width << " " << height << "\n" << max << "\n";    
    
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if(output_array[i][j] < 0){
                out_file << 0 << " ";
            }
            else if(output_array[i][j] > 255){
                out_file << 255 << " ";
            }
            else{
                out_file << output_array[i][j] << " ";
            }
        }
        out_file << "\n";
    }
}

int main(){
    
    string operation;
    ifstream in_file("simplecircle.ppm");
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
    data_array = new int *[height];
    
    double **gradient_magnitude;
    double **gradient_orientation;
    int **laplacian_array;
    int **threshold_array;
    
    gradient_magnitude = new double *[height];
    gradient_orientation = new double *[height];
    laplacian_array = new int *[height];
    threshold_array = new int *[height];
    
    char data;
    int initial_count = 0;
    
    for(int i = 0; i < height; i++){
        data_array[i] = new int[width];
        gradient_magnitude[i] = new double[width];
        gradient_orientation[i] = new double[width]; 
        laplacian_array[i] = new int[width];
        threshold_array[i] = new int[width];
//  gray version
//        for(int j = 0; j < width; j++){
//          in_file.get(data);
//	    unsigned char data_u = (unsigned char) data;
//	    data_array[i][j] = (int)data_u;
//	    initial_count++;

//   color version
        for(int j = 0; j < width; j++){
            int red = 0;
            int green = 0;
            int blue = 0;
            for(int k = 0; k < 3; k++){
                in_file.get(data);                
	        unsigned char data_u = (unsigned char) data;
                if(k == 0){
                    red = (int)data_u;
                }
                else if(k == 1){
                    green = (int)data_u;
                }
                else if(k == 2){
                    blue = (int)data_u;
                }

            }

            data_array[i][j] =(int)(0.299*red+0.587*green+0.114*blue);
            initial_count++;
        }
    }   

    cout << "initial count: " << initial_count << "\n";
	
    calculate_gradient(data_array, gradient_magnitude, gradient_orientation, height, width);
    laplacian(data_array, laplacian_array, height, width);      // laplacian_array binarized after this point
    threshold_gradient(gradient_magnitude, threshold_array, height, width);     // threshold_Array binarized after this point
    joint(laplacian_array, threshold_array, height, width);
    
    

// checking how many elements
    int count = 0;
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            count++;

         }
    }
    cout << "count in main: " << count << "\n";
 
 // writing to an image file
    write_file(threshold_array, operation, height, width, max);

    return 0;

}
