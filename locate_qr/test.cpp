#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <math.h>

using namespace cv;
using namespace std;

Mat src_all;

//�ҳ���ά��
void judge_QR(vector<vector<Point>> contours, vector<vector<Point>>& contours_true)
{
	int n = contours.size();   //��ȡ����������
	RotatedRect *box;
	double area_all = 0;
	vector<int> QR_true;
	RotatedRect temp;
	box = (RotatedRect *)malloc(sizeof(RotatedRect)*n);
	for (int i = 0; i < n; i++)
	{
		box[i] = minAreaRect(Mat(contours[i]));  //�ҳ�����������������С����
		cout << box[i].size.area() << endl;
	}

	for (int i = 0; i < n; i++)
	{
		QR_true.clear();
		for (int j = 0; j < n; j++)
		{
			if (i != j)
			{
				//�ҳ������ȵľ���
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
		printf("��ȡ��ά��ʧ��!!~!\n");
		free(box);
		return;
	}
}

//�����ά��
void draw_qd(vector<vector<Point>> contours)
{
	vector<Point> contour_all;
	for (int i = 0; i < contours.size(); i++)
	{
		for (int n = 0; n < contours[i].size(); n++)
		{
			contour_all.push_back(contours[i][n]);   //�����е�����ŵ�һ��������
		}
	}

	RotatedRect box = minAreaRect(Mat(contour_all));  //�ҳ�����������������С����
	Point2f vertex[4];   //�����ĸ�����
	box.points(vertex);
	Point vertex_all[4];
	for (int i = 0; i < 4; i++)
	{
		vertex_all[i] = (Point)vertex[i];
	}
	const Point * ppt[1] = { vertex_all };
	int npt[] = { 4 };
	//fillPoly(src_all, ppt,npt,1,Scalar(0,255,0));
	//���ƿ��ĵĶ����
	polylines(src_all, ppt, npt, 1, true, Scalar(0, 0, 255), 4);  //�÷��򽫶�ά�������
}

int main()
{
	Mat src_img = imread("QR_code2.jpg");
	//Mat src_img = imread("QR_code4.png");
	resize(src_img, src_img, Size(600, 600));  //����ͼƬ��С,���ڴ���
	Mat src_gray;  //�Ҷ�ͼ
	Mat threshold_output;  //��ֵ�����ͼ
	src_all = src_img.clone();

	vector<vector<Point>> contours_true;
	//ÿһ��Point�㼯����һ������,�ж�������������contours���ж���Ԫ��
	vector<vector<Point>> contours, contours2; //������ÿ��Ԫ�ر���һ��������Point�㹹�ɵĵ�ļ��ϵ�����
											   //������ÿһ��Ԫ�ذ�����4��int�ͱ���������
											   //����hiararchy�ڵ�Ԫ�غ���������contours�ڵ�Ԫ����һһ��Ӧ��
											   //��һ��������ǰһ����������Ƕ����,���������������
	vector<Vec4i> hierarchy;

	cvtColor(src_img, src_gray, CV_BGR2GRAY);   //��ͼƬת��Ϊ�Ҷ�ͼ
												//blur(src_gray,src_gray,Size(3,3));   //��ֵ�˲�,ģ������
												//THRESH_OTSU ����Ӧ��ֵ
	threshold(src_gray, threshold_output, 100, 255, THRESH_BINARY | THRESH_OTSU);

	//Ѱ������
	//��һ�������� ����ͼ�� ��ֵ��
	//�ڶ��������� �ڴ�洢��,�ҵ��������ŵ���������
	//������������ �����Ĳ�ι�ϵ (��һ��,ǰһ��,��Ƕ������������������)
	//���ĸ������� �������ṹ
	//����������� Ѱ�����е����������
	findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CHAIN_APPROX_NONE, Point(0, 0));

	int k, c = 0;
	for (int i = 0; i < contours.size(); i++)  //������������
	{
		k = i;
		c = 0;
		while (hierarchy[k][2] != -1)  //����Ƕ������
		{
			k = hierarchy[k][2];
			c++;
		}
		if (c == 2)  //��Ƕ�����������Ƕ�ά��Ķ�λ��
		{
			contours2.push_back(contours[i]);
		}
	}

	//��䶨λ��
	//for (int i = 0; i < contours2.size(); i++)
	//{
	//	drawContours(src_all, contours2, i, CV_RGB(0, 255, 0), -1);
	//}

	//�ҳ���ά��
	judge_QR(contours2, contours_true);
	//��䶨λ��
	for (int i = 0; i < contours_true.size(); i++)
	{
		drawContours(src_all, contours_true, i, CV_RGB(0, 255, 0), -1);
	}

	//�����ά���λ��
	draw_qd(contours_true);

	imshow("ԭʼͼ", src_img);
	imshow("��ֵͼ��", threshold_output);
	imshow("Ч��ͼ", src_all);
	waitKey(0);
	return 0;
}