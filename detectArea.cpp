//////
////// Created by nku_zth on 2018/4/19.
//////
////
////检测视频中的边界，实时反馈出新的感兴趣的边界
////稳定性不够，有时候能检测出来，有时候检测不出来
////标志点的y坐标实际上是已知的，也就是中间位置，只要能够检测到抽出物质最外侧的一部分就可以
////设置一个逻辑， 假如到了选取点的时候，未能检测到标志点，那么继续播放一帧，再进行检测，如果还是检测不到，继续播放，直到检测到标志点为止
////怎么算是检测到了想要的点，因为y的位置固定，x不固定，只要x对就行，怎么看x对不对，x必须有2个及以上，还有x不能太大，到边界了，第二个点就是所需要的标志点
//
////改进方法。腐蚀不要太严重，使得抽出物质的最外侧能够一直检测到，改参改参
//
//#include <iostream>
//#include <algorithm>
//#include <opencv2/opencv.hpp>
//#define WINDOW_NAME1 "【原始图窗口】"
//#define WINDOW_NAME2 "【轮廓图】"
//
//
//using namespace std;
//using namespace cv;
//
////--------------------------------【定义全局变量】-----------------------------------
//
//Mat det_firstImage;
//Mat det_frame;
//Mat img_detect;
//Mat img_gray;
//Mat det_dst, det_edge, det_gray, det_dst2, det_dst3, det_dst4, det_dst5, det_dst6;
//int g_nThresh = 80;
//int g_nThresh_max = 255;
//RNG g_rng(12345);
//vector<vector<Point> > g_vContours;
//vector<Vec4i> g_vHierarchy;
//Mat output_img;
////----------------------------------【定义全局函数】----------------------------------
//void on_ThreshChange(int, void*);
//void swap(int *a, int *b);
//int sorted(vector<Point> nodeList);
////-----------------------------------【主函数】----------------------------------------
//int detectEdge(Mat filename)
//{
//    img_detect = filename;
//    img_detect = imread("/Users/arcstone_mems_108/Desktop/keyan/cvproject/cell2/cmake-build-debug/10/146.jpg");
//    int img_h  = img_detect.rows;
//    int img_w = img_detect.cols;
//    Mat src = img_detect(Rect(0.25*img_w, 0.45*img_h, 0.6*img_w, 0.12*img_h));
//    cout<<"所截取图像的中间位置水平线坐标为"<<int(0.06*img_h)<<endl;
//    if(!src.data)
//    {
//        cout<<"读取图片错误！"<<endl;
//    }
//    cvtColor(src, img_gray, COLOR_BGR2GRAY);
//    Mat grad_x, grad_y;
//    Mat abs_grad_x, abs_grad_y;
//    Sobel(img_gray, grad_x, CV_32F, 1 ,0, 3,1,1,BORDER_DEFAULT);
//    convertScaleAbs(grad_x, abs_grad_x);
//    Sobel(img_gray, grad_y, CV_32F, 0, 1,3,1,1,BORDER_DEFAULT);
//    convertScaleAbs(grad_y, abs_grad_y);
//
//    addWeighted(abs_grad_x, 0.5,abs_grad_y, 0.5,0,det_dst);
//    //imshow("dst", dst);
//    //waitKey(0);
//
//    blur(det_dst, det_dst, Size(3, 3) );
//    //imshow("滤波后", dst);
//    //waitKey(0);
//    //二值化
//    threshold(det_dst, det_dst2, 0, 255, THRESH_OTSU);
//    //imshow("二值化", dst2);
//    //waitKey(0);
//
////    //形态学运算,定义核
//    Mat kernel = getStructuringElement(MORPH_RECT, Size(10, 10));
//    //Mat kernel2 = getStructuringElement(MORPH_RECT, Size(4, 4));
//    //erode(dst2, dst3, kernel);
//    dilate(det_dst2, det_dst4, kernel);
//    //dilate(dst4, dst4, kernel2);
//    imshow("膨胀", det_dst4);
//    waitKey(0);
//
//
//    findContours(det_dst4, g_vContours, g_vHierarchy, RETR_LIST,  CHAIN_APPROX_NONE, Point(0,0));
//    cout<<"有多少个轮廓"<<g_vContours.size()<<endl;
//    //定义一个逻辑，当findContours获得的轮廓数目太多时，加大膨胀的参数，继续膨胀，最终只保留两个轮廓
//    int k = 1;
//    while (g_vContours.size() > 2)
//    {
//        Mat kernel3 = getStructuringElement(MORPH_RECT, Size(k, k));
//        dilate(det_dst4, det_dst4, kernel3);
//        findContours(det_dst4, g_vContours, g_vHierarchy, RETR_LIST, CHAIN_APPROX_NONE, Point(0, 0));
//        cout<<"当前有"<<g_vContours.size()<<"个轮廓"<<endl;
//        k = k + 1;
//    }
//    //画出最终识别到的轮廓
//    Mat drawing = Mat::zeros(det_dst4.size(), CV_8UC3);
//    for (int i = 0; i < g_vContours.size(); i++)
//    {
//        Scalar color = Scalar(g_rng.uniform(0,255), g_rng.uniform(0, 255), g_rng.uniform(0, 255));
//        drawContours(drawing, g_vContours, i , color,1,8,g_vHierarchy,0,Point());
//        //第一次绘制的轮廓为我们需要的
//        imshow(WINDOW_NAME2, drawing);
//        waitKey(0);
//    }
//    //当前为两个轮廓，一个外层轮廓，一个内层轮廓,
//    cout<<"hierarchy为"<<g_vHierarchy.size()<<endl;
//    cout<<"第一个轮廓"<<g_vHierarchy[0][0]<<g_vHierarchy[0][1]<<g_vHierarchy[0][2]<<g_vHierarchy[0][3]<<endl;
//    cout<<"第二个轮廓"<<g_vHierarchy[1][0]<<g_vHierarchy[1][1]<<g_vHierarchy[1][2]<<g_vHierarchy[1][3]<<endl;
//    vector<Point> node_std;
//
//    //当前为两个轮廓或者一个轮廓，如何获得有所需要边界的那个轮廓，这个轮廓有什么特征
//    //有所需要轮廓的那个边界
//    //先取内侧的轮廓，如果内侧的轮廓的两个标志点距离大于30，那么是该轮廓，否则检测另一个轮廓
//    if(g_vContours.size() == 1)
//    {
//        for(int i = 0; i<g_vContours[0].size(); i++)
//        {
//            if(g_vContours[0][i].y == int(0.06*img_h))
//            {
//                cout<<"标志点的坐标为"<<"("<<g_vContours[0][i].x<<","<<g_vContours[0][i].y<<")"<<endl;
//                node_std.push_back(g_vContours[0][i]);
//            }
//            else
//            {
//                continue;
//            }
//        }
//        if(node_std.size() == 2 && abs(node_std[0].x - node_std[1].x) > 30)
//        {
//            cout<<"筛选得到的标志点为"<<node_std[1]<<endl;
//        }
//        else
//        {
//            cout<<"有问题"<<endl;
//        }
//    }
//    else if(g_vContours[0].size() < g_vContours[1].size())
//    {
//        //计算出所有可能的标志点的坐标
//        for(int i = 0; i<g_vContours[0].size(); i++)
//        {
//            if(g_vContours[0][i].y == int(0.06*img_h))
//            {
//                cout<<"标志点的坐标为"<<"("<<g_vContours[0][i].x<<","<<g_vContours[0][i].y<<")"<<endl;
//                node_std.push_back(g_vContours[0][i]);
//            }
//            else
//            {
//                continue;
//            }
//        }
//        //如果没有检测到轮廓或者检测到的点距离太进，那么不是这个轮廓，为下一个轮廓
//        if(node_std.size() == 0 || abs(node_std[0].x - node_std[1].x) < 30 )
//        {
//            cout<<"不是这个轮廓，自动切换下一个轮廓检测！"<<endl;
//            node_std.clear();
//            for(int j = 0; j<g_vContours[1].size(); j++)
//            {
//                if(g_vContours[1][j].y == int(0.06*img_h))
//                {
//                    cout<<"标志点的坐标为"<<"("<<g_vContours[1][j].x<<","<<g_vContours[1][j].y<<")"<<endl;
//                    node_std.push_back(g_vContours[1][j]);
//                }
//                else
//                {
//                    continue;
//                }
//            }
//            if((node_std.size() == 2) && abs(node_std[0].x - node_std[1].x) > 30 )
//            {
//                cout<<"筛选得到的标志点为"<<node_std[1]<<endl;
//            }
//        }
//        else if((node_std.size() >= 2) && abs(node_std[0].x - node_std[1].x) > 30)
//        {
//            //对node_std里边的所有点进行排序，最小的点就是标志点
//            cout<<"筛选得到的标志点为"<<"("<<sorted(node_std)<<","<<"34)"<<endl;
//        }
//        else
//        {
//            cout<<"有问题"<<endl;
//        }
//    }
//    else
//    {
//        //计算出标志点的坐标
//        for(int i = 0; i<g_vContours[1].size(); i++)
//        {
//            if(g_vContours[1][i].y == int(0.06*img_h))
//            {
//                cout<<"标志点的坐标为"<<"("<<g_vContours[1][i].x<<","<<g_vContours[1][i].y<<")"<<endl;
//                node_std.push_back(g_vContours[1][i]);
//            }
//            else
//            {
//                continue;
//            }
//        }
//        if(abs(node_std[0].x - node_std[1].x) < 30)
//        {
//            cout<<"不是这个轮廓，自动切换到下一个轮廓！"<<endl;
//            node_std.clear();
//            for(int j = 0; j<g_vContours[0].size(); j++)
//            {
//                if(g_vContours[0][j].y == int(0.06*img_h))
//                {
//                    cout<<"标志点的坐标为"<<"("<<g_vContours[0][j].x<<","<<g_vContours[0][j].y<<")"<<endl;
//                    node_std.push_back(g_vContours[0][j]);
//                }
//                else
//                {
//                    continue;
//                }
//            }
//            if((node_std.size() == 2) && abs(node_std[0].x - node_std[1].x) > 30 )
//            {
//                cout<<"筛选得到的标志点为"<<node_std[1]<<endl;
//            }
//
//        }
//        if(node_std.size() == 2 && abs(node_std[0].x - node_std[1].x) > 30)
//        {
//            cout<<"筛选得到的标志点为"<<node_std[1]<<endl;
//        }
//        else
//        {
//            cout<<"有问题"<<endl;
//        }
//    }
//}
//void on_ThreshChange(int, void*)
//{
//    Canny(img_gray, output_img, g_nThresh, g_nThresh*3, 3);
//    findContours(output_img, g_vContours, g_vHierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0,0));
//    Mat drawing = Mat::zeros(output_img.size(), CV_8UC3);
//
//    for (int i = 0; i < g_vContours.size(); i++)
//    {
//        Scalar color = Scalar(g_rng.uniform(0,255), g_rng.uniform(0, 255), g_rng.uniform(0, 255));
//        drawContours(drawing, g_vContours, i , color,2,8,g_vHierarchy,0,Point());
//        imshow(WINDOW_NAME2, drawing);
//    }
//
//}
//
//void swap(int *a, int *b)
//{
//    int temp;
//    temp = *a;
//    *a = *b;
//    *b = temp;
//}
//// 对获得所有标志点进行排序，从小到大
//int sorted(vector<Point> nodeList)
//{
//    int temp = 1000;
//    for(int i = 0; i < nodeList.size();i++)
//    {
//        if(nodeList[i].x < temp)
//        {
//            swap(&nodeList[i].x, &temp);
//        }
//    }
//    return temp;
//}
//
//
