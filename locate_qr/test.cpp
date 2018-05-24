#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <math.h>

using namespace cv;
using namespace std;

Mat src_all;

//找出二维码
void judge_QR(vector<vector<Point>> contours, vector<vector<Point>>& contours_true)
{
	int n = contours.size();   //提取出的特征数
	RotatedRect *box;
	double area_all = 0;
	vector<int> QR_true;
	RotatedRect temp;
	box = (RotatedRect *)malloc(sizeof(RotatedRect)*n);
	for (int i = 0; i < n; i++)
	{
		box[i] = minAreaRect(Mat(contours[i]));  //找出完整包含轮廓的最小矩形
		cout << box[i].size.area() << endl;
	}

	for (int i = 0; i < n; i++)
	{
		QR_true.clear();
		for (int j = 0; j < n; j++)
		{
			if (i != j)
			{
				//找出面积相等的矩形
				if ((0.65 <= ((float)(box[i].size.area()) / (box[j].size.area()))) && (1.35 >= ((float)(box[i].size.area()) / (box[j].size.area()))))
				{
					QR_true.push_back(j);
				}
			}
		}
		if (QR_true.size() >= 2)
		{
			QR_true.push_back(i);
			break;
		}
	}
	if (QR_true.size() == 3)
	{
		for (int i = 0; i < 3; i++)
		{
			contours_true.push_back(contours[QR_true[i]]);
		}
		free(box);
	}
	else
	{
		printf("读取二维码失败!!~!\n");
		free(box);
		return;
	}
}

//框出二维码
void draw_qd(vector<vector<Point>> contours)
{
	vector<Point> contour_all;
	for (int i = 0; i < contours.size(); i++)
	{
		for (int n = 0; n < contours[i].size(); n++)
		{
			contour_all.push_back(contours[i][n]);   //把所有的坐标放到一个向量中
		}
	}

	RotatedRect box = minAreaRect(Mat(contour_all));  //找出完整包含轮廓的最小矩形
	Point2f vertex[4];   //矩形四个顶点
	box.points(vertex);
	Point vertex_all[4];
	for (int i = 0; i < 4; i++)
	{
		vertex_all[i] = (Point)vertex[i];
	}
	const Point * ppt[1] = { vertex_all };
	int npt[] = { 4 };
	//fillPoly(src_all, ppt,npt,1,Scalar(0,255,0));
	//绘制空心的多边形
	polylines(src_all, ppt, npt, 1, true, Scalar(0, 0, 255), 4);  //用方框将二维码框起来
}

int main()
{
	Mat src_img = imread("QR_code2.jpg");
	//Mat src_img = imread("QR_code4.png");
	resize(src_img, src_img, Size(600, 600));  //调整图片大小,便于处理
	Mat src_gray;  //灰度图
	Mat threshold_output;  //二值化后的图
	src_all = src_img.clone();

	vector<vector<Point>> contours_true;
	//每一组Point点集就是一个轮廓,有多少轮廓，向量contours就有多少元素
	vector<vector<Point>> contours, contours2; //向量内每个元素保存一组由连续Point点构成的点的集合的向量
											   //向量内每一个元素包含了4个int型变量的向量
											   //向量hiararchy内的元素和轮廓向量contours内的元素是一一对应的
											   //后一个轮廓、前一个轮廓、内嵌轮廓,父轮廓的索引编号
	vector<Vec4i> hierarchy;

	cvtColor(src_img, src_gray, CV_BGR2GRAY);   //将图片转换为灰度图
												//blur(src_gray,src_gray,Size(3,3));   //均值滤波,模糊噪声
												//THRESH_OTSU 自适应阈值
	threshold(src_gray, threshold_output, 100, 255, THRESH_BINARY | THRESH_OTSU);

	//寻找轮廓
	//第一个参数是 输入图像 二值化
	//第二个参数是 内存存储器,找到的轮廓放到该向量中
	//第三个参数是 轮廓的层次关系 (后一个,前一个,内嵌轮廓，父轮廓的索引)
	//第四个参数是 采用树结构
	//第五个参数是 寻找所有的轮廓坐标点
	findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CHAIN_APPROX_NONE, Point(0, 0));

	int k, c = 0;
	for (int i = 0; i < contours.size(); i++)  //查找所有轮廓
	{
		k = i;
		c = 0;
		while (hierarchy[k][2] != -1)  //内有嵌套轮廓
		{
			k = hierarchy[k][2];
			c++;
		}
		if (c == 2)  //内嵌两个轮廓的是二维码的定位点
		{
			contours2.push_back(contours[i]);
		}
	}

	//填充定位点
	//for (int i = 0; i < contours2.size(); i++)
	//{
	//	drawContours(src_all, contours2, i, CV_RGB(0, 255, 0), -1);
	//}

	//找出二维码
	judge_QR(contours2, contours_true);
	//填充定位点
	for (int i = 0; i < contours_true.size(); i++)
	{
		drawContours(src_all, contours_true, i, CV_RGB(0, 255, 0), -1);
	}

	//框出二维码的位置
	draw_qd(contours_true);

	imshow("原始图", src_img);
	imshow("二值图像", threshold_output);
	imshow("效果图", src_all);
	waitKey(0);
	return 0;
}