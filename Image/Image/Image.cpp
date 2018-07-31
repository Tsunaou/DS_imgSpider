#include "PixImage.h"

using namespace std;
#define _CRT_SECURE_NO_WARNINGS

const int MAX = 5000;
double RGB_Array[MAX][MAX][3];
double Gray_Array[MAX][MAX];

int main()
{
	PixImage img("5.jpg", true);
	int swi = 0;
	int m = 0, n = 0, scale = 0;
	cout << "请输入功能选择：0代表模糊，1代表索贝尔：" << endl;
	while (cin >> swi)
	{
		if (swi == 0)
		{
			cout << "选择模糊功能:" << endl;
			cout << "请依次输入模糊范围 n 和迭代次数 m：" << endl;
			cin >> n >> m;
			img.Blurring(n,m);
			img.Save_Image("处理后\\");
			cout << "模糊成功" << endl;
		}
		else  if (swi == 1)
		{
			cout << "边缘化检测功能:" << endl;
			cout << "请输出索贝尔系数scale：" << endl;
			cin >>scale;
			img.Sobel(scale);
			img.Save_ImageGray("处理后\\");
			cout << "边缘化检测成功" << endl;

		}
		else
		{
			cout << "输入有误，请重新输入。" << endl;
		}
	}

	return 0;
}
