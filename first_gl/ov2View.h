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
class vo2View {
private:
	float nn_match_ratio = 0.8f;   // Nearest neighbor matching ratio
	int max_iter = 1000;
	float lamda = 0.0;
	float step = 1;
public:
	void genImg() {
		OpenGlInit glObj;
		glObj.Init3D();
		glObj.renderOneFrame();
		float* imagP = glObj.clipScreen();
		Mat img1 = opencvUtil::saveaimg(imagP, "a1");
		glObj.moveCam(0.3, 0, 0);
		glObj.renderOneFrame();
		imagP = glObj.clipScreen();
		Mat img2 = opencvUtil::saveaimg(imagP, "a2");
	}
	void exec() {
		Mat img1 = imread("loli.jpg", IMREAD_GRAYSCALE);
		Mat img2 = imread("loli2.jpg", IMREAD_GRAYSCALE);

		vector<KeyPoint> kpts1, kpts2;
		Mat desc1, desc2;

		Ptr<AKAZE> akaze = AKAZE::create();
		akaze->detectAndCompute(img1, noArray(), kpts1, desc1);
		akaze->detectAndCompute(img2, noArray(), kpts2, desc2);

		BFMatcher matcher(NORM_HAMMING);
		vector< vector<DMatch> > nn_matches;
		matcher.knnMatch(desc1, desc2, nn_matches, 2);

		vector<KeyPoint> matched1, matched2;
		vector<DMatch> good_matches;
		for (size_t i = 0; i < nn_matches.size(); i++) {
			DMatch first = nn_matches[i][0];
			float dist1 = nn_matches[i][0].distance;
			float dist2 = nn_matches[i][1].distance;

			if (dist1 < nn_match_ratio * dist2) {
				int new_i = static_cast<int>(matched1.size());
				matched1.push_back(kpts1[first.queryIdx]);
				matched2.push_back(kpts2[first.trainIdx]);
				good_matches.push_back(DMatch(new_i, new_i, 0));
			}
		}
		Mat res;
		drawMatches(img1, matched1, img2, matched2, good_matches, res);
		imwrite("res.png", res);
		cout << matched1.size() << endl;

		float x1 = matched1[0].pt.x;
		float y1 = matched1[0].pt.y;
		float x2 = matched1[1].pt.x;
		float y2 = matched1[1].pt.y;
		float x11 = matched2[0].pt.x;
		float y11 = matched2[0].pt.y;
		float x22 = matched2[1].pt.x;
		float y22 = matched2[1].pt.y;
		Mat A = (Mat_<float>(4, 4) << 1, 0, x1, -y1, 0, 1, y1, x1, 1, 0, x2, -y2, 0, 1, y2, x2);
		Mat b = (Mat_<float>(4, 1) << x11 - x1, y11 - y1, x22 - x2, y22 - y2);
		Mat P = A.inv()*b;
		//P.at<float>(0, 0) = 30; P.at<float>(1, 0) = 20; P.at<float>(2, 0) = 0; P.at<float>(3, 0) = 0;
		float last_r=0;
		for (int j = 0; j < max_iter; j++) {
			Mat culJ = Mat::zeros(4, 4, CV_32FC1);
			Mat culb = Mat::zeros(4, 1, CV_32FC1);
			float culr = 0;
			for (int i = 0; i < matched1.size(); i++) {
				float x = matched1[i].pt.x;
				float y = matched1[i].pt.y;
				float xx = matched2[i].pt.x;
				float yy = matched2[i].pt.y;
				Mat J = (Mat_<float>(2, 4) << 1, 0, x, -y, 0, 1, y, x);
				float a = P.at<float>(2);
				float b = P.at<float>(3);
				float tx = P.at<float>(0);
				float ty = P.at<float>(1);
				Mat T = (Mat_<float>(2, 3) << 1+a, -b, tx, b, 1+a, ty);
				//cout << T << endl;
				Mat x_m = (Mat_<float>(3, 1) << x, y,1);
				Mat xx_m = (Mat_<float>(2, 1) << xx, yy);
				//cout << x_m << endl;
				//cout << xx_m << endl;
				Mat r = xx_m - T*x_m;
				//cout << T*x_m << endl;
				culr = culr + norm(r);
				Mat transJ;
				transpose(J, transJ);
				culb = culb + transJ*r;
				culJ = culJ + transJ*J;
			}
			if (abs(culr- last_r)<0.1) {
				break;
			}
			last_r = culr;
			cout << j << " " << culr << endl;
			Mat diag_t = Mat::eye(4, 4, CV_32FC1);
			diag_t = diag_t.mul(culJ)*lamda;
			Mat A_tmep = culJ+ diag_t;
			Mat deltaP = A_tmep.inv()*culb;
			Mat TransP;
			transpose(P, TransP);
			Mat TransDeltaP;
			transpose(deltaP, TransDeltaP);
			cout << TransP << endl;
			cout << TransDeltaP << endl;	
			P = P + deltaP*step;
			
		}
		

		



	}
};
