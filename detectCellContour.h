//
// Created by arcstone_mems on 2018/6/8.
// 检测细胞的圆轮廓，用于检测细胞内的物质流动规定感兴趣的区域
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

class detectCellContour {
    //成员变量定义
public:
    Mat det_cell_img;                                                //存放要检测图片




    //成员函数定义
public:
    //Point det_cell_Contour(Mat )



















};


#endif //CELL2_DETECTCELLCONTOUR_H
