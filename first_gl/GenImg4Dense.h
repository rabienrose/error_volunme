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
class GenImg4Dense {
public:
	void gen() {
		float amp = 0.5;
		Mat camX = (Mat_<float>(9, 1) << -1, 0, 1, -1, 0, 1, -1, 0, 1);
		camX = camX*amp;
		Mat camY = (Mat_<float>(9, 1) << -1, -1, -1, 0, 0, 0, 1, 1, 1);
		camY = camY*amp;
		
		OpenGlInit glObj;
		glObj.Init3D(2);
		for (int i = 0; i < 9; i++) {
			//cout << camX.at<float>(i) <<":"<< camY.at<float>(i) << endl;
			glObj.setCam(camX.at<float>(i), camY.at<float>(i), 3);
			//glObj.moveCam(camX.at<float>(i), camY.at<float>(i), 0);
			glObj.renderOneFrame();
			float* imagP = glObj.clipScreen();
			string fileName = "denseImg//den";
			string strI = to_string(i);
			fileName= fileName+ strI;
			Mat img1 = opencvUtil::saveaimg(imagP, fileName.c_str());
			
		}
	}
};
