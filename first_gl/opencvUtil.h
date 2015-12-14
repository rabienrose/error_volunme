#pragma once
#include "commonHeader.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
using namespace std;
class opencvUtil 
{
public:
	static cv::Mat saveaimg(float* imgp, string name) {
		cv::Mat img;
		img = cv::Mat(win_h_c, win_w_c, CV_32FC3, imgp).clone();
		img = img * 255;
		img.convertTo(img, CV_8UC3);
		cv::cvtColor(img, img, CV_RGB2BGR);
		cv::flip(img, img, 0);
		cv::imwrite(name + ".jpg", img);
		return img;
	}
};
