#include "depthVol.h"
#include "GenImg4Dense.h"
using namespace std;
void drawTri() {
	OpenGlInit glObj;
	glObj.Init3D(1);
	glObj.renderOneFrame();
	float* imagP = glObj.clipScreen();
	string fileName = "denseImg//tri";
	Mat img1 = opencvUtil::saveaimg(imagP, fileName.c_str());
}

void calDepth() {
	Mat img = imread("denseImg\\den4.jpg", IMREAD_GRAYSCALE);
	img.convertTo(img, CV_32FC1);
	Mat imgOther = imread("denseImg\\den5.jpg", IMREAD_GRAYSCALE);
	imgOther.convertTo(imgOther, CV_32FC1);
	depthVol obj;
	obj.img = img;
	obj.imgOther = imgOther;
	obj.init();
	obj.cal();
}

void drawBall() {
	GenImg4Dense objGen;
	objGen.gen();
}

void tryMulMat() {
	int sz[] = { 3, 4, 5};
	Mat depthErrors(3, sz, CV_32SC1, Scalar::all(1));
	float n = 0;
	for (int i = 0; i < depthErrors.size[0]; i++) {
		for (int j = 0; j < depthErrors.size[1]; j++)
		{
			for (int k = 0; k < depthErrors.size[2]; k++)
			{
				n++;
				depthErrors.at<float>(i, j, k) = n;
			}
		}
	}
	Mat unit(1, 10, CV_32FC1, depthErrors.ptr<float>(1,1,1));
	cout << depthErrors.at<Mat>(1,1) << endl;
}

int main(void)
{
	//drawTri();
	//drawBall();
	//calDepth();
	tryMulMat();
	waitKey(0);
}
