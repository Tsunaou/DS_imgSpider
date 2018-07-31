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
	CvScalar rgb; //����һ��CvScalar����,���ֱ�ȡ��rgbֵȻ��ֵ
	rgb.val[0] = value[0];
	rgb.val[1] = value[1];
	rgb.val[2] = value[2];
	cvSet2D(img, i, j, rgb); //opencv�Դ���������������imgͼ���е�i�е�j�д�������Ϊrgb
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

double PixImage::getGray(int i, int j, bool isDefault)//isDefault:�Ƿ���opencvĬ�ϵĻҶȻ�ȡ������ȡͼ��Ҷ�ֵ
{
	if (!isDefault)
		return (RGB_Array[i][j][0] * 299 +RGB_Array[i][j][1] * 587 +RGB_Array[i][j][2] * 114 + 500) / 1000; //rgbת�Ҷ�ֵ��ʽ
	else
		return cvGet2D(imgGray, i, j).val[0]; //opencv��iscolor=0�ķ�ʽ��ȡ���ĻҶ�ֵ
}

void PixImage::Blurring(int n, int m)//n:��ֵ������ȡ�ķ�Χn��,m:����ĵ�������
{
	if (!img)
		return;
	double *rgb = new double[3];

	while (m > 0) //����ֵ�˲�������m��
	{ 
		for (int i = 0; i < ImageHeight; i++)
		{
			for (int j = 0; j < ImageWidth; j++)
			{
				Sum_Average_RGB(rgb, i, j, n); //�ֱ��ȡ��i�е�j��������Χn*n�����ص�rgbƽ��ֵ
				RGB_Array[i][j][0] = rgb[0];
				RGB_Array[i][j][1] = rgb[1];
				RGB_Array[i][j][2] = rgb[2];
			}
		}
		Save_RGB(); //����Save_RGB��������ͼ��
		m--;
	}
}

void PixImage::Sobel(double scale)//scale:scaleϵ��
{
	if (!img)
		return;

	double t = Sobel_Thresh(scale); //���ݸ����Ĺ�ʽ����sobel��ֵ

	for (int i = 1; i < ImageHeight - 1; i++) 
	{
		for (int j = 1; j < ImageWidth - 1; j++) 
		{
			if (Gxy(i, j) >= t)//���ݶȴ��ڵ�����ֵ������Ϊ255
				Gray_Array[i][j] = 255; 
			else //���ݶ�С����ֵ������Ϊ0
				Gray_Array[i][j] = 0; 
		}
	}
}

void PixImage::Save_Image()
{
	if (!img)
		return;
	Save_RGB(); //����Save_RGB�ķ�����RGB_Array�е�ֵȫ�����µ�ʵ��ͼ����
	cvSaveImage(ImageName, img); //����opencv�Դ���cvSaveImage��ͼ�񱣴�����Ӧλ��
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
	Save_Gray(); //ͨ��Save_Gray������Gray_Array�е�ֵ���µ�ʵ��ͼ����
	cvSaveImage(ImageName, imgGray); //����cvSaveImage��������ͼ������ӦĿ¼
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
	//��ȡ��ֵ�˲����������ʼ����,��ֹ����,��ʼ����,��ֹ����
	sx = sy = n % 2 == 0 ? -n / 2 + 1 : -n / 2;
	ex = ey = n / 2;
	sy += ci, ey += ci;
	sx += cj, ex += cj;
	for (int i = sy; i <= ey; i++) {
		for (int j = sx; j <= ex; j++) {
			if (i < 0 || i >= ImageHeight || j < 0 || j >= ImageWidth) 
			{
				cnt++; //��¼����ͼ��Χ��λ�õ�����
				continue;
			}
			r += Get_RGB(i, j, 0);
			g += Get_RGB(i, j, 1);
			b += Get_RGB(i, j, 2);
		}
	}
	int div = n*n - cnt; //��ȡʵ�ʼ������ڵ�λ������
						 //��ȡƽ��ֵ
	rgb[0] = r / div;
	rgb[1] = g / div;
	rgb[2] = b / div;
}

void PixImage::Mid_Average_RGB(double * rgb, int ci, int cj, int n)
{
	int cnt = 0, sx, ex, sy, ey;
	double r, g, b;
	r = g = b = 0.0;
	//��ȡ��ֵ�˲����������ʼ����,��ֹ����,��ʼ����,��ֹ����
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
				cnt++; //��¼����ͼ��Χ��λ�õ�����
				continue;
			}
			rvc.push_back(Get_RGB(i, j, 0));
			gvc.push_back(Get_RGB(i, j, 1));
			bvc.push_back(Get_RGB(i, j, 2));
		}
	}
	int div = n*n - cnt; //��ȡʵ�ʼ������ڵ�λ������

	sort(rvc.begin(), rvc.end());
	sort(gvc.begin(), gvc.end());
	sort(bvc.begin(), bvc.end());

	int index_t = div / 2;//��ȡ��ֵ
	cout << index_t << endl;

	rgb[0] = rvc[index_t];
	rgb[1] = gvc[index_t];
	rgb[2] = bvc[index_t];
}


double PixImage::Gxy(int ci, int cj)
{
	double Gx = 0, Gy = 0; //Gx Gy��ʼ��Ϊ0,���Լ�¼x��y����������ݶ�ֵ
	int li, lj;
	for (int i = -1; i <= 1; i++) 
	{
		for (int j = -1; j <= 1; j++)
		{
			li = i + ci, lj = j + cj;
			if (li < 0 || li >= ImageHeight || lj <= 0 || lj >= ImageWidth) //��¼����ͼ��Χ��λ�õ�����
				continue;
			Gx += Sobel_OperatorX[i + 1][j + 1] * getGray(li, lj, grayDefault); //�������Ӧλ����˲����
			Gy += Sobel_OperatorY[i + 1][j + 1] * getGray(li, lj, grayDefault); //�������Ӧλ����˲����
		}
	}
	return sqrt(Gx*Gx + Gy*Gy); //�������յ��ݶȹ���ֵ
}

double PixImage::Sobel_Thresh(double scale)
{
	double mean = 0;
	//����ͼ���ÿһ��λ�õĻҶ�ֵ,��ȡ�Ҷ�ƽ��ֵ
	for (int i = 0; i < ImageHeight; i++) 
	{
		for (int j = 0; j < ImageWidth; j++)
		{
			mean += Gray_Array[i][j];
		}
	}
	mean /= (ImageHeight*ImageWidth);
	return sqrt(scale*mean); //���ݹ�ʽ����sobel��ֵ
}

