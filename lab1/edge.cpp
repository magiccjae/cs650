#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <math.h>

using namespace std;


int** dilate(int **data_array, int height, int width){
  
    int **data_copy;
    int count = 0;
    data_copy = new int *[height];   
    for(int i = 0; i < height; i++){
        data_copy[i] = new int[width];
        for(int j = 0; j < width; j++){
            data_copy[i][j] = data_array[i][j];
        }
    }

    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){

        }
    }
    cout << "count in dilate: " << count << "\n";
    return data_copy;
}

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
            if(i = 0 || j = 0 || i = height-1 || j = width-1){
                
            }
            else{
               double didx = data_array[i-1][j-1]*kernel[0][0]+data_array[i-1][j]*kernel[0][1]+data_array[i-1][j+1]*kernel[0][2]+data_array[i][j-1]*kernel[1][0]+data_array[i][j]*kernel[1][1]+data_array[i][j+1]*kernel[1][2]+data_array[i+1][j-1]*kernel[2][0]+data_array[i+1][j]*kernel[2][1]+data_array[i+1][j+1]*kernel[2][2];
               
               double didy = data_array[i-1][j-1]*kernel[0][0]+data_array[i-1][j]*kernel[1][0]+data_array[i-1][j+1]*kernel[2][0]+data_array[i][j-1]*kernel[0][1]+data_array[i][j]*kernel[1][1]+data_array[i][j+1]*kernel[2][1]+data_array[i+1][j-1]*kernel[0][2]+data_array[i+1][j]*kernel[1][2]+data_array[i+1][j+1]*kernel[2][2];

               gradient_magnitude[i][j] =  sqrt(pow(didx,2.0)+pow(didy,2.0));
               gradient_orientation = atan2(didy/didx);     // in radian



            }
        }
    }
    cout << "count in calculate_gradient: " << count << "\n";
}

int main(){
    
    string operation;
    ifstream in_file("blocks.pgm");
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
    gradient_magnitude = new double *[height];
    gradient_orientation = new double *[height];
    char data;
    int initial_count = 0;
    
    for(int i = 0; i < height; i++){
        data_array[i] = new int[width];
        gradient_magnitude[i] = new double[width];
        gradient_orientation[i] = new double[width]; 
        for(int j = 0; j < width; j++){
            in_file.get(data);
	        unsigned char data_u = (unsigned char) data;
			data_array[i][j] = (int)data_u;
			initial_count++;
        }
    }   

    cout << "initial count: " << initial_count << "\n";
	
	calculate_gradient(data_array, gradient_magnitude, gradient_orientation, height, width);
	
// checking how many elements
    int count = 0;
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            count++;

         }
    }
    cout << "count in main: " << count << "\n";
 
 
// writing to an image file
    ofstream out_file;
    out_file.open("result.pgm");
    operation = "P2";
    out_file << operation << "\n" << width << " " << height << "\n" << max << "\n";    

    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
        	out_file << 128 << " ";
        }
    }


    return 0;

}
