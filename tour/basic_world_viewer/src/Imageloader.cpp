
#include <assert.h>
#include <fstream>
#include <iostream>
#include <jpeglib.h>
#include <jerror.h>

#include "Imageloader.h"

using namespace std;

Image::Image(unsigned char* ps, int w, int h) : pixels(ps), width(w), height(h) {

}

Image::~Image() {
    delete[] pixels;
}

namespace {
    //Converts a four-character array to an integer, using little-endian form

    int toInt(const char* bytes) {
        return (int) (((unsigned char) bytes[3] << 24) |
                ((unsigned char) bytes[2] << 16) |
                ((unsigned char) bytes[1] << 8) |
                (unsigned char) bytes[0]);
    }

    //Converts a two-character array to a short, using little-endian form

    short toShort(const char* bytes) {
        return (short) (((unsigned char) bytes[1] << 8) |
                (unsigned char) bytes[0]);
    }

    //Reads the next four bytes as an integer, using little-endian form

    int readInt(ifstream &input) {
        char buffer[4];
        input.read(buffer, 4);
        return toInt(buffer);
    }

    //Reads the next two bytes as a short, using little-endian form

    short readShort(ifstream &input) {
        char buffer[2];
        input.read(buffer, 2);
        return toShort(buffer);
    }

    //Just like auto_ptr, but for arrays

    template<class T>
    class auto_array {
    private:
        T* array;
        mutable bool isReleased;
    public:

        explicit auto_array(T* array_ = NULL) :
        array(array_), isReleased(false) {
        }

        auto_array(const auto_array<T> &aarray) {
            array = aarray.array;
            isReleased = aarray.isReleased;
            aarray.isReleased = true;
        }

        ~auto_array() {
            if (!isReleased && array != NULL) {
                delete[] array;
            }
        }

        T* get() const {
            return array;
        }

        T &operator*() const {
            return *array;
        }

        void operator=(const auto_array<T> &aarray) {
            if (!isReleased && array != NULL) {
                delete[] array;
            }
            array = aarray.array;
            isReleased = aarray.isReleased;
            aarray.isReleased = true;
        }

        T* operator->() const {
            return array;
        }

        T* release() {
            isReleased = true;
            return array;
        }

        void reset(T* array_ = NULL) {
            if (!isReleased && array != NULL) {
                delete[] array;
            }
            array = array_;
        }

        T* operator+(int i) {
            return array + i;
        }

        T &operator[](int i) {
            return array[i];
        }
    };
}

Image* loadBMP(const char* filename) {
    ifstream input;
    input.open(filename, ifstream::binary);
    assert(!input.fail() || !"Could not find file");
    char buffer[2];
    input.read(buffer, 2);
    assert(buffer[0] == 'B' && buffer[1] == 'M' || !"Not a bitmap file");
    input.ignore(8);
    int dataOffset = readInt(input);

    cout << "reading the file..." << endl;

    //Read the header
    int headerSize = readInt(input);
    int width;
    int height;
    switch (headerSize) {
        case 40:
            //V3
            width = readInt(input);
            height = readInt(input);
            input.ignore(2);
            assert(readShort(input) == 24 || !"Image is not 24 bits per pixel");
            assert(readShort(input) == 0 || !"Image is compressed");
            break;
        case 12:
            //OS/2 V1
            width = readShort(input);
            height = readShort(input);
            input.ignore(2);
            assert(readShort(input) == 24 || !"Image is not 24 bits per pixel");
            break;
        case 64:
            //OS/2 V2
            assert(!"Can't load OS/2 V2 bitmaps");
            break;
        case 108:
            //Windows V4
            assert(!"Can't load Windows V4 bitmaps");
            break;
        case 124:
            //Windows V5
            assert(!"Can't load Windows V5 bitmaps");
            break;
        default:
            assert(!"Unknown bitmap format");
    }

    //Read the data
    int bytesPerRow = ((width * 3 + 3) / 4) * 4 - (width * 3 % 4);
    int size = bytesPerRow * height;
    auto_array<char> pixels(new char[size]);
    input.seekg(dataOffset, ios_base::beg);
    input.read(pixels.get(), size);

    //Get the data into the right format
    auto_array<unsigned char> pixels2(new unsigned char[width * height * 3]);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < 3; c++) {
                pixels2[3 * (width * y + x) + c] =
                        pixels[bytesPerRow * y + 3 * x + (2 - c)];
            }
        }
    }

    input.close();
    return new Image(pixels2.release(), width, height);
}

Image* loadJPG(const char* filename) {
    unsigned long x;
    unsigned long y;
    unsigned short int bpp; //bits per pixels   unsigned short int
    unsigned char* data; //the data of the image
    unsigned long size; //length of the file
    int channels; //the channels of the image 3 = RGA 4 = RGBA


    FILE* file = fopen(filename, "rb"); //open the file
    struct jpeg_decompress_struct info; //the jpeg decompress info
    struct jpeg_error_mgr err; //the error handler

    info.err = jpeg_std_error(&err); //tell the jpeg decompression handler to send the errors to err
    jpeg_create_decompress(&info); //sets info to all the default stuff

    //if the jpeg file didnt load exit
    if (!file) {
        fprintf(stderr, "Error reading JPEG file %s!!!", filename);
        return 0;
    }

    jpeg_stdio_src(&info, file); //tell the jpeg lib the file we'er reading

    jpeg_read_header(&info, TRUE); //tell it to start reading it

    jpeg_start_decompress(&info); //decompress the file

    //set the x and y
    x = info.output_width;
    y = info.output_height;
    channels = info.num_components;

    bpp = channels * 8;

    size = x * y * 3;

    //read turn the uncompressed data into something ogl can read
    data = new unsigned char[size]; //setup data for the data its going to be handling

    unsigned char* p1 = data;
    unsigned char** p2 = &p1;
    p1;
    int numlines = 0;

    while (info.output_scanline < info.output_height) {
        numlines = jpeg_read_scanlines(&info, p2, 1);
        *p2 += numlines * 3 * info.output_width;
    }

    jpeg_finish_decompress(&info); //finish decompressing this file

    fclose(file); //close the file

    return new Image(data, x, y);
}




