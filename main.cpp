#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

//ͳ�����������ص��ܺ�
int getColSum(Mat src, int col)
{
	int sum = 0;
	int height = src.rows;
	int width = src.cols;
	for (int i = 0; i < height; i++)
	{
		sum = sum + src.at <uchar>(i, col);
	}
	return sum;
}

//ͳ�����������ص��ܺ�
int getRowSum(Mat src, int row)
{
	int sum = 0;
	int width = src.cols;
	for (int i = 0; i < width; i++)
	{
		sum += src.at <uchar>(row, i);
	}
	return sum;
}

//�����и�
void cutTop(Mat& src, Mat& dstImg)
{
	int top, bottom;
	top = 0;
	bottom = src.rows;

	int i;
	for (i = 0; i < src.rows; i++)
	{
		int colValue = getRowSum(src, i);//ͳ�����������ص��ܺ�
		//cout <<i<<" th "<< colValue << endl;
		if (colValue > 0)//ɨ��ֱ�������ص��ܺʹ���0ʱ�����µ�ǰλ��top
		{
			top = i;
			break;
		}
	}
	for (; i < src.rows; i++)
	{
		int colValue = getRowSum(src, i);//ͳ�����������ص��ܺ�
		//cout << i << " th " << colValue << endl;
		if (colValue == 0)//����ɨ��ֱ�������ص��ܺ͵���0ʱ�����µ�ǰλ��bottom
		{
			bottom = i;
			break;
		}
	}

	int height = bottom - top;
	Rect rect(0, top, src.cols, height);
	dstImg = src(rect).clone();
}

//�����и�
int cutLeft(Mat& src, Mat& leftImg, Mat& rightImg)
{
	int left, right;
	left = 0;
	right = src.cols;

	int i;
	for (i = 0; i < right; i++)
	{
		int colValue = getColSum(src, i);//ͳ�����������ص��ܺ�
		//cout <<i<<" th "<< colValue << endl;
		if (colValue > 0)//ɨ��ֱ�������ص��ܺʹ���0ʱ�����µ�ǰλ��left
		{
			left = i;
			break;
		}
	}
	if (left == 0)
	{
		return 1;
	}

	//����ɨ��
	for (; i < src.cols; i++)
	{
		int colValue = getColSum(src, i);//ͳ�����������ص��ܺ�
		//cout << i << " th " << colValue << endl;
		if (colValue == 0)//����ɨ��ֱ�������ص��ܺ͵���0ʱ�����µ�ǰλ��right
		{
			right = i;
			break;
		}
	}
	int width = right - left;//�ָ�ͼƬ�Ŀ����Ϊright - left
	Rect rect(left, 0, width, src.rows);//����һ�����Σ������ֱ�Ϊ������߶�����X���ꡢY���꣬�ұߵײ���X���ꡢY���꣨���Ͻ�����Ϊ0��0��
	leftImg = src(rect).clone();
	Rect rectRight(right, 0, src.cols - right, src.rows);//�ָ��ʣ�µ�ԭͼ
	rightImg = src(rectRight).clone();
	cutTop(leftImg, leftImg);//�����и�
	return 0;
}

//��ȡ�������ص��
void getPXSum(Mat& src, int& a)
{
	threshold(src, src, 100, 255, THRESH_BINARY);
	a = 0;
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			a += src.at <uchar>(i, j);
		}
	}
}

//����ʶ��
int  getSubtract(Mat& src, int TemplateNum) 
{
	Mat img_result;
	int min = 1000000;
	int serieNum = 0;
	for (int i = 0; i < TemplateNum; i++) {
		char name[100];
		sprintf_s(name, "D:\\visual studio\\work\\test\\image\\%dLeft.jpg", i);
		Mat Template = imread(name, IMREAD_GRAYSCALE);//��ȡģ��
		threshold(Template, Template, 100, 255, THRESH_BINARY);
		threshold(src, src, 100, 255, THRESH_BINARY);
		resize(src, src, Size(32, 48), 0, 0, INTER_LINEAR);
		resize(Template, Template, Size(32, 48), 0, 0, INTER_LINEAR);//�����ߴ�
		//imshow(name, Template);

		/*����Ҫƥ���ͼ�ֱ��10��ģ���Ӧ���ص�ֵ�����Ȼ���󷵻�ͼƬ������ͼƬ�����ص�ֵ��ƽ���ͣ����ĸ�ģ��ƥ��ʱ�򷵻�ͼƬ��ƽ������С��Ϳ��Եõ����*/
		absdiff(Template, src, img_result);//AbsDiff��OpenCV�м������������ľ���ֵ�ĺ�����
		int diff = 0;
		getPXSum(img_result, diff);//��ȡ�������ص��
		if (diff < min)//���ص�Ա�
		{
			min = diff;
			serieNum = i;
		}
	}

	printf("��С������%d ", min);
	printf("ƥ�䵽��%d��ģ��ƥ���������%d\n", serieNum, serieNum);
	return serieNum;
}

int main()
{
	Mat src = imread("test1.jpg", IMREAD_GRAYSCALE);//��ȡͼƬ
	threshold(src, src, 100, 255, 1);//��ֵ��
	imshow("��ֵ��", src);//��ʾ��ֵ����ͼƬ
	//cout << src;
	Mat leftImg, rightImg;
	int res = cutLeft(src, leftImg, rightImg);
	int i = 0;
	while (res == 0)
	{
		char nameLeft[10];
		sprintf_s(nameLeft, "%dLeft", i++);
		//imshow(nameLeft, leftImg);//��ʾ�ָ���ͼƬ

		/*����ָ�ͼƬ��Ϊʶ��ģ��*/
		//stringstream ss;
		//ss << nameLeft;
		//imwrite("D:\\visual studio\\work\\test\\image\\" + ss.str() + ".jpg", leftImg);//�ѷָ�ͼƬ����
		//ss >> nameLeft;

		Mat srcTmp = rightImg;
		getSubtract(leftImg, 10);//��������ʶ��
		res = cutLeft(srcTmp, leftImg, rightImg);
	}

	waitKey(0);
	return 0;
}
