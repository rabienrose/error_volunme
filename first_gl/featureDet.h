#pragma once
#include "opencvUtil.h"
using namespace cv;
using namespace std;
class featureDet {
public:
	static void advScan() {
		Mat img = imread("loli.jpg", IMREAD_GRAYSCALE);
		Mat imgDeriX;
		Mat imgDeriY;
		Mat deriX, deriY;
		Mat deriXT, deriYT;
		//getDerivKernels(deriX, deriY, 1, 0, 5, false, CV_32FC1);
		Mat gausMat;
		//getGaussianFilter(gausMat,);
		transpose(deriX, deriXT);
		transpose(deriY, deriYT);
		deriX = deriX*deriXT;
		deriY = deriY*deriYT;
		
		filter2D(img, imgDeriX, CV_32FC1, deriX);
		filter2D(img, imgDeriY, CV_32FC1, deriY);
		cout << deriX << endl;
		cout << deriY << endl;
		Mat combiOutput;
		hconcat(imgDeriX, imgDeriY, combiOutput);
		combiOutput = combiOutput / 100;
		combiOutput.convertTo(combiOutput, CV_8UC1);
		imshow("chamo",combiOutput);
			
	}

	static void rawScan() {
		Mat img = imread("loli.jpg", IMREAD_GRAYSCALE);
		cout << img.type()<<endl;
		int patchSizeX =5;
		int patchSizeY = 5;
		vector<Mat> diffs;
		int nn = 0;
		diffs.resize((patchSizeX*2+1)*(patchSizeX*2+1));
		for (int i = -patchSizeY; i <= patchSizeY; i++) {
			for (int j = -patchSizeX; j <= patchSizeX; j++) {
				Range base_col, base_row, offset_col, offset_row;
				if (i >= 0 && j >= 0) {
					base_col.start = j; base_col.end = img.cols;
					base_row.start = i; base_row.end = img.rows;
					offset_col.start = 0; offset_col.end = img.cols-j;
					offset_row.start = 0; offset_row.end = img.rows-i;
				}
				else if (i < 0 && j >= 0) {
					base_col.start = j; base_col.end = img.cols;
					base_row.start = 0; base_row.end = img.rows + i;
					offset_col.start = 0; offset_col.end = img.cols - j;
					offset_row.start = -i; offset_row.end = img.rows;
				}
				else if (i >= 0 && j < 0) {
					base_col.start = 0; base_col.end = img.cols + j;
					base_row.start = i; base_row.end = img.rows;
					offset_col.start = -j; offset_col.end = img.cols;
					offset_row.start = 0; offset_row.end = img.rows - i;
				}
				else if (i < 0 && j < 0) {
					base_col.start = 0; base_col.end = img.cols + j;
					base_row.start = 0; base_row.end = img.rows + i;
					offset_col.start = -j; offset_col.end = img.cols;
					offset_row.start = -i; offset_row.end = img.rows;
				}
				diffs[nn] = img(base_row, base_col) - img(offset_row, offset_col);
				nn++;
			}
		}
		Mat AC(img.rows - patchSizeY, img.cols - patchSizeX,CV_32FC1, Scalar(0));
		Mat GausK= getGaussianKernel(patchSizeY * 2 + 1, 1, CV_32FC1);
		Mat GausKT;
		transpose(GausK, GausKT);
		GausK = GausK*GausKT;
		cout << GausK << endl;
		for (int i = 0; i < diffs.size(); i++) {
			Mat temp;
			diffs[i](Range(0, img.rows - patchSizeY), Range(0, img.cols - patchSizeX)).convertTo(temp, CV_32FC1);
			AC = AC + GausK.at<float>(i)* temp;
		}
		//AC = AC / diffs.size();
		AC.convertTo(AC, CV_8UC1);
		imshow("chamo", AC);
	}
};
