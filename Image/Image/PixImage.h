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
	int ImageHeight, ImageWidth; //记录图像的宽高
	char *ImageName; //记录图像的名称
	bool grayDefault; //是否以opencv默认的灰度获取方法获取图像灰度矩阵
private:
	IplImage *img, *imgGray; //图像读取的指针变量
	int Sobel_OperatorX[3][3] = 
	{ { -1,0,1 },
	  { -2,0,2 },
	  { -1,0,1 } }; //横向sobel算子
	int Sobel_OperatorY[3][3] = 
	{ { 1,2,1 },
	  { 0,0,0 },
	  { -1,-2,-1 } }; //纵向sobel算子
public:
	PixImage(char *fileName, bool isGrayDefault = false) :img(cvLoadImage(fileName)), imgGray(cvLoadImage(fileName, 0)) 
	{
		if (!img) 
		{
			cout <<"图像打开错误" << endl;
			return;
		}
		//功能:利用opencv库进行图像的读取操作以及一些成员变量的初始化.
		//isGrayDefault:是否以opencv默认的灰度获取方法获取图像灰度矩阵.
		grayDefault = isGrayDefault;
		//获取图片名
		if (strrchr(fileName, '\\'))
			ImageName = strrchr(fileName, '\\') + 1;
		else 
			ImageName = fileName;

		if (img->height != imgGray->height || img->width != imgGray->width)
		{
			cout << "RGB或者灰度读取错误" << endl;
			return;
		}

		ImageHeight = img->height; //获取图像高度
		ImageWidth = img->width; //获取图像宽度
		Load_RGB(); //将图像RGB值读取到RGB_Array中
		Load_Gray(grayDefault); //将图像灰度值读取到Gray_Array中
	}
	CvScalar Get_RGB(int i, int j);//获取图像第i行第j列的rgb值
								  //直接调用cvGet2D的方法获取该位置rgb值
	double Get_RGB(int i, int j, int k);//获取图像第i行第j列的rgb值
	void Set_RGB(int i, int j, double *value);//将图像第i行第j列的像素设置为value参数:
	void Set_RGB(int i, int j, CvScalar value);//将图像第i行第j列的像素设置为value参数:
	void Set_Gray(int i, int j, double value);//设置图像第i行第j列的灰度值为value
	double getGray(int i, int j, bool isDefault);//获取图像第i行第j列的灰度值
	void Blurring(int n, int m = 1);//以均值滤波器将图像作平滑模糊处理
	void Sobel(double scale = 4);//以sobel算子处理图像，取得各位置的梯度值
	void Save_Image();//保存图像
	void Save_Image(char *savePath);//保存图像至其它路径
	void Save_ImageGray();//保存灰度图像
	void Save_ImageGray(char *savePath);//保存灰度图像至其他路径

private:
	void Load_RGB();//遍历图像的每一个像素,并调用Get_RGB方法获取各位置的像素值,并记录至RGB_Array矩阵中
	void Save_RGB();//遍历RGB_Array矩阵并实际更新到实际图像的每一个像素
	void Load_Gray(bool isDefault = false);//遍历图像的每一个像素,并调用getGray方法获取各位置的灰度值,并记录至Gray_Array矩阵中
	void Save_Gray();//遍历Gray_Array矩阵并实际更新到实际灰度图像的每一个像素
	void Sum_Average_RGB(double *rgb, int ci, int cj, int n);//计算图像第ci行第cj列像素周围n*n个像素的rgb平均值,并记录到rgb向量中
	void Mid_Average_RGB(double * rgb, int ci, int cj, int n);//计算图像第ci行第cj列像素周围n*n个像素的中值,用于中值滤波器实现，并记录到rgb向量中
	double Gxy(int ci, int cj);//利用sobel算子计算ci行cj列处灰度值的梯度,并返回
	double Sobel_Thresh(double scale);//计算sobel阈值
};


