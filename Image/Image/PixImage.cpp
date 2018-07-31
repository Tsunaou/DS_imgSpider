#include "PixImage.h"
#include <vector>
#include<algorithm>

extern const int MAX = 5000;
extern double RGB_Array[MAX][MAX][3];
extern double Gray_Array[MAX][MAX];

CvScalar PixImage::Get_RGB(int i, int j)
{
	return cvGet2D(img, i, j);
}

double PixImage::Get_RGB(int i, int j, int k)
{
	return cvGet2D(img, i, j).val[k];
}

void PixImage::Set_RGB(int i, int j, double * value)
{
	CvScalar rgb; //声明一个CvScalar向量,并分别取出rgb值然后赋值
	rgb.val[0] = value[0];
	rgb.val[1] = value[1];
	rgb.val[2] = value[2];
	cvSet2D(img, i, j, rgb); //opencv自带方法，用以设置img图像中第i行第j列处的像素为rgb
}

void PixImage::Set_RGB(int i, int j, CvScalar value)
{
	cvSet2D(img, i, j, value);

}

void PixImage::Set_Gray(int i, int j, double value)
{
	CvScalar s;
	s.val[0] = value;
	s.val[1] = s.val[2] = 0;
	cvSet2D(imgGray, i, j, s);
}

double PixImage::getGray(int i, int j, bool isDefault)//isDefault:是否以opencv默认的灰度获取方法获取图像灰度值
{
	if (!isDefault)
		return (RGB_Array[i][j][0] * 299 +RGB_Array[i][j][1] * 587 +RGB_Array[i][j][2] * 114 + 500) / 1000; //rgb转灰度值公式
	else
		return cvGet2D(imgGray, i, j).val[0]; //opencv以iscolor=0的方式读取到的灰度值
}

void PixImage::Blurring(int n, int m)//n:均值像素所取的范围n方,m:处理的迭代次数
{
	if (!img)
		return;
	double *rgb = new double[3];

	while (m > 0) //将均值滤波器迭代m次
	{ 
		for (int i = 0; i < ImageHeight; i++)
		{
			for (int j = 0; j < ImageWidth; j++)
			{
				Sum_Average_RGB(rgb, i, j, n); //分别获取第i行第j列像素周围n*n个像素的rgb平均值
				RGB_Array[i][j][0] = rgb[0];
				RGB_Array[i][j][1] = rgb[1];
				RGB_Array[i][j][2] = rgb[2];
			}
		}
		Save_RGB(); //调用Save_RGB方法更新图像
		m--;
	}
}

void PixImage::Sobel(double scale)//scale:scale系数
{
	if (!img)
		return;

	double t = Sobel_Thresh(scale); //根据给出的公式计算sobel阈值

	for (int i = 1; i < ImageHeight - 1; i++) 
	{
		for (int j = 1; j < ImageWidth - 1; j++) 
		{
			if (Gxy(i, j) >= t)//若梯度大于等于阈值，则置为255
				Gray_Array[i][j] = 255; 
			else //若梯度小于阈值，则置为0
				Gray_Array[i][j] = 0; 
		}
	}
}

void PixImage::Save_Image()
{
	if (!img)
		return;
	Save_RGB(); //调用Save_RGB的方法将RGB_Array中的值全部更新到实际图像中
	cvSaveImage(ImageName, img); //调用opencv自带的cvSaveImage将图像保存至相应位置
}

void PixImage::Save_Image(char * savePath)
{
	if (!img)
		return;
	Save_RGB();

	char *s = new char[strlen(savePath) + strlen(ImageName) + 1];

	strcpy(s, savePath);
	strcat(s, ImageName);
	cvSaveImage(s, img);
}

void PixImage::Save_ImageGray()
{
	if (!img)return;
	Save_Gray(); //通过Save_Gray方法将Gray_Array中的值更新到实际图像中
	cvSaveImage(ImageName, imgGray); //调用cvSaveImage方法保存图像至相应目录
}

void PixImage::Save_ImageGray(char * savePath)
{
	if (!img)return;
	Save_Gray();
	char *s = new char[strlen(savePath) + strlen(ImageName) + 1];
	strcpy(s, savePath);
	strcat(s, ImageName);
	cvSaveImage(s, imgGray);
}

void PixImage::Load_RGB()
{
	for (int i = 0; i < ImageHeight; i++)
	{
		for (int j = 0; j < ImageWidth; j++)
		{
			CvScalar rgb = Get_RGB(i, j);
			RGB_Array[i][j][0] = rgb.val[0];
			RGB_Array[i][j][1] = rgb.val[1];
			RGB_Array[i][j][2] = rgb.val[2];
		}
	}
}

void PixImage::Save_RGB()
{
	for (int i = 0; i < ImageHeight; i++)
	{
		for (int j = 0; j < ImageWidth; j++)
		{
			Set_RGB(i, j, RGB_Array[i][j]);
		}
	}
}

void PixImage::Load_Gray(bool isDefault)
{
	for (int i = 0; i < ImageHeight; i++) 
	{
		for (int j = 0; j < ImageWidth; j++) 
		{
			Gray_Array[i][j] = getGray(i, j, isDefault);
		}
	}
}

void PixImage::Save_Gray()
{
	for (int i = 0; i < ImageHeight; i++)
	{
		for (int j = 0; j < ImageWidth; j++) 
		{
			Set_Gray(i, j, Gray_Array[i][j]);
		}
	}
}

void PixImage::Sum_Average_RGB(double * rgb, int ci, int cj, int n)
{
	int cnt = 0, sx, ex, sy, ey;
	double r, g, b;
	r = g = b = 0.0;
	//获取均值滤波器计算的起始行数,终止行数,起始列数,终止列数
	sx = sy = n % 2 == 0 ? -n / 2 + 1 : -n / 2;
	ex = ey = n / 2;
	sy += ci, ey += ci;
	sx += cj, ex += cj;
	for (int i = sy; i <= ey; i++) {
		for (int j = sx; j <= ex; j++) {
			if (i < 0 || i >= ImageHeight || j < 0 || j >= ImageWidth) 
			{
				cnt++; //记录超出图像范围的位置的数量
				continue;
			}
			r += Get_RGB(i, j, 0);
			g += Get_RGB(i, j, 1);
			b += Get_RGB(i, j, 2);
		}
	}
	int div = n*n - cnt; //获取实际计算在内的位置数量
						 //求取平均值
	rgb[0] = r / div;
	rgb[1] = g / div;
	rgb[2] = b / div;
}

void PixImage::Mid_Average_RGB(double * rgb, int ci, int cj, int n)
{
	int cnt = 0, sx, ex, sy, ey;
	double r, g, b;
	r = g = b = 0.0;
	//获取均值滤波器计算的起始行数,终止行数,起始列数,终止列数
	sx = sy = n % 2 == 0 ? -n / 2 + 1 : -n / 2;
	ex = ey = n / 2;
	sy += ci, ey += ci;
	sx += cj, ex += cj;
	vector<double> rvc;
	vector<double> gvc;
	vector<double> bvc;

	for (int i = sy; i <= ey; i++) {
		for (int j = sx; j <= ex; j++) {
			if (i < 0 || i >= ImageHeight || j < 0 || j >= ImageWidth)
			{
				cnt++; //记录超出图像范围的位置的数量
				continue;
			}
			rvc.push_back(Get_RGB(i, j, 0));
			gvc.push_back(Get_RGB(i, j, 1));
			bvc.push_back(Get_RGB(i, j, 2));
		}
	}
	int div = n*n - cnt; //获取实际计算在内的位置数量

	sort(rvc.begin(), rvc.end());
	sort(gvc.begin(), gvc.end());
	sort(bvc.begin(), bvc.end());

	int index_t = div / 2;//求取中值
	cout << index_t << endl;

	rgb[0] = rvc[index_t];
	rgb[1] = gvc[index_t];
	rgb[2] = bvc[index_t];
}


double PixImage::Gxy(int ci, int cj)
{
	double Gx = 0, Gy = 0; //Gx Gy初始化为0,用以记录x和y两个方向的梯度值
	int li, lj;
	for (int i = -1; i <= 1; i++) 
	{
		for (int j = -1; j <= 1; j++)
		{
			li = i + ci, lj = j + cj;
			if (li < 0 || li >= ImageHeight || lj <= 0 || lj >= ImageWidth) //记录超出图像范围的位置的数量
				continue;
			Gx += Sobel_OperatorX[i + 1][j + 1] * getGray(li, lj, grayDefault); //两矩阵对应位置相乘并求和
			Gy += Sobel_OperatorY[i + 1][j + 1] * getGray(li, lj, grayDefault); //两矩阵对应位置相乘并求和
		}
	}
	return sqrt(Gx*Gx + Gy*Gy); //返回最终的梯度估计值
}

double PixImage::Sobel_Thresh(double scale)
{
	double mean = 0;
	//遍历图像的每一个位置的灰度值,求取灰度平均值
	for (int i = 0; i < ImageHeight; i++) 
	{
		for (int j = 0; j < ImageWidth; j++)
		{
			mean += Gray_Array[i][j];
		}
	}
	mean /= (ImageHeight*ImageWidth);
	return sqrt(scale*mean); //根据公式计算sobel阈值
}

