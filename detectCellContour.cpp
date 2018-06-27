//
// Created by arcstone_mems on 2018/6/8.
//

#include "detectCellContour.h"


vector<Point2f> detectCellContour::detect_hough_circle(Mat inputImage,int img_h, int img_w)
{
    Mat cell_midimage,inputImage_roi;
    Point cell_center;                                                               //定义细胞的中心
    int cell_img_w = inputImage.cols;
    int cell_img_h = inputImage.rows;
    inputImage_roi = inputImage(Rect(img_w*0.07, img_h*0.25, img_w*0.30, img_h*0.5));//定义感兴趣的检测区域
    cvtColor(inputImage_roi, cell_midimage,COLOR_BGR2GRAY);//将图像转为灰度图
    //GaussianBlur(cell_midimage,cell_midimage,Size(9,9),2,2);
    //imshow("滤波后", cell_midimage);
    //waitKey(0);
    //进行霍夫圆变换
    HoughCircles(cell_midimage, cell_circles,HOUGH_GRADIENT,1.5,20,130,130,0,0);
    cout<<"当前检测到的圆个数为："<<cell_circles.size()<<endl;
    if(cell_circles.size() != 1)
    {
        cout<<"请重新调整霍夫圆检测的参数"<<endl;
    }
    else
    {
        cell_center.x = cvRound(cell_circles[0][0]) + img_w*0.07;
        cell_center.y = cvRound(cell_circles[0][1]) + img_h*0.25;
        int radius = cvRound(cell_circles[0][2]);
        circle(inputImage, cell_center, 3, Scalar(0,255,0), -1, 8, 0);
        circle(inputImage, cell_center, radius, Scalar(155,50,255), 1 , 8, 0);
        //细胞的外轮廓

        radius_out = radius + 30;
        circle(inputImage, cell_center, radius_out, Scalar(155,50,0), 2, 8, 0);
    }
    //画出需要检测区域，之后再该区域取点，假如点距离圆心的距离小于半径，那么就在圆内
    for (int y = 0; y < cell_img_h; y+=10)
    {
        for (int x = 0; x < cell_img_w; x+=10)
        {
            //计算图像上的点到圆心的距离，如果距离小于半径，那么该点在圆内
            float distance = sqrtf(pow((x - cell_center.x), 2) + pow((y - cell_center.y), 2));
            if(distance < radius_out)
            {
                cell_point_temp.push_back(Point(x,y));
            }
        }
    }
    cell_point1 = cell_point_temp;
//    for (int k = 0; k < cell_point1.size(); k++)
//    {
//        //画出容器内的所有点
//        circle(inputImage, Point2f(cell_point1[k].x,cell_point1[k].y),1,Scalar(0,255,0),-1,8);
//    }

    //imshow("【效果图】",inputImage);
    //waitKey(0);
    return cell_point1;
    //返回检测到的圆的区域,返回圆心的坐标
    //return cell_center;
}







vector<Point2f> detectCellContour::detectCell(Mat input_Image, int img_h, int img_w)
{
    //只需要检测第一帧的图像就行了，检测第一帧图像保存需要检测的点
    img_cell_roi = input_Image(Rect(0, img_h*0.25, img_w*0.30, img_h*0.5));    //定义感兴趣的检测区域
    cvtColor(img_cell_roi, img_cell_gray, COLOR_BGR2GRAY);
    Canny(img_cell_gray,img_cell_gray1,30,90,3);
    //imshow("此时", img_cell_gray1);
    //waitKey(0);
    //定义核
    Mat element = getStructuringElement(MORPH_RECT, Size(15,15));
    //进行形态学操作
    Mat result;
    morphologyEx(img_cell_gray1, result, cv::MORPH_CLOSE, element);

    //imshow("【膨胀腐蚀】", result);
    //waitKey(0);

    findContours(result, contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE,Point(0,0));
    Mat drawing = Mat::zeros(img_cell_gray1.size(), CV_8UC3);

    for (int i = 0; i < contours.size(); i++)
    {
        //if(contours[i])
        cout<<contours.size()<<endl;
        drawContours(drawing, contours, i, Scalar(255,0,0), 2,8,hierarchy,0,Point());
    }
    //找到轮廓后需要进一步确定圆心
    //?????????????????
    //?????????????????

    //imshow("此时", drawing);
    //选择需要追踪的点，追踪的点都在所选的轮廓内

    for (int j = 0; j < contours.size(); j++)
    {
        //对检测到的每个轮廓进行遍历
        for (int y = 0; y < img_h; y+=10)
        {
            for (int x = 0; x < img_w; x+=10)
            {
                if(pointPolygonTest(contours[j], Point2f(x, y), true)>0)
                {
                    cell_point_temp.push_back(Point2f(x,y));
                }
            }
        }
        if(cell_point_temp.size() > 50)
        {
            //如果在轮廓内的特征点个数大于50个，说明轮廓足够大，是我们想要的那个
            cell_point1 = cell_point_temp;                                     //把得到的点存入cell_point1
            break;
        }
        else
        {
            //是检测到的小轮廓，噪音
            cell_point_temp.clear();
        }
    }

    for (int k = 0; k < cell_point1.size(); k++)
    {
        circle(input_Image, Point2f(cell_point1[k].x,cell_point1[k].y+img_h*0.25),1,Scalar(0,255,0),-1,8);
    }
    //imshow("【细胞内选取点】",input_Image);
    return cell_point1;

    //waitKey(0);
}

vector<Point2f> detectCellContour::flow_in_cell(Mat frame, vector<Point2f> input_points)
{
    cvtColor(frame, cell_flow_gray, COLOR_BGR2GRAY);                 //将输入图像转换为灰度图
    points[0] = input_points;                                        //初始化特征点的位置
    if(cell_flow_pre.empty())
    {
        cell_flow_gray.copyTo(cell_flow_pre);
    }
    calcOpticalFlowPyrLK(cell_flow_pre, cell_flow_gray, points[0], points[1], status, err);

    swap(points[1], points[0]);
    swap(cell_flow_pre, cell_flow_gray);

    return points[1];                                                //返回新的一帧特征点的位置

}
result_circle detectCellContour::detect_hough_circle_center(Mat inputImage, int img_h, int img_w)
{
    Mat cell_midimage,inputImage_roi;
    Point2f cell_center;                                                               //定义细胞的中心
    int cell_img_w = inputImage.cols;
    int cell_img_h = inputImage.rows;
    inputImage_roi = inputImage(Rect(img_w*0.07, img_h*0.25, img_w*0.30, img_h*0.5));//定义感兴趣的检测区域
    cvtColor(inputImage_roi, cell_midimage,COLOR_BGR2GRAY);//将图像转为灰度图
    //GaussianBlur(cell_midimage,cell_midimage,Size(9,9),2,2);
    //imshow("滤波后", cell_midimage);
    //waitKey(0);
    //进行霍夫圆变换
    HoughCircles(cell_midimage, cell_circles,HOUGH_GRADIENT,1.5,20,130,130,0,0);
    cout<<"当前检测到的圆个数为："<<cell_circles.size()<<endl;
    if(cell_circles.size() != 1)
    {
        cout<<"请重新调整霍夫圆检测的参数"<<endl;
    }
    else
    {
        cell_center.x = cvRound(cell_circles[0][0]) + img_w*0.07;
        cell_center.y = cvRound(cell_circles[0][1]) + img_h*0.25;
        int radius = cvRound(cell_circles[0][2]);
        // 画出圆心坐标，以及检测得到的轮廓
        circle(inputImage, cell_center, 3, Scalar(0,255,0), -1, 8, 0);
        circle(inputImage, cell_center, radius, Scalar(155,50,255), 1 , 8, 0);
        //细胞的外轮廓

        radius_out = radius + 30;
        circle(inputImage, cell_center, radius_out, Scalar(155,50,0), 2, 8, 0);
    }
    //画出需要检测区域，之后再该区域取点，假如点距离圆心的距离小于半径，那么就在圆内
    for (int y = 0; y < cell_img_h; y+=10)
    {
        for (int x = 0; x < cell_img_w; x+=10)
        {
            //计算图像上的点到圆心的距离，如果距离小于半径，那么该点在圆内
            float distance = sqrtf(pow((x - cell_center.x), 2) + pow((y - cell_center.y), 2));
            if(distance < radius_out)
            {
                cell_point_temp.push_back(Point(x,y));
            }
        }
    }
    cell_point1 = cell_point_temp;
//    for (int k = 0; k < cell_point1.size(); k++)
//    {
//        //画出容器内的所有点
//        circle(inputImage, Point2f(cell_point1[k].x,cell_point1[k].y),1,Scalar(0,255,0),-1,8);
//    }

    //imshow("【效果图】",inputImage);
    //waitKey(0);
    struct result_circle result_circle1;                             //实例化一个结构体类型，用于存放返回的值
    result_circle1.radius_out = radius_out;
    result_circle1.cell_center = cell_center;
    result_circle1.first_Node = cell_point1;
    return result_circle1;

}

