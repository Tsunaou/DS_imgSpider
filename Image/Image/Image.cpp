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
	cout << "�����빦��ѡ��0����ģ����1������������" << endl;
	while (cin >> swi)
	{
		if (swi == 0)
		{
			cout << "ѡ��ģ������:" << endl;
			cout << "����������ģ����Χ n �͵������� m��" << endl;
			cin >> n >> m;
			img.Blurring(n,m);
			img.Save_Image("�����\\");
			cout << "ģ���ɹ�" << endl;
		}
		else  if (swi == 1)
		{
			cout << "��Ե����⹦��:" << endl;
			cout << "�����������ϵ��scale��" << endl;
			cin >>scale;
			img.Sobel(scale);
			img.Save_ImageGray("�����\\");
			cout << "��Ե�����ɹ�" << endl;

		}
		else
		{
			cout << "�����������������롣" << endl;
		}
	}

	return 0;
}
