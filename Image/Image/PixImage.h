#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <io.h>


using namespace std;
using namespace cv;



class PixImage {
public:
	int ImageHeight, ImageWidth; //��¼ͼ��Ŀ��
	char *ImageName; //��¼ͼ�������
	bool grayDefault; //�Ƿ���opencvĬ�ϵĻҶȻ�ȡ������ȡͼ��ҶȾ���
private:
	IplImage *img, *imgGray; //ͼ���ȡ��ָ�����
	int Sobel_OperatorX[3][3] = 
	{ { -1,0,1 },
	  { -2,0,2 },
	  { -1,0,1 } }; //����sobel����
	int Sobel_OperatorY[3][3] = 
	{ { 1,2,1 },
	  { 0,0,0 },
	  { -1,-2,-1 } }; //����sobel����
public:
	PixImage(char *fileName, bool isGrayDefault = false) :img(cvLoadImage(fileName)), imgGray(cvLoadImage(fileName, 0)) 
	{
		if (!img) 
		{
			cout <<"ͼ��򿪴���" << endl;
			return;
		}
		//����:����opencv�����ͼ��Ķ�ȡ�����Լ�һЩ��Ա�����ĳ�ʼ��.
		//isGrayDefault:�Ƿ���opencvĬ�ϵĻҶȻ�ȡ������ȡͼ��ҶȾ���.
		grayDefault = isGrayDefault;
		//��ȡͼƬ��
		if (strrchr(fileName, '\\'))
			ImageName = strrchr(fileName, '\\') + 1;
		else 
			ImageName = fileName;

		if (img->height != imgGray->height || img->width != imgGray->width)
		{
			cout << "RGB���߻Ҷȶ�ȡ����" << endl;
			return;
		}

		ImageHeight = img->height; //��ȡͼ��߶�
		ImageWidth = img->width; //��ȡͼ����
		Load_RGB(); //��ͼ��RGBֵ��ȡ��RGB_Array��
		Load_Gray(grayDefault); //��ͼ��Ҷ�ֵ��ȡ��Gray_Array��
	}
	CvScalar Get_RGB(int i, int j);//��ȡͼ���i�е�j�е�rgbֵ
								  //ֱ�ӵ���cvGet2D�ķ�����ȡ��λ��rgbֵ
	double Get_RGB(int i, int j, int k);//��ȡͼ���i�е�j�е�rgbֵ
	void Set_RGB(int i, int j, double *value);//��ͼ���i�е�j�е���������Ϊvalue����:
	void Set_RGB(int i, int j, CvScalar value);//��ͼ���i�е�j�е���������Ϊvalue����:
	void Set_Gray(int i, int j, double value);//����ͼ���i�е�j�еĻҶ�ֵΪvalue
	double getGray(int i, int j, bool isDefault);//��ȡͼ���i�е�j�еĻҶ�ֵ
	void Blurring(int n, int m = 1);//�Ծ�ֵ�˲�����ͼ����ƽ��ģ������
	void Sobel(double scale = 4);//��sobel���Ӵ���ͼ��ȡ�ø�λ�õ��ݶ�ֵ
	void Save_Image();//����ͼ��
	void Save_Image(char *savePath);//����ͼ��������·��
	void Save_ImageGray();//����Ҷ�ͼ��
	void Save_ImageGray(char *savePath);//����Ҷ�ͼ��������·��

private:
	void Load_RGB();//����ͼ���ÿһ������,������Get_RGB������ȡ��λ�õ�����ֵ,����¼��RGB_Array������
	void Save_RGB();//����RGB_Array����ʵ�ʸ��µ�ʵ��ͼ���ÿһ������
	void Load_Gray(bool isDefault = false);//����ͼ���ÿһ������,������getGray������ȡ��λ�õĻҶ�ֵ,����¼��Gray_Array������
	void Save_Gray();//����Gray_Array����ʵ�ʸ��µ�ʵ�ʻҶ�ͼ���ÿһ������
	void Sum_Average_RGB(double *rgb, int ci, int cj, int n);//����ͼ���ci�е�cj��������Χn*n�����ص�rgbƽ��ֵ,����¼��rgb������
	void Mid_Average_RGB(double * rgb, int ci, int cj, int n);//����ͼ���ci�е�cj��������Χn*n�����ص���ֵ,������ֵ�˲���ʵ�֣�����¼��rgb������
	double Gxy(int ci, int cj);//����sobel���Ӽ���ci��cj�д��Ҷ�ֵ���ݶ�,������
	double Sobel_Thresh(double scale);//����sobel��ֵ
};


