#include <stdint.h>
#include "fast.hpp"

namespace fovis
{

	void FAST(uint8_t* image, int width, int height, int row_stride,
			std::vector<KeyPoint>* keypoints, int threshold, bool nonmax_suppression )
	{
		std::vector<cv::KeyPoint> cvKeypoints;
		cv::Mat frameCPU(height,width,CV_8UC1,image);
		if (cv::gpu::getCudaEnabledDeviceCount() > 0)
		{
			cv::gpu::GpuMat frameGPU(height,width,CV_8UC1); //initialize mats

			frameGPU.upload(frameCPU); //copy frame from cpu to gpu

			cv::gpu::GpuMat mask(height,width,CV_8UC1);
			mask.setTo(cv::Scalar(255,255,255)); //create a mask to run the detection on the whole image

			cv::gpu::FAST_GPU FASTObject(threshold,nonmax_suppression); //run the detection
			FASTObject(frameGPU,mask,cvKeypoints);
		}
		else
		{
			FAST(frameCPU, cvKeypoints, threshold, nonmax_suppression);
		}

		keypoints->clear();
		for(uint i = 0; i < cvKeypoints.size(); i++) { //the pointers here are so ugly please
			keypoints->push_back(fovis::KeyPoint()); //don't read these lines
			(*keypoints)[i].u = cvKeypoints[i].pt.x; //just follow the comments
			(*keypoints)[i].v = cvKeypoints[i].pt.y; //because if you read them all the 
			(*keypoints)[i].score = cvKeypoints[i].response; //way you can skip reading this section of code
		}
	}

}
