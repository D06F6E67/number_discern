#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

//统计所有列像素的总和
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

//统计所有行像素的总和
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

//上下切割
void cutTop(Mat& src, Mat& dstImg)
{
	int top, bottom;
	top = 0;
	bottom = src.rows;

	int i;
	for (i = 0; i < src.rows; i++)
	{
		int colValue = getRowSum(src, i);//统计所有行像素的总和
		//cout <<i<<" th "<< colValue << endl;
		if (colValue > 0)//扫描直到行像素的总和大于0时，记下当前位置top
		{
			top = i;
			break;
		}
	}
	for (; i < src.rows; i++)
	{
		int colValue = getRowSum(src, i);//统计所有行像素的总和
		//cout << i << " th " << colValue << endl;
		if (colValue == 0)//继续扫描直到行像素的总和等于0时，记下当前位置bottom
		{
			bottom = i;
			break;
		}
	}

	int height = bottom - top;
	Rect rect(0, top, src.cols, height);
	dstImg = src(rect).clone();
}

//左右切割
int cutLeft(Mat& src, Mat& leftImg, Mat& rightImg)
{
	int left, right;
	left = 0;
	right = src.cols;

	int i;
	for (i = 0; i < right; i++)
	{
		int colValue = getColSum(src, i);//统计所有列像素的总和
		//cout <<i<<" th "<< colValue << endl;
		if (colValue > 0)//扫描直到列像素的总和大于0时，记下当前位置left
		{
			left = i;
			break;
		}
	}
	if (left == 0)
	{
		return 1;
	}

	//继续扫描
	for (; i < src.cols; i++)
	{
		int colValue = getColSum(src, i);//统计所有列像素的总和
		//cout << i << " th " << colValue << endl;
		if (colValue == 0)//继续扫描直到列像素的总和等于0时，记下当前位置right
		{
			right = i;
			break;
		}
	}
	int width = right - left;//分割图片的宽度则为right - left
	Rect rect(left, 0, width, src.rows);//构造一个矩形，参数分别为矩形左边顶部的X坐标、Y坐标，右边底部的X坐标、Y坐标（左上角坐标为0，0）
	leftImg = src(rect).clone();
	Rect rectRight(right, 0, src.cols - right, src.rows);//分割后剩下的原图
	rightImg = src(rectRight).clone();
	cutTop(leftImg, leftImg);//上下切割
	return 0;
}

//获取所有像素点和
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

//数字识别
int  getSubtract(Mat& src, int TemplateNum) 
{
	Mat img_result;
	int min = 1000000;
	int serieNum = 0;
	for (int i = 0; i < TemplateNum; i++) {
		char name[100];
		sprintf_s(name, "D:\\visual studio\\work\\test\\image\\%dLeft.jpg", i);
		Mat Template = imread(name, IMREAD_GRAYSCALE);//读取模板
		threshold(Template, Template, 100, 255, THRESH_BINARY);
		threshold(src, src, 100, 255, THRESH_BINARY);
		resize(src, src, Size(32, 48), 0, 0, INTER_LINEAR);
		resize(Template, Template, Size(32, 48), 0, 0, INTER_LINEAR);//调整尺寸
		//imshow(name, Template);

		/*让需要匹配的图分别和10个模板对应像素点值相减，然后求返回图片的整个图片的像素点值得平方和，和哪个模板匹配时候返回图片的平方和最小则就可以得到结果*/
		absdiff(Template, src, img_result);//AbsDiff，OpenCV中计算两个数组差的绝对值的函数。
		int diff = 0;
		getPXSum(img_result, diff);//获取所有像素点和
		if (diff < min)//像素点对比
		{
			min = diff;
			serieNum = i;
		}
	}

	printf("最小距离是%d ", min);
	printf("匹配到第%d个模板匹配的数字是%d\n", serieNum, serieNum);
	return serieNum;
}

int main()
{
	Mat src = imread("test1.jpg", IMREAD_GRAYSCALE);//读取图片
	threshold(src, src, 100, 255, 1);//二值化
	imshow("二值化", src);//显示二值化后图片
	//cout << src;
	Mat leftImg, rightImg;
	int res = cutLeft(src, leftImg, rightImg);
	int i = 0;
	while (res == 0)
	{
		char nameLeft[10];
		sprintf_s(nameLeft, "%dLeft", i++);
		//imshow(nameLeft, leftImg);//显示分割后的图片

		/*保存分割图片作为识别模板*/
		//stringstream ss;
		//ss << nameLeft;
		//imwrite("D:\\visual studio\\work\\test\\image\\" + ss.str() + ".jpg", leftImg);//把分割图片保存
		//ss >> nameLeft;

		Mat srcTmp = rightImg;
		getSubtract(leftImg, 10);//进行数字识别
		res = cutLeft(srcTmp, leftImg, rightImg);
	}

	waitKey(0);
	return 0;
}
