//
// Created by nku_zth on 2018/4/19.
// test_10.avi一共为171帧，80帧开始取点，隔20帧再取一次，80第一次，100第二次，120第三次，140第四次（意义不大），到150停止
// test_l.avi 的抽核过程从123帧开始，178帧结束
//

#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/video.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "detectEdge.h"
#include "detectCellContour.h"
using namespace std;
using namespace cv;
#define UNKNOWN_FLOW_THRESH 1e9
//-------------------------------------【全局变量声明】----------------------------------------
string window_name = "flow tracking";                                //定义窗口的名称
string window_name2 = "原始图窗口";                                    //原始视频
bool selectObject = false;                                           //定义是否激活鼠标划取区域这个功能
Rect selection;                                                      //定义一个矩形区域，表示鼠标划取的区域
Point origin;                                                        //定义鼠标划取区域的起始点
Mat frame;                                                           //定义当前读取到的视频帧
Mat image;                                                           //存储每一帧待处理的图像
Mat result;                                                          //定义输出的图像
Mat firstImage;                                                      //保存视频第一帧的图像
Mat gray;                                                            //定义当前帧的图像
Mat gray_prev;                                                       //预测帧的图像
Mat flow;                                                            //光流矩阵
Mat cell_flow;                                                       //细胞内部的光流矩阵
Mat gray_detect;                                                     //进行检测的灰度图
Mat cflow;
Mat cell_cflow;
Mat outimg_detect;                                                   //边缘检测后输出的视频帧
int trackObject = 0;
int image_cols;                                                      //读入视频的列数，宽
int image_rows;                                                      //读入视频的行数，高
int k = 0;                                                           //记录当前正在播放的帧数
vector<Point2f> points_temp;                                         //存放需要跟踪的临时点
vector<Point2f> points1;                                             //存放需要跟踪的点
int z = 0;
ofstream outfile;
vector<Point2f> pointflow_now;                                       //保存之前选定的那些点，每个点最新的坐标
vector<Point2f> pointflow_nowcp;                                     //保存之前选定的那些点，每个点最新的坐标
vector<Point2f> cell_pointflow_nowcp;
int ks;                                                              //定义播放多少帧暂停一次
Point node_ROI;                                                      //定义基于有ROI区域的标志点坐标
Point node_img;                                                      //定义基于原始图的标志点的坐标
int chou_begin;                                                      //定义抽核过程开始的帧数
int chou_end;                                                        //定义抽核过程结束的帧数
Mat cell_inside;                                                     //定义细胞内部的检测区域
vector<Point2f> firstNode;                                           //定义细胞内部选取的初始点
vector<Point2f> cell_points[2];                                      //定义细胞内部的点，[0]当前帧，[1]下一帧
bool flag = false;                                                   //设置标志点，是否画出初始点
bool flag_track = false;                                             //设置标志点，是否对点的位置进行更新
vector<uchar> status;
vector<float> err;
Mat cell_flow_gray, cell_flow_pre;
Mat c_cell_flow;                                                     // 存放
const double pi=3.14;
Point2f cell_center;
Mat motion2color;
//-------------------------------------【全局函数声明】----------------------------------------
void drawOptFlowMap(const Mat& flow, Mat& cflowmap, int step, const Scalar& color);
static void onMouse(int event, int x, int y, int, void*);
void tracking(Mat &frame, Mat &output);                              //跟踪函数
Point getNode(Mat frame_n);
void track_cell_in(Mat &frame1, Mat &output1);                       //检测细胞内部的物质
void drawarrow(Mat output, Point2f f_pre, Point2f f_now, int n);     //画箭头
void motionToColor(Mat flow, Mat &color);                            //孟塞尔颜色系统
void makecolorwheel(vector<Scalar> &colorwheel);                     //孟赛尔颜色系统的相关函数
//--------------------------------------【主函数】--------------------------------------------
int main()
{
    outfile.open("data.txt");
    //VideoCapture capture("/Users/arcstone_mems_108/Desktop/result/自动抽核/最靠前/test_4_10.avi");
    VideoCapture capture("/Users/arcstone_mems_108/Desktop/keyan/githubproject/cell2/cmake-build-debug/test_2.avi");
    capture>>firstImage;
    image_cols = firstImage.cols;
    image_rows = firstImage.rows;
    //对细胞内的物质进行光流检测
    Mat inputImage = imread("/Users/arcstone_mems_108/Desktop/keyan/githubproject/cell_edge_detection/untitled/cmake-build-debug/10/120.jpg");
    detectCellContour det_cell_circle;
    //输入第一帧的图像，返回检测到的轮廓包含的点
    result_circle result_circle1;
    result_circle1  =det_cell_circle.detect_hough_circle_center(inputImage, image_rows, image_cols);//霍夫圆检测，返回检测到圆的圆心等信息
    cell_points[0] = result_circle1.first_Node;                           //细胞圆形区域的点
    cell_pointflow_nowcp = cell_points[0];
    cout<<"细胞内部点的个数为："<<cell_points[0].size()<<endl;
    // 在初始帧中画出检测得到的
    for (int i = 0; i < cell_pointflow_nowcp.size(); i++)
    {
        circle(inputImage, cell_pointflow_nowcp[i], 2, CV_RGB(255,200,100), -1);
    }
    imshow("原始", inputImage);
    if(!capture.isOpened())
    {
        cout<<"原始视频未能正确打开！"<<endl;
    }

    int delay = 30;                                                  // 设置等待的时间

    //-------------------------【更换原视频必调参数】--------------------------------------
    ks = 10;                                                         // 每播放20帧暂停一次，***换视频必调参数***
    chou_begin = 110;                                                //
    chou_end = 218;

    //------------------------------------------------------------------------------------------------
    while(true)
    {
        capture>>frame;
        k = k + 1;
        cout<<"当前为第"<<k<<"帧"<<endl;
        outfile<<"当前为第"<<k<<"帧"<<endl;
        if(frame.empty())
        {
            break;
        }
        frame.copyTo(image);
        setMouseCallback(window_name, onMouse, 0);
        if(!frame.empty())
        {
            tracking(image, result);
        }
        //每播放ks帧，暂停一次
        if( k>=chou_begin && (k-chou_begin)%ks == 0 || (k-chou_end) == 0)               //从多少帧开始取点，并且隔多少帧再次取点，***换视频必调参数***
        {
            waitKey(0);
        }
        //等待，等待delay的ms如果没有操作，那么返回-1，视频将继续播放。按一下键，视频播放40帧

        if(delay>=0 && waitKey(delay)>=0)
        {
            waitKey(0);
        }
    }

    return 0;
}


/*鼠标回调函数*/
static void onMouse(int event, int x, int y, int, void*)
{
    if (selectObject)
    {
        selection.x = MIN(x, origin.x);
        selection.y = MIN(y, origin.y);
        selection.width = abs(x - origin.x);
        selection.height = abs(y - origin.y);

        selection &= Rect(0, 0, image.cols, image.rows);     //保证selection在画面的里边
    }
    switch (event)
    {
        case EVENT_LBUTTONDOWN:
            origin = Point(x, y);
            selection = Rect(x, y, 0, 0);
            selectObject = true;
            break;
        case EVENT_LBUTTONUP:
            selectObject = false;
            if (selection.width > 0 && selection.height > 0)
            {
                trackObject = -1;
                outfile<<"这次取点为："<<endl;
            }
            break;
    }
}
/*定义光流跟踪的函数*/
void tracking(Mat &frame, Mat &output)
{
   // Mat cell_frame;
    cvtColor(frame, gray, COLOR_BGR2GRAY);
    frame.copyTo(output);
    //frame.copyTo(cell_frame);
//----------------------------------------------【细胞内物质的光流检测】----------------------------------------
//----------------------------------------------【加入第二次全局稠密光流检测后，程序出现明显卡顿】-------------------
    cvtColor(output, cell_flow_gray, COLOR_BGR2GRAY);                 //将输入图像转换为灰度图
    if(cell_flow_pre.empty())
    {
        cell_flow_gray.copyTo(cell_flow_pre);
    }
    //稠密光流检测，输入整幅画面，进行光流计算，然后显示检测到的圆形内部的画面
    calcOpticalFlowFarneback(cell_flow_pre, cell_flow_gray, cell_flow, 0.5, 2, 15, 3, 5, 1.2, 0);
    //cvtColor(cell_flow_pre, c_cell_flow, CV_GRAY2BGR);
    // 使用孟塞尔颜色系统表示光流，不同颜色表示不同的运动方向，深浅就表示运动的快慢了
    // 没有必要表示整张图像，只要表示细胞内和针管即可
    // 将获得光流进行切割，取出ROI区域
    Mat cell_flow_roi = cell_flow(Rect(image_cols*0.07, image_rows*0.25, image_cols*0.9, image_rows*0.5));
    motionToColor(cell_flow_roi, motion2color);
    imshow("孟塞尔颜色系统", motion2color);
    if(flag_track == true)
    {
        //更新选定的跟踪点的位置，以及保存输出所选点的速度信息
        for (int l = 0; l < cell_points[0].size(); l++)
        {
            //将flow矩阵中对应的原先选定的那些点的数值赋值给fxy
            const Point2f& cell_fxy = cell_flow.at<Point2f>(cell_points[0][l].y, cell_points[0][l].x);
            cell_pointflow_nowcp[l].x = cell_pointflow_nowcp[l].x + cell_fxy.x;
            cell_pointflow_nowcp[l].y = cell_pointflow_nowcp[l].y + cell_fxy.y;
            //画出所有选定的点下一帧的位置
            circle(output, cell_pointflow_nowcp[l], 2, CV_RGB(0,255,0), -1);
            //drawarrow(output, cell_fxy, cell_pointflow_nowcp[l], 5);

        }
    }
    // 在最终的输出图像中画出起始点的位置（细胞内部），只画一次就行了
    if(flag == false)
    {
        for (int i = 0; i < cell_pointflow_nowcp.size(); i++)
        {
            circle(output, cell_pointflow_nowcp[i], 2, CV_RGB(255,200,100), -1);
        }
        flag = true;                                                 // 将标志位定义为true，下一帧中就不需要重新画出初始点的位置
        flag_track = true;                                           // 将标志位定义为true，下一帧中开始执行更新初始点的位置
    }
    swap(cell_flow_pre, cell_flow_gray);

//-----------------------------------------------【液面跟踪模块】----------------------------------------------
    if(k > 140)
    {
        getNode(frame);
        cout<<"-----------最终标志点的坐标为:"<<node_img<<endl;
    }
    //绘制液面位置的标志线和标识文字
    // 标志文字定义
    string text = "marker";
    int font_face = cv::FONT_HERSHEY_SIMPLEX;
    double font_scale = 0.6 ;
    int thickness = 1;
    int baseline;
    Size text_size = getTextSize(text, font_face, font_scale, thickness, &baseline);
    Point origin;
    origin.x = node_img.x - text_size.width/2;
    origin.y = node_img.y - 40;
    if(node_img.x != 0 && node_img.y != 0)
    {
        circle(output, node_img, 2, CV_RGB(0,0,255), -1);//绘制检测得到的液面位置标志点
        line(output, Point(node_img.x, node_img.y-17), Point(node_img.x, node_img.y+17), Scalar(0,0,255), 2, 8);//绘制液面位置的线

        putText(output, text, origin, font_face, font_scale, Scalar(0,255,255), thickness, 8, 0);               // 将液面的标识文字画出

    }
//----------------------------------------------【针管内光流计算模块】----------------------------------------
    // 鼠标勾选针管内需要检测的区域，长方形
    if(selectObject)
    {
        rectangle(output, Point(selection.x, selection.y),
                  Point(selection.x + selection.width,
                        selection.y + selection.height), Scalar(255, 0, 0), 0.5, 8);
    }
    //鼠标抬起时，进行检测
    if(trackObject == -1)
    {
        //画出鼠标勾选的矩形
//        rectangle(output, Point(selection.x, selection.y),
//                  Point(selection.x + selection.width,
//                        selection.y + selection.height), Scalar(255, 0, 0), 0.5, 8);
        //cout<<"矩形的长度和宽度为："<<selection.height<<","<<selection.width<<endl;
       // 对整个画面的每个像素点进行遍历，如果此点在所画矩形内，那么加入点的集合
        for (int y = 0; y < firstImage.rows; y+= 8)
        {
            for (int x = 0; x < firstImage.cols; x+=8)
            {
                Point p(x, y);
                if(selection.contains(p))
                {
                    points_temp.push_back(p);
                }
            }
        }
        points1 = points_temp;
        //pointflow_now = points_temp;
        //pointflow_nowcp应该每画一次新的矩形，变动一次，现在是每一帧都回到原始点，造成位移不能累加
        //当移动到的帧数为画新矩形的下一帧时，给pointflow_nowcp赋予新的值，否则不赋予新的值，该变量即可实现累加
        //该数值为第一次取点的帧数+间隔+1
        if((k-(chou_begin+ks+1))%ks == 0)                                          //***换视频必调参数***，给跟踪区域的点集重新赋值，即重新取点的那一帧
        {
            pointflow_nowcp = points_temp;
        }
        if(k > chou_end)
        {
            imshow(window_name, output);
        }
        cout<<"当前帧需要显示的特征点有"<<points1.size()<<"个"<<endl;
        outfile<<"本次显示的特征点为"<<points1.size()<<"个"<<endl;
        points_temp.clear();                                           //清空point_temp
        //pointflow_now.clear();                                       //清空pointflow_now
        //定义ROI区域，进行稠密光流计算时只计算该区域的光流
        //Mat ROI_img;                                                 //定义进行光流计算的区域
        //Mat ROI_gray;                                                //定义进行光流计算的区域的灰度图
        //Mat ROI_img = frame(Rect(selection.x, selection.y, selection.width, selection.height));
        //cvtColor(ROI_img, ROI_gray,COLOR_BGR2GRAY);

        if(gray_prev.empty())
        {
            gray.copyTo(gray_prev);
        }
        //利用用Gunnar Farneback的算法计算全局性的稠密光流算法
        if(gray_prev.data)
        {
            calcOpticalFlowFarneback(gray_prev, gray, flow, 0.5, 2, 15, 3, 5, 1.2, 0);
            cvtColor(gray_prev, cflow, CV_GRAY2BGR);
            //画出选定的跟踪点的位置，以及保存输出所选点的速度信息
            for (int i = 0; i < points1.size(); i++)
            {
                //将flow矩阵中对应的原先选定的那些点的数值赋值给fxy
                const Point2f& fxy = flow.at<Point2f>(points1[i].y, points1[i].x);
                outfile<<"点的运动速度为"<<setiosflags(ios::fixed)<<setprecision(1)<<fxy.x<<endl;
                //outfile<<"坐标为"<<"("<<points1[i].x<<","<<points1[i].y<<")"<<"的点的运动速度为"<<fxy.x<<endl;
                //定义第i个点运动的速度
                double v_flow = sqrt(fxy.x*fxy.x + fxy.y*fxy.y);
                //定义下一帧的位置（此时的point_n应该是每一次的结果累加，重新选点以后再clear）
                pointflow_nowcp[i].x = pointflow_nowcp[i].x + fxy.x;
                pointflow_nowcp[i].y = pointflow_nowcp[i].y + fxy.y;
                //Point2f point_n = Point2f(pointflow_now[i].x, pointflow_now[i].y);
                //画出所有选定的点下一帧的位置
                circle(output, pointflow_nowcp[i], 2, CV_RGB(255,0,0), -1);
            }
            swap(gray_prev, gray);
        }
    }

//---------------------------------------【计算细胞内的运动】--------------------------

//    //对细胞内的区域进行检测,稀疏光流法
//    if(cell_points[0].size() == 0)
//    {
//        cell_points[0] = firstNode;//初始化特征点的位置
//    }
//    //进行光流计算，得到cell_points[1]的值
//    cvtColor(frame, cell_flow_gray, COLOR_BGR2GRAY);                 //将输入图像转换为灰度图
//    if(cell_flow_pre.empty())
//    {
//        cell_flow_gray.copyTo(cell_flow_pre);
//    }
//    calcOpticalFlowPyrLK(cell_flow_pre, cell_flow_gray, cell_points[0], cell_points[1], status, err);
//    //画出预测得到的特征点的位置
//    for (int j = 0; j < cell_points[1].size(); j++)
//    {
//        circle(output, cell_points[1][j], 1, Scalar(0,100,200),-1);
//        //画箭头
//        drawarrow(output, cell_points[0][j], cell_points[1][j], 5);
//
//    }
//    swap(cell_points[1], cell_points[0]);
//    swap(cell_flow_pre, cell_flow_gray);


//-------------------------------------------【显示最终的画面】----------------------------------------
    imshow(window_name, output);

}


//获得标志点
Point getNode(Mat frame_n)
{
    detectEdge detedge;                                              //实例化一个对象，用于检测当前帧边缘的标志点
    node_ROI = detedge.detectCell(frame_n);                          //返回基于ROI区域的标志点的坐标
    node_img = detedge.get_node(node_ROI);                           //返回基于原始图的标志点的坐标
    return node_img;
}

void drawarrow(Mat output, Point2f f_fxy, Point2f f_now, int n)
{
    Scalar mycolor[10];
    mycolor[0]=CV_RGB(0,0,170);
    mycolor[1]=CV_RGB(0,0,255);
    mycolor[2]=CV_RGB(0,85,255);
    mycolor[3]=CV_RGB(0,170,255);
    mycolor[4]=CV_RGB(0,255,255);
    mycolor[5]=CV_RGB(85,255,170);
    mycolor[6]=CV_RGB(170,255,85);
    mycolor[7]=CV_RGB(255,255,0);
    mycolor[8]=CV_RGB(255,170,0);
    mycolor[9]=CV_RGB(255,85,0);

    //double ff = sqrt(f_fxy.x*f_fxy.x + f_fxy.y*f_fxy.y);        //当前点的速度
    //int v_b = ceil(fabs(ff)*(10/n))-1;
    //if(v_b<0) {v_b=0;}
    //if(v_b>9) {v_b=9;}
    Point2f f_pre = f_now - f_fxy;
    //line(output, f_pre, f_now, Scalar(255,0,0),2);              //移动轨迹的线
    //画箭头
    Point2f arrow1;
    Point2f arrow2;
    double xx = f_fxy.x;
    double yy = f_fxy.y;
    double angle = atan2(yy, xx);
    arrow1.x = f_now.x + cos(angle+pi*15/180);
    arrow1.y = f_now.y + sin(angle+pi*15/180);
    line(output, f_now, arrow1, Scalar(255,0,0),2);
    arrow2.x = f_now.x + cos(angle-pi*15/180);
    arrow2.y = f_now.y + sin(angle-pi*15/180);
    line(output, f_now, arrow2, Scalar(255,0,0),2);

    circle(output, f_pre, 2, Scalar(255,0,0),-1);
}
// 制作颜色系统
void makecolorwheel(vector<Scalar> &colorwheel)
{
    int RY = 15;
    int YG = 6;
    int GC = 4;
    int CB = 11;
    int BM = 13;
    int MR = 6;

    int i;

    for (i = 0; i < RY; i++)   { colorwheel.push_back(Scalar(255,           255*i/RY,       0)); }
    for (i = 0; i < YG; i++)   { colorwheel.push_back(Scalar(255-255*i/YG,  255,            0)); }
    for (i = 0; i < RY; i++)   { colorwheel.push_back(Scalar(0,             255,            255*i/GC)); }
    for (i = 0; i < CB; i++)   { colorwheel.push_back(Scalar(0,             255-255*i/CB,   255));}
    for (i = 0; i < BM; i++)   { colorwheel.push_back(Scalar(255*i/BM,      0,              255));}
    for (i = 0; i < MR; i++)   { colorwheel.push_back(Scalar(255,           0,              255-255*i/MR));}
}
// 孟塞尔颜色系统函数，用于光流法显示追踪结果，光流计算结果
// 不同颜色表示不同的运动方向，深浅就表示运动的快慢了
// 可以根据所需要的情况进行修改，细胞内需要方向和速度快慢，针管内需要速度即可
void motionToColor(Mat flow, Mat &color)
{
    if(color.empty())
    {
        color.create(flow.rows, flow.cols, CV_8UC3);
    }
    static vector<Scalar> colorwheel;
    if(colorwheel.empty())
    {
        makecolorwheel(colorwheel);                                   // 创建新的颜色系统
    }

    float maxrad = -1;                                                // 决定移动的范围
    //找到最大的光流用来初始化fx和fy
    for (int i = 0; i < flow.rows; ++i)
    {
        for (int j = 0; j < flow.cols; ++j)
        {
            Vec2f flow_at_point = flow.at<Vec2f>(i, j);
            float fx = flow_at_point[0];
            float fy = flow_at_point[1];
            if((fabs(fx) > UNKNOWN_FLOW_THRESH) || fabs(fy) > UNKNOWN_FLOW_THRESH)
            {
                continue;
            }
            float rad = sqrt(fx*fx + fy*fy);
            maxrad = maxrad > rad? maxrad : rad;
        }
    }
    for (int i= 0; i < flow.rows; ++i)
    {
        for (int j = 0; j < flow.cols; ++j)
        {
            uchar *data = color.data + color.step[0] * i + color.step[1] * j;
            Vec2f flow_at_point = flow.at<Vec2f>(i, j);

            float fx = flow_at_point[0] / maxrad;
            float fy = flow_at_point[1] / maxrad;
            if ((fabs(fx) >  UNKNOWN_FLOW_THRESH) || (fabs(fy) >  UNKNOWN_FLOW_THRESH))
            {
                data[0] = data[1] = data[2] = 0;
                continue;
            }
            float rad = sqrt(fx * fx + fy * fy);

            float angle = atan2(-fy, -fx) / CV_PI;
            float fk = (angle + 1.0) / 2.0 * (colorwheel.size()-1);
            int k0 = (int)fk;
            int k1 = (k0 + 1) % colorwheel.size();
            float f = fk - k0;
            //f = 0; // uncomment to see original color wheel

            for (int b = 0; b < 3; b++)
            {
                float col0 = colorwheel[k0][b] / 255.0;
                float col1 = colorwheel[k1][b] / 255.0;
                float col = (1 - f) * col0 + f * col1;
                if (rad <= 1)
                    col = 1 - rad * (1 - col); // increase saturation with radius
                else
                    col *= .75; // out of range
                data[2 - b] = (int)(255.0 * col);
            }
        }
    }
}
// 针管内的孟塞尔颜色系统，深浅表快慢，二维的图像，所以，颜色可以少几样，