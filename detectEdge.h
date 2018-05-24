//
// Created by arcstone_mems on 2018/5/23.
//

#ifndef CELL2_DETECTEDGE_H
#define CELL2_DETECTEDGE_H

#include <iostream>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <ctime>

#define WINDOW_NAME1 "【原始图窗口】"
#define WINDOW_NAME2 "【轮廓图】"


using namespace std;
using namespace cv;

class detectEdge {
//成员变量的定义
public:
    Mat det_firstImage;
    Mat det_frame;
    Mat img_detect;
    Mat img_gray;
    Mat det_dst, det_edge, det_gray, det_dst2, det_dst3, det_dst4, det_dst5, det_dst6;
    int g_nThresh = 80;
    int g_nThresh_max = 255;
    //RNG g_rng();
    vector<vector<Point> > g_vContours;
    vector<Vec4i> g_vHierarchy;
    Mat output_img;
    Point node_x_y;                                        //定义返回的标志点的坐标，坐标是基于ROI图像的
    Point node_done;                                       //定义基于原始帧的图像得到的坐标
    int img_h;                                             //定义输入图像的高度
    int img_w;                                             //定义输入图像的宽度
    int imgROI_h_begin;                                    //ROI区域的开始点
    int imgROI_w_begin;                                    //ROI区域的开始点
    int imgROI_h_height;                                   //ROI区域的高度
    int imgROI_w_width;                                    //ROI区域的宽度
    int node_y;                                            //标志点的y坐标，等于imgROI_h_height的一半

//成员函数的定义
public:
    detectEdge();
    void swap(int *a, int *b);
    int sorted(vector<Point> nodeList);
    Point detectCell(Mat filename);                        //将输入的图像进行边缘检测，返回标志点，返回的是基于ROI区域的坐标
    Point get_node(Point node_x_y);                        //得到最终的坐标，也就是基于原始帧的图像得到的坐标
};


#endif //CELL2_DETECTEDGE_H
