//
// Created by arcstone_mems on 2018/6/8.
// 检测细胞的圆轮廓，用于检测细胞内的物质流动规定感兴趣的区域
// 包含两种方法检测细胞的轮廓，边缘检测和霍夫圆检测
//

#ifndef CELL2_DETECTCELLCONTOUR_H
#define CELL2_DETECTCELLCONTOUR_H

#include <iostream>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <ctime>

#define WINDOW_NAME1 "【细胞原始图窗口】"
#define WINDOW_NAME2 "【细胞轮廓图】"


using namespace std;
using namespace cv;
//定义结构体，用于返回检测到的圆的取点，圆心坐标以及圆的外轮廓半径
struct result_circle
{
    vector<Point2f> first_Node;
    Point2f cell_center;
    int radius_out;
};


class detectCellContour {
    //成员变量定义
public:
    Mat img_cell_detect;
    Mat img_cell_roi;
    Mat img_cell_gray;
    Mat img_cell_gray1; //canny变换后的图像
    vector<Vec3f> cell_circles;
    vector<Vec4i> hierarchy;
    vector<vector<Point>> contours;
    vector<Point2f> cell_point_temp;                                 //存放需要检测的点，临时数组，用于筛选
    vector<Point2f> cell_point1;                                     //存放需要检测的点，最终送入光流检测函数的数组
    int img_h;                                                       //图像的原始尺寸，行数
    int img_w;                                                       //图像的原始尺寸，列数
    Mat first_Image;
    int radius_out;
    Mat cell_flow_gray, cell_flow_pre;
    vector<uchar> status;
    vector<float> err;
    vector<Point2f> points[2];                                       // point0为特征点的原来位置，point1为特征点的新位置

    //成员函数定义
public:
    //输入源图像，输出细胞内取点的坐标容器
    vector<Point2f> detectCell(Mat input_Image,int img_h, int img_w);
    vector<Point2f> detect_hough_circle(Mat inputImage, int img_h, int img_w);
    //光流计算，返回当前帧跟踪得到的点
    vector<Point2f> flow_in_cell(Mat frame, vector<Point2f> points);
    result_circle detect_hough_circle_center(Mat inputImage, int img_h, int img_w);

};


#endif //CELL2_DETECTCELLCONTOUR_H
