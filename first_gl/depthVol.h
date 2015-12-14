#pragma once
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "OpenGlInit.h"
#include "opencvUtil.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
using namespace std;
using namespace cv;
class depthVol {
public:
	Mat transMat;
	Mat K;
	Mat KInv;
	Mat img;
	Mat imgOther;
	float fx = 240/ 0.5579;
	float fy = fx;
	float cx = 160;
	float cy = 120;
	float depthRangeMin = 2;
	float depthRangeMax = 4;
	float depthStepSize = 0.2;
	float unavailbleDepth = 1000;
	void init() {
		//KInv = (Mat_<float>(3, 3) << fx, 0, cx,
		//						     0, fy, cy,
		//						     0, 0, 1,
		//						     0, 0, 0);
		K = (Mat_<float>(4, 4) << fx, 0, cx, 0,
								  0, fy, cy, 0,
								  0, 0, 1, 0,
								  0, 0, 0, 1);
		KInv = K.inv();
		transMat = (Mat_<float>(4, 4) << 1, 0, 0, -0.5,
										 0, 1, 0, 0,
										 0, 0, 1, 0,
										 0, 0, 0, 1);
	}
	void cal() {
		int sz[] = { img.rows, img.cols, ceil((depthRangeMax - depthRangeMin)/ depthStepSize) };
		Mat depthErrors(3, sz, CV_32FC1, Scalar::all(unavailbleDepth));
		for (int i = 0; i < img.rows; i++) {
			for (int j = 0; j < img.cols; j++) {
				float x = j;
				float y = i;
				int depthIndex = 0;
				for (float d = depthRangeMin; d <= depthRangeMax; d=d+ depthStepSize){
					Mat addOther = transToOther(x,y,d, transMat);
					int yOther = round(addOther.at<float>(1, 0));
					int xOther = round(addOther.at<float>(0, 0));
					if (yOther < 0 || yOther >= img.rows || xOther < 0 || xOther >= img.cols) {
					}
					else {
						float err = abs(img.at<float>(i, j) - imgOther.at<float>(yOther, xOther));
						depthErrors.at<float>(i, j, depthIndex) = err;
					}
					
				}
				depthIndex++;
			}
		}
		Mat ss = depthErrors.at<Mat>(120, 160);
		cout << ss << endl;
		int a = 1;

	}

	Mat transToOther(float x, float y, float d,  Mat transMat) {
		Mat oriXY = (Mat_<float>(4, 1) << x, y,1,0);
		Mat camLoc = KInv*oriXY;
		camLoc = camLoc * d;
		camLoc.at<float>(3, 0) = 1;
		Mat camAddOther = transMat*camLoc;
		Mat imgAddOther = K*camAddOther;
		imgAddOther.at<float>(0, 0) = imgAddOther.at<float>(0, 0) / imgAddOther.at<float>(2, 0);
		imgAddOther.at<float>(1, 0) = imgAddOther.at<float>(1, 0) / imgAddOther.at<float>(2, 0);
		return imgAddOther;
	}
};
