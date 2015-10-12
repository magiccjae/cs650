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

void smoothe(int **data_array, int height, int width){

    double **data_copy;
    int count = 0;

    data_copy = new double *[height];   
    for(int i = 0; i < height; i++){
        data_copy[i] = new double[width];
        for(int j = 0; j < width; j++){
            data_copy[i][j] = data_array[i][j];
        }
    }
    
    double kernel[3][3] = {{0.1111,0.1111,0.1111},{0.1111,0.1111,0.1111},{0.1111,0.1111,0.1111}};
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if(i == 0 || j == 0 || i == height-1 || j == width-1){
                count++;
            }
            else{
                
               data_array[i][j] = (int)(data_copy[i-1][j-1]*kernel[0][0]+data_copy[i-1][j]*kernel[0][1]+data_copy[i-1][j+1]*kernel[0][2]+data_copy[i][j-1]*kernel[1][0]+data_copy[i][j]*kernel[1][1]+data_copy[i][j+1]*kernel[1][2]+data_copy[i+1][j-1]*kernel[2][0]+data_copy[i+1][j]*kernel[2][1]+data_copy[i+1][j+1]*kernel[2][2]);
                count++;
            }
        }
    }
    cout << "count in smoothe: " << count << "\n";
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
    int threshold = 50;
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

void create_hough_array(int **hough_array, int height, int width, int padding){
    
    int count = 0;
    hough_array = new int *[height+2*padding];
    for(int i = 0; i < height+2*padding; i++){
        hough_array[i] = new int[width+2*padding];
        for(int j = 0; j < width+2*padding; j++){
            count++;
        }
    }

    cout << "count in create_hough_array: " << count << "\n";

}


void vote(int **threshold_array, int **hough_array, int height, int width, int padding){

    int points = 360;

    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if(threshold_array[i][j] == 255){
                int newi = i+padding;
                int newj = j+padding;
                for(int k = 0; k < points; k++){   // creating 360 points of a circle
                    int a = (int)round(newi + padding*cos(k*2*M_PI/360.0));
                    int b = (int)round(newj + padding*sin(k*2*M_PI/360.0));

                    hough_array[a][b]++;
                }                

            }
        }
    }

}

void pick_center(int **hough_array, int height, int width, int padding, int threshold){
    int surrounding = 4;
    for(int i = 0; i < height+2*padding; i++){
        for(int j = 0; j < width+2*padding; j++){
            if(hough_array[i][j] > threshold){
                int iammax = 0;
                for(int a = 0; a < surrounding; a++){
                    for(int b = 0; b < surrounding; b++){
                        if(hough_array[i][j] > hough_array[i+a][j+b]){
                            hough_array[i+a][j+b] = 0;
                            iammax++;
                        }
                    }
                }
                if(iammax == (surrounding*surrounding-1)){
                    hough_array[i][j] = 255;
                }
                else{
                    hough_array[i][j] = 0;
                }
//                if((hough_array[i][j] < hough_array[i-1][j-1]) || (hough_array[i][j] < hough_array[i-1][j]) || (hough_array[i][j] < hough_array[i-1][j+1]) || (hough_array[i][j] < hough_array[i][j-1]) || (hough_array[i][j] < hough_array[i][j+1]) || (hough_array[i][j] < hough_array[i+1][j-1]) || (hough_array[i][j] < hough_array[i+1][j]) || (hough_array[i][j] < hough_array[i+1][j+1])
//){
//                    hough_array[i][j] = 0;
//                }
//                else{
//                    hough_array[i][j] = 255;
//                }
            }
            else{
                hough_array[i][j] = 0;
            }
        }
    }
}


void draw_circle(int **hough_array, int height, int width, int padding){
    
    int **data_copy;
    data_copy = new int *[height+2*padding];   
    for(int i = 0; i < height+2*padding; i++){
        data_copy[i] = new int[width+2*padding];
        for(int j = 0; j < width+2*padding; j++){
            data_copy[i][j] = hough_array[i][j];
            hough_array[i][j] = 0;
        }
    }
    
    int points = 360;
    

    for(int i = 0; i < height+2*padding; i++){
        for(int j = 0; j < width+2*padding; j++){
            if(data_copy[i][j] == 255){

                for(int k = 0; k < points; k++){   // creating 360 points of a circle
                    int a = (int)round(i + padding*cos(k*2*M_PI/360.0));
                    int b = (int)round(j + padding*sin(k*2*M_PI/360.0));
                    if(a >= 0 && b >= 0){
                        hough_array[a][b] = 255;
                    }
                }                
            }
        }
    }
}

void combine_hough(int **threshold_array, int **hough_32, int **hough_64, int **hough_96, int height, int width, int padding1, int padding2, int padding3){
 
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if(hough_32[i+padding1][j+padding1] == 255 || hough_64[i+padding2][j+padding1] == 255 || hough_96[i+padding3][j+padding3] == 255){
                threshold_array[i][j] = 255;
            }
            else{
                threshold_array[i][j] = 0;
            }
        }
    }
    
    for(int i = 0; i < height+2*padding1; i++){
        for(int j = 0; j < width+2*padding1; j++){
            if(hough_32[i][j] == 255){
                hough_96[i+2*padding2][j+2*padding2] = 255;
            }
        }
    }

    for(int i = 0; i < height+2*padding2; i++){
        for(int j = 0; j < width+2*padding2; j++){
            if(hough_64[i][j] == 255){
                hough_96[i+2*padding1][j+2*padding1] = 255;
            }
        }
    }

}

void write_hough(string filename, int **output_array, int height, int width, int padding, int max){
    ofstream out_file;
    out_file.open("hough.pgm");
    string operation = "P2";
    int new_width = width+2*padding;
    int new_height = height+2*padding;
    out_file << operation << "\n" << new_width << " " << new_height << "\n" << max << "\n";   
    int threshold = 1;
    for(int i = 0; i < new_height; i++){
        for(int j = 0; j < new_width; j++){
            out_file << output_array[i][j] << " ";
        }
        out_file << "\n";
    }
}

void hough_transform(int **threshold_array, int height, int width, int max){
    
    
    int **hough_32;
    int **hough_64;
    int **hough_96;
    
    int padding1 = 16;
    int padding2 = 32;
    int padding3 = 48;
    
    int count = 0;
    hough_32 = new int *[height+2*padding1];
    for(int i = 0; i < height+2*padding1; i++){
        hough_32[i] = new int[width+2*padding1];
        for(int j = 0; j < width+2*padding1; j++){
            count++;
        }
    }
    cout << "count in hough_32: " << count << "\n";
    
    count = 0;
    hough_64 = new int *[height+2*padding2];
    for(int i = 0; i < height+2*padding2; i++){
        hough_64[i] = new int[width+2*padding2];
        for(int j = 0; j < width+2*padding2; j++){
            count++;
        }
    }
    cout << "count in hough_64: " << count << "\n";   
    
    count = 0;
    hough_96 = new int *[height+2*padding3];
    for(int i = 0; i < height+2*padding3; i++){
        hough_96[i] = new int[width+2*padding3];
        for(int j = 0; j < width+2*padding3; j++){
            count++;
        }
    }
    cout << "count in hough_96: " << count << "\n";


//    create_hough_array(hough_32, height, width, padding1);
    
//    for(int i = 0; i < height+2*padding1; i++){
//        for(int j = 0; j < width+2*padding1; j++){
//           cout << "i: " << i << " " << "j: " << j << " " << "stuff: " << hough_32[i][j] << endl;			
//        }
//    }


    int threshold_32 = 125;     // so far 125 is the best
    int threshold_64 = 105;     // so far 105 is the best
    int threshold_96 = 100;     // so far 100 is the best
    vote(threshold_array, hough_32, height, width, padding1);
    vote(threshold_array, hough_64, height, width, padding2);
    vote(threshold_array, hough_96, height, width, padding3);
    pick_center(hough_32,height,width,padding1,threshold_32);
    pick_center(hough_64,height,width,padding2,threshold_64);
    pick_center(hough_96,height,width,padding3,threshold_96);
//    draw_circle(hough_32,height,width,padding1);
//    draw_circle(hough_64,height,width,padding2);
//    draw_circle(hough_96,height,width,padding3);
    combine_hough(threshold_array, hough_32, hough_64, hough_96, height, width, padding1, padding2, padding3);
    write_hough("hough_96", hough_96, height, width, padding3, max);
    
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
    ifstream in_file("circles.ppm");
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
//            in_file.get(data);
//	        unsigned char data_u = (unsigned char) data;
//	        data_array[i][j] = (int)data_u;
//	        initial_count++;

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
	
    // smoothe the image
    smoothe(data_array, height, width);
    smoothe(data_array,height,width);
    calculate_gradient(data_array, gradient_magnitude, gradient_orientation, height, width);
    
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            if(gradient_orientation[i][j] >=0 && gradient_orientation[i][j] < M_PI/2){
                threshold_array[i][j] = 255;
            }
            else if(gradient_orientation[i][j] >= M_PI/2 && gradient_orientation[i][j] < M_PI){
                threshold_array[i][j] = 170;
            }
            else if(gradient_orientation[i][j] <= -M_PI/2 && gradient_orientation[i][j] > -M_PI){
                threshold_array[i][j] = 85;
            }
            else{
                threshold_array[i][j] = 0;
            }
        }
    }

    laplacian(data_array, laplacian_array, height, width);      // laplacian_array binarized after this point
    threshold_gradient(gradient_magnitude, threshold_array, height, width);     // threshold_Array binarized after this point
    joint(laplacian_array, threshold_array, height, width);
    
    hough_transform(threshold_array, height, width, max);

// checking how many elements
    int count = 0;
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            count++;

         }
    }
    cout << "count in main: " << count << "\n";
 
//  writing to an image file
    write_file(threshold_array, operation,height, width, max);

    return 0;

}
