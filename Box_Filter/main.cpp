#include <cv.h>
#include <highgui.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include "opencv2/opencv.hpp"
#include "getCPUTime.h"


static int KERNEL_SIZE;
static char IMAGE_NAME[256];
static char OUTPUT_NAME[256];
static bool OPTIMIZATION;


extern "C" IplImage* box_filter_direct_method(const IplImage* source_image)
{
    double startTime, endTime;

    startTime = getCPUTime( );
    IplImage* copy = cvCloneImage(source_image);
    for (int i = KERNEL_SIZE/2;i<copy->height-KERNEL_SIZE/2;i++)
        for (int j = KERNEL_SIZE/2;j<copy->height-KERNEL_SIZE/2;j++) {
            double accumulator = 0;
            for (int bias_i = -KERNEL_SIZE/2; bias_i < KERNEL_SIZE-KERNEL_SIZE/2; bias_i++)
                for (int bias_j = -KERNEL_SIZE/2; bias_j < KERNEL_SIZE-KERNEL_SIZE/2; bias_j++)
                    accumulator+=cvGetReal2D(copy, i + bias_i, j + bias_j);
            double  scale = accumulator/(KERNEL_SIZE*KERNEL_SIZE);

            cvSet2D(copy,i,j,scale);
        }
    endTime = getCPUTime( );
    fprintf( stderr, "CPU time used = %lf\n", (endTime - startTime) );
    return copy;
}

extern "C" IplImage* box_filter_optimized_method(const IplImage* source_image)
{
    double startTime, endTime;

    startTime = getCPUTime( );

    IplImage* copy = cvCloneImage(source_image);
    for (int i = KERNEL_SIZE/2;i<copy->height-KERNEL_SIZE/2;i++)
        for (int j = KERNEL_SIZE/2;j<copy->height-KERNEL_SIZE/2;j++) {
            double accumulator = 0;

            for (int bias_i = -KERNEL_SIZE/2; bias_i < KERNEL_SIZE-KERNEL_SIZE/2; bias_i++)
                    accumulator+=cvGetReal2D(copy, i + bias_i, j);
            double  scale = accumulator/(KERNEL_SIZE);
            cvSet2D(copy,i,j,scale);
        }
    for (int i = KERNEL_SIZE/2;i<copy->height-KERNEL_SIZE/2;i++)
        for (int j = KERNEL_SIZE/2;j<copy->height-KERNEL_SIZE/2;j++) {
            double accumulator = 0;

            for (int bias_j = -KERNEL_SIZE/2; bias_j < KERNEL_SIZE-KERNEL_SIZE/2; bias_j++)
                accumulator+=cvGetReal2D(copy, i , j + bias_j);
            double  scale = accumulator/(KERNEL_SIZE);
            cvSet2D(copy,i,j,scale);
        }
    endTime = getCPUTime( );
    fprintf( stderr, "CPU time used = %lf\n", (endTime - startTime) );
    return copy;

}

bool readParametrs(const char *file_name_config) {
    cv::FileStorage fs(file_name_config, cv::FileStorage::READ);
    std::string image_name;
    std::string output_name;
    int kernel_size;
    if (fs.isOpened()) {
        fs["image_name"] >> image_name;
        fs["kernel_size"] >> kernel_size;
        fs["output_name"] >> output_name;
        fs["optimization"] >> OPTIMIZATION;
        fs.release();
        output_name = output_name.size()<256 && output_name.size()>5 ? (output_name): "../src/output_image.jpg";
        strcpy(OUTPUT_NAME,output_name.c_str());
        image_name = image_name.size()<256 && image_name.size()>5? (image_name): "../src/Image0.jpg";
        strcpy(IMAGE_NAME,image_name.c_str());
        KERNEL_SIZE = (kernel_size % 2 != 0 ? (kernel_size) : kernel_size + 1);
        return true;
    } else {
        printf("Error 0: file %s was not opened", file_name_config);
        fs.release();
        return false;
    }
}


int main(int argc, char **argv) {
    const char *file_name_config = argc == 2 ? argv[1] : "../config/config.yml";
    if (!readParametrs(file_name_config))
        return 0;
    static IplImage *input_image;
    input_image = cvLoadImage(IMAGE_NAME, CV_LOAD_IMAGE_GRAYSCALE);
    cvNamedWindow("input image", 1);
    cvShowImage("input image", input_image);
    IplImage* output_image;
    if(OPTIMIZATION)
        output_image = box_filter_optimized_method(input_image);
    else
        output_image = box_filter_direct_method(input_image);
    cvNamedWindow("output image", 1);
    cvShowImage("output image", output_image);
    if(!cv::imwrite(OUTPUT_NAME,cv::cvarrToMat(output_image)))
        printf("Could not save: %s\n",OUTPUT_NAME);

    cvWaitKey(0);
    return 0;
}

